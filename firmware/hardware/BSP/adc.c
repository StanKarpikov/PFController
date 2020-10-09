/**
 * @file adc.c
 * @author Stanislav Karpikov
 * @brief Board support package: ADC
 */

/** @addtogroup hdw_bsp_adc
 * @{
 */
 
/*--------------------------------------------------------------
                       INCLUDES
--------------------------------------------------------------*/

#include "BSP/adc.h"

#include "BSP/bsp.h"
#include "BSP/timer.h"
#include "defines.h"
#include "stm32f7xx_hal.h"

/*--------------------------------------------------------------
                       PRIVATE DATA
--------------------------------------------------------------*/

static ADC_HandleTypeDef hadc = {0};     /**< ADC hardware handle */
static DMA_HandleTypeDef hdma_adc = {0}; /**< ADC DMA hardware handle */

static ADC_TRANSFER_CALLBACK adc_cplt_callback = 0;      /**< ADC DMA full complete callback */
static ADC_TRANSFER_CALLBACK adc_half_cplt_callback = 0; /**< ADC DMA half complete callback */

/*--------------------------------------------------------------
                       PRIVATE FUNCTIONS
--------------------------------------------------------------*/

/*
 * @brief Set callbacks for the ADC module
 *
 * @param cptl_callback ADC DMA full complete callback
 * @param half_cplt_callback ADC DMA half complete callback
 *
 * @return The status of the operation
 */
status_t adc_register_callbacks(ADC_TRANSFER_CALLBACK cptl_callback, ADC_TRANSFER_CALLBACK half_cplt_callback)
{
    adc_cplt_callback = cptl_callback;
    adc_half_cplt_callback = half_cplt_callback;
    return PFC_SUCCESS;
}

/**
 * @brief HAL ADC conversion complete callback (overrides the weak one)
 *
 * @param hadc ADC hardware handle
 */
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
    if (adc_cplt_callback) adc_cplt_callback();
}

/**
 * @brief HAL ADC conversion half complete callback (overrides the weak one)
 *
 * @param hadc ADC hardware handle
 */
void HAL_ADC_ConvHalfCpltCallback(ADC_HandleTypeDef* hadc)
{
    if (adc_half_cplt_callback) adc_half_cplt_callback();
}

/*
 * @brief Start the ADC DMA conversion 
 *
 * @param buffer A buffer for the data
 * @param buffer_size The buffer size
 *
 * @return The status of the operation
 */
status_t adc_start(uint32_t* buffer, uint32_t buffer_size)
{
    HAL_ADC_Start(&hadc);
    HAL_ADC_Start_DMA(&hadc, (uint32_t*)buffer, buffer_size);
    return PFC_SUCCESS;
}

/*
 * @brief Stop the ADC DMA conversion
 *
 * @return The status of the operation
 */
status_t adc_stop(void)
{
    HAL_ADC_Stop_DMA(&hadc);

    return PFC_SUCCESS;
}

/**
 * @brief ADC MSP Initialization callback (overrides the default one)
 * This function configures the hardware resources
 *
 * @param hadc ADC handle pointer
 */
void HAL_ADC_MspInit(ADC_HandleTypeDef* hadc)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    if (hadc->Instance == ADC_ID)
    {
        /* Peripheral clock enable */
        ADC_CLK_ENABLE();

        __HAL_RCC_GPIOC_CLK_ENABLE();
        __HAL_RCC_GPIOA_CLK_ENABLE();
        __HAL_RCC_GPIOB_CLK_ENABLE();

        GPIO_InitStruct.Pin = U_DC_ADC_Pin | A_HALF_ADC_Pin | B_HALF_ADC_Pin | C_HALF_ADC_Pin | A_EDC_ADC_Pin | B_EDC_ADC_Pin;
        GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

        GPIO_InitStruct.Pin = I_ADC_PFC_A_Pin | I_ADC_PFC_B_Pin | I_ADC_PFC_C_Pin | I_ADC_EFMC_Pin | TEST_1_Pin | TEST_2_Pin;
        GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

        GPIO_InitStruct.Pin = C_EDC_ADC_Pin | I_EDC_OUT_Pin;
        GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

        /* ADC1 DMA Init */
        /* ADC1 Init */
        hdma_adc.Instance = ADC_DMA_STREAM;
        hdma_adc.Init.Channel = ADC_DMA_CHANNEL;
        hdma_adc.Init.Direction = DMA_PERIPH_TO_MEMORY;
        hdma_adc.Init.PeriphInc = DMA_PINC_DISABLE;
        hdma_adc.Init.MemInc = DMA_MINC_ENABLE;
        hdma_adc.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
        hdma_adc.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
        hdma_adc.Init.Mode = DMA_NORMAL;
        hdma_adc.Init.Priority = DMA_PRIORITY_LOW;
        hdma_adc.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
        if (HAL_DMA_Init(&hdma_adc) != HAL_OK)
        {
            error_handler();
        }

        __HAL_LINKDMA(hadc, DMA_Handle, hdma_adc);
    }
}

/**
 * @brief ADC MSP De-Initialization (overrides the default one)
 * This function freeze the hardware resources
 *
 * @param hadc ADC handle pointer
 */
void HAL_ADC_MspDeInit(ADC_HandleTypeDef* hadc)
{
    if (hadc->Instance == ADC1)
    {
        ADC_CLK_DISABLE();

        HAL_GPIO_DeInit(GPIOC, U_DC_ADC_Pin | A_HALF_ADC_Pin | B_HALF_ADC_Pin | C_HALF_ADC_Pin | A_EDC_ADC_Pin | B_EDC_ADC_Pin);
        HAL_GPIO_DeInit(GPIOA, I_ADC_PFC_A_Pin | I_ADC_PFC_B_Pin | I_ADC_PFC_C_Pin | I_ADC_EFMC_Pin | TEST_1_Pin | TEST_2_Pin);
        HAL_GPIO_DeInit(GPIOB, C_EDC_ADC_Pin | I_EDC_OUT_Pin);

        /* ADC1 DMA DeInit */
        HAL_DMA_DeInit(hadc->DMA_Handle);
    }
}

/*
 * @brief ADC Initialization Function
 *
 * @return The status of the operation
 */
status_t adc_init(void)
{
    ADC_ChannelConfTypeDef sConfig = {0};

    /**Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion) */
    hadc.Instance = ADC_ID;
    hadc.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
    hadc.Init.Resolution = ADC_RESOLUTION_12B;
    hadc.Init.ScanConvMode = ADC_SCAN_ENABLE;
    hadc.Init.ContinuousConvMode = DISABLE;
    hadc.Init.DiscontinuousConvMode = DISABLE;
    hadc.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_RISING;
    hadc.Init.ExternalTrigConv = ADC_EXTERNALTRIGCONV_T2_TRGO;
    hadc.Init.DataAlign = ADC_DATAALIGN_RIGHT;
    hadc.Init.NbrOfConversion = 14;
    hadc.Init.DMAContinuousRequests = DISABLE;
    hadc.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
    if (HAL_ADC_Init(&hadc) != HAL_OK)
    {
        error_handler();
    }
    /**Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time. */
    sConfig.Channel = ADC_CHANNEL_10;
    sConfig.Rank = ADC_REGULAR_RANK_1;
    sConfig.SamplingTime = ADC_SAMPLETIME_15CYCLES;
    if (HAL_ADC_ConfigChannel(&hadc, &sConfig) != HAL_OK)
    {
        error_handler();
    }
    /**Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time. */
    sConfig.Channel = ADC_CHANNEL_11;
    sConfig.Rank = ADC_REGULAR_RANK_2;
    if (HAL_ADC_ConfigChannel(&hadc, &sConfig) != HAL_OK)
    {
        error_handler();
    }
    /**Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time. */
    sConfig.Channel = ADC_CHANNEL_12;
    sConfig.Rank = ADC_REGULAR_RANK_3;
    if (HAL_ADC_ConfigChannel(&hadc, &sConfig) != HAL_OK)
    {
        error_handler();
    }
    /**Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.*/
    sConfig.Channel = ADC_CHANNEL_13;
    sConfig.Rank = ADC_REGULAR_RANK_4;
    if (HAL_ADC_ConfigChannel(&hadc, &sConfig) != HAL_OK)
    {
        error_handler();
    }
    /**Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time. */
    sConfig.Channel = ADC_CHANNEL_0;
    sConfig.Rank = ADC_REGULAR_RANK_5;
    if (HAL_ADC_ConfigChannel(&hadc, &sConfig) != HAL_OK)
    {
        error_handler();
    }
    /**Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time. */
    sConfig.Channel = ADC_CHANNEL_1;
    sConfig.Rank = ADC_REGULAR_RANK_6;
    if (HAL_ADC_ConfigChannel(&hadc, &sConfig) != HAL_OK)
    {
        error_handler();
    }
    /**Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time. */
    sConfig.Channel = ADC_CHANNEL_2;
    sConfig.Rank = ADC_REGULAR_RANK_7;
    if (HAL_ADC_ConfigChannel(&hadc, &sConfig) != HAL_OK)
    {
        error_handler();
    }
    /**Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time. */
    sConfig.Channel = ADC_CHANNEL_3;
    sConfig.Rank = ADC_REGULAR_RANK_8;
    if (HAL_ADC_ConfigChannel(&hadc, &sConfig) != HAL_OK)
    {
        error_handler();
    }
    /**Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.  */
    sConfig.Channel = ADC_CHANNEL_5;
    sConfig.Rank = ADC_REGULAR_RANK_9;
    if (HAL_ADC_ConfigChannel(&hadc, &sConfig) != HAL_OK)
    {
        error_handler();
    }
    /**Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.  */
    sConfig.Channel = ADC_CHANNEL_6;
    sConfig.Rank = ADC_REGULAR_RANK_10;
    if (HAL_ADC_ConfigChannel(&hadc, &sConfig) != HAL_OK)
    {
        error_handler();
    }
    /**Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time. */
    sConfig.Channel = ADC_CHANNEL_14;
    sConfig.Rank = ADC_REGULAR_RANK_11;
    if (HAL_ADC_ConfigChannel(&hadc, &sConfig) != HAL_OK)
    {
        error_handler();
    }
    /**Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.  */
    sConfig.Channel = ADC_CHANNEL_15;
    sConfig.Rank = ADC_REGULAR_RANK_12;
    if (HAL_ADC_ConfigChannel(&hadc, &sConfig) != HAL_OK)
    {
        error_handler();
    }
    /**Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time. */
    sConfig.Channel = ADC_CHANNEL_8;
    sConfig.Rank = ADC_REGULAR_RANK_13;
    if (HAL_ADC_ConfigChannel(&hadc, &sConfig) != HAL_OK)
    {
        error_handler();
    }
    /**Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time. */
    sConfig.Channel = ADC_CHANNEL_9;
    sConfig.Rank = ADC_REGULAR_RANK_14;
    if (HAL_ADC_ConfigChannel(&hadc, &sConfig) != HAL_OK)
    {
        error_handler();
    }
    return PFC_SUCCESS;
}

/**
  * @brief This function handles DMA global interrupt
  */
void ADC_DMA_IRQ(void)
{
    HAL_DMA_IRQHandler(&hdma_adc);
}
/** @} */