/**
 * @file timer.c
 * @author Stanislav Karpikov
 * @brief Board support package: Timer
 */

/*---------------------  INCLUDES  -------------------------------------------*/

#include "BSP/timer.h"

#include "BSP/bsp.h"
#include "BSP/debug.h"
#include "defines.h"
#include "stm32f7xx_hal.h"

TIM_HandleTypeDef htim1;
TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim8;
TIM_HandleTypeDef htim9;
DMA_HandleTypeDef hdma_tim1_ch1;
DMA_HandleTypeDef hdma_tim1_ch2;
DMA_HandleTypeDef hdma_tim1_ch3;
DMA_HandleTypeDef hdma_tim8_ch2;
DMA_HandleTypeDef hdma_tim8_ch3;

void HAL_TIM_MspPostInit(TIM_HandleTypeDef* htim);

status_t timer_start_adc_timer(void)
{
    HAL_TIM_Base_Start(&htim2);
    return PFC_SUCCESS;
}

/**
* @brief TIM_Base MSP Initialization
* This function configures the hardware resources used in this example
* @param htim_base: TIM_Base handle pointer
* @retval None
*/
void HAL_TIM_Base_MspInit(TIM_HandleTypeDef* htim_base)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    if (htim_base->Instance == TIM1)
    {
        /* USER CODE BEGIN TIM1_MspInit 0 */

        /* USER CODE END TIM1_MspInit 0 */
        /* Peripheral clock enable */
        __HAL_RCC_TIM1_CLK_ENABLE();

        __HAL_RCC_GPIOE_CLK_ENABLE();
        /**TIM1 GPIO Configuration    
    PE15     ------> TIM1_BKIN 
    */
        GPIO_InitStruct.Pin = GPIO_PIN_15;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
        GPIO_InitStruct.Alternate = GPIO_AF1_TIM1;
        HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

        /* TIM1 DMA Init */
        /* TIM1_CH1 Init */
        hdma_tim1_ch1.Instance = DMA2_Stream1;
        hdma_tim1_ch1.Init.Channel = DMA_CHANNEL_6;
        hdma_tim1_ch1.Init.Direction = DMA_MEMORY_TO_PERIPH;
        hdma_tim1_ch1.Init.PeriphInc = DMA_PINC_DISABLE;
        hdma_tim1_ch1.Init.MemInc = DMA_MINC_ENABLE;
        hdma_tim1_ch1.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
        hdma_tim1_ch1.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
        hdma_tim1_ch1.Init.Mode = DMA_NORMAL;
        hdma_tim1_ch1.Init.Priority = DMA_PRIORITY_LOW;
        hdma_tim1_ch1.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
        if (HAL_DMA_Init(&hdma_tim1_ch1) != HAL_OK)
        {
            Error_Handler();
        }

        __HAL_LINKDMA(htim_base, hdma[TIM_DMA_ID_CC1], hdma_tim1_ch1);

        /* TIM1_CH2 Init */
        hdma_tim1_ch2.Instance = DMA2_Stream2;
        hdma_tim1_ch2.Init.Channel = DMA_CHANNEL_6;
        hdma_tim1_ch2.Init.Direction = DMA_MEMORY_TO_PERIPH;
        hdma_tim1_ch2.Init.PeriphInc = DMA_PINC_DISABLE;
        hdma_tim1_ch2.Init.MemInc = DMA_MINC_ENABLE;
        hdma_tim1_ch2.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
        hdma_tim1_ch2.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
        hdma_tim1_ch2.Init.Mode = DMA_NORMAL;
        hdma_tim1_ch2.Init.Priority = DMA_PRIORITY_LOW;
        hdma_tim1_ch2.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
        if (HAL_DMA_Init(&hdma_tim1_ch2) != HAL_OK)
        {
            Error_Handler();
        }

        __HAL_LINKDMA(htim_base, hdma[TIM_DMA_ID_CC2], hdma_tim1_ch2);

        /* TIM1_CH3 Init */
        hdma_tim1_ch3.Instance = DMA2_Stream6;
        hdma_tim1_ch3.Init.Channel = DMA_CHANNEL_6;
        hdma_tim1_ch3.Init.Direction = DMA_MEMORY_TO_PERIPH;
        hdma_tim1_ch3.Init.PeriphInc = DMA_PINC_DISABLE;
        hdma_tim1_ch3.Init.MemInc = DMA_MINC_ENABLE;
        hdma_tim1_ch3.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
        hdma_tim1_ch3.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
        hdma_tim1_ch3.Init.Mode = DMA_NORMAL;
        hdma_tim1_ch3.Init.Priority = DMA_PRIORITY_LOW;
        hdma_tim1_ch3.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
        if (HAL_DMA_Init(&hdma_tim1_ch3) != HAL_OK)
        {
            Error_Handler();
        }

        __HAL_LINKDMA(htim_base, hdma[TIM_DMA_ID_CC3], hdma_tim1_ch3);

        /* USER CODE BEGIN TIM1_MspInit 1 */

        /* USER CODE END TIM1_MspInit 1 */
    }
    else if (htim_base->Instance == TIM2)
    {
        /* USER CODE BEGIN TIM2_MspInit 0 */

        /* USER CODE END TIM2_MspInit 0 */
        /* Peripheral clock enable */
        __HAL_RCC_TIM2_CLK_ENABLE();
        /* USER CODE BEGIN TIM2_MspInit 1 */

        /* USER CODE END TIM2_MspInit 1 */
    }
    else if (htim_base->Instance == TIM8)
    {
        /* USER CODE BEGIN TIM8_MspInit 0 */

        /* USER CODE END TIM8_MspInit 0 */
        /* Peripheral clock enable */
        __HAL_RCC_TIM8_CLK_ENABLE();

        __HAL_RCC_GPIOA_CLK_ENABLE();
        /**TIM8 GPIO Configuration    
    PA8     ------> TIM8_BKIN2 
    */
        GPIO_InitStruct.Pin = GPIO_PIN_8;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
        GPIO_InitStruct.Alternate = GPIO_AF3_TIM8;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

        /* TIM8 DMA Init */
        /* TIM8_CH2 Init */
        hdma_tim8_ch2.Instance = DMA2_Stream3;
        hdma_tim8_ch2.Init.Channel = DMA_CHANNEL_7;
        hdma_tim8_ch2.Init.Direction = DMA_MEMORY_TO_PERIPH;
        hdma_tim8_ch2.Init.PeriphInc = DMA_PINC_DISABLE;
        hdma_tim8_ch2.Init.MemInc = DMA_MINC_ENABLE;
        hdma_tim8_ch2.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
        hdma_tim8_ch2.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
        hdma_tim8_ch2.Init.Mode = DMA_NORMAL;
        hdma_tim8_ch2.Init.Priority = DMA_PRIORITY_LOW;
        hdma_tim8_ch2.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
        if (HAL_DMA_Init(&hdma_tim8_ch2) != HAL_OK)
        {
            Error_Handler();
        }

        __HAL_LINKDMA(htim_base, hdma[TIM_DMA_ID_CC2], hdma_tim8_ch2);

        /* TIM8_CH3 Init */
        hdma_tim8_ch3.Instance = DMA2_Stream4;
        hdma_tim8_ch3.Init.Channel = DMA_CHANNEL_7;
        hdma_tim8_ch3.Init.Direction = DMA_MEMORY_TO_PERIPH;
        hdma_tim8_ch3.Init.PeriphInc = DMA_PINC_DISABLE;
        hdma_tim8_ch3.Init.MemInc = DMA_MINC_ENABLE;
        hdma_tim8_ch3.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
        hdma_tim8_ch3.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
        hdma_tim8_ch3.Init.Mode = DMA_NORMAL;
        hdma_tim8_ch3.Init.Priority = DMA_PRIORITY_LOW;
        hdma_tim8_ch3.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
        if (HAL_DMA_Init(&hdma_tim8_ch3) != HAL_OK)
        {
            Error_Handler();
        }

        __HAL_LINKDMA(htim_base, hdma[TIM_DMA_ID_CC3], hdma_tim8_ch3);

        /* USER CODE BEGIN TIM8_MspInit 1 */

        /* USER CODE END TIM8_MspInit 1 */
    }
}

/**
* @brief TIM_PWM MSP Initialization
* This function configures the hardware resources used in this example
* @param htim_pwm: TIM_PWM handle pointer
* @retval None
*/
void HAL_TIM_PWM_MspInit(TIM_HandleTypeDef* htim_pwm)
{
    if (htim_pwm->Instance == TIM9)
    {
        /* USER CODE BEGIN TIM9_MspInit 0 */

        /* USER CODE END TIM9_MspInit 0 */
        /* Peripheral clock enable */
        __HAL_RCC_TIM9_CLK_ENABLE();
        /* USER CODE BEGIN TIM9_MspInit 1 */

        /* USER CODE END TIM9_MspInit 1 */
    }
}

void HAL_TIM_MspPostInit(TIM_HandleTypeDef* htim)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    if (htim->Instance == TIM1)
    {
        /* USER CODE BEGIN TIM1_MspPostInit 0 */

        /* USER CODE END TIM1_MspPostInit 0 */
        __HAL_RCC_GPIOE_CLK_ENABLE();
        /**TIM1 GPIO Configuration    
    PE8     ------> TIM1_CH1N
    PE9     ------> TIM1_CH1
    PE10     ------> TIM1_CH2N
    PE11     ------> TIM1_CH2
    PE12     ------> TIM1_CH3N
    PE13     ------> TIM1_CH3 
    */
        GPIO_InitStruct.Pin = CH1_KKM_LOW_TIM1CH1N_Pin | CH1_KKM_HIGH_TIM1CH1_Pin | CH2_KKM_LOW_TIM1CH2N_Pin | CH2_KKM_HIGH_TIM1CH2_Pin | CH3_KKM_LOW_TIM1CH3N_Pin | CH3_KKM_HIGH_TIM1CH3_Pin;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
        GPIO_InitStruct.Alternate = GPIO_AF1_TIM1;
        HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

        /* USER CODE BEGIN TIM1_MspPostInit 1 */

        /* USER CODE END TIM1_MspPostInit 1 */
    }
    else if (htim->Instance == TIM8)
    {
        /* USER CODE BEGIN TIM8_MspPostInit 0 */

        /* USER CODE END TIM8_MspPostInit 0 */

        __HAL_RCC_GPIOB_CLK_ENABLE();
        __HAL_RCC_GPIOC_CLK_ENABLE();
        /**TIM8 GPIO Configuration    
    PB14     ------> TIM8_CH2N
    PB15     ------> TIM8_CH3N
    PC7     ------> TIM8_CH2
    PC8     ------> TIM8_CH3 
    */
        GPIO_InitStruct.Pin = CH1_ET_LOW_TIM8_CH2N_Pin | CH2_ET_LOW_TIM8_CH3N_Pin;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
        GPIO_InitStruct.Alternate = GPIO_AF3_TIM8;
        HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

        GPIO_InitStruct.Pin = CH1_ET_HIGH_TIM8_CH2_Pin | CH2_ET_HIGH_TIM8_CH3_Pin;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
        GPIO_InitStruct.Alternate = GPIO_AF3_TIM8;
        HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

        /* USER CODE BEGIN TIM8_MspPostInit 1 */

        /* USER CODE END TIM8_MspPostInit 1 */
    }
    else if (htim->Instance == TIM9)
    {
        /* USER CODE BEGIN TIM9_MspPostInit 0 */

        /* USER CODE END TIM9_MspPostInit 0 */

        __HAL_RCC_GPIOE_CLK_ENABLE();
        /**TIM9 GPIO Configuration    
    PE5     ------> TIM9_CH1 
    */
        GPIO_InitStruct.Pin = PWM_KYLER_Pin;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
        GPIO_InitStruct.Alternate = GPIO_AF3_TIM9;
        HAL_GPIO_Init(PWM_KYLER_GPIO_Port, &GPIO_InitStruct);

        /* USER CODE BEGIN TIM9_MspPostInit 1 */

        /* USER CODE END TIM9_MspPostInit 1 */
    }
}
/**
* @brief TIM_Base MSP De-Initialization
* This function freeze the hardware resources used in this example
* @param htim_base: TIM_Base handle pointer
* @retval None
*/

void HAL_TIM_Base_MspDeInit(TIM_HandleTypeDef* htim_base)
{
    if (htim_base->Instance == TIM1)
    {
        /* USER CODE BEGIN TIM1_MspDeInit 0 */

        /* USER CODE END TIM1_MspDeInit 0 */
        /* Peripheral clock disable */
        __HAL_RCC_TIM1_CLK_DISABLE();

        /**TIM1 GPIO Configuration    
    PE8     ------> TIM1_CH1N
    PE9     ------> TIM1_CH1
    PE10     ------> TIM1_CH2N
    PE11     ------> TIM1_CH2
    PE12     ------> TIM1_CH3N
    PE13     ------> TIM1_CH3
    PE15     ------> TIM1_BKIN 
    */
        HAL_GPIO_DeInit(GPIOE, CH1_KKM_LOW_TIM1CH1N_Pin | CH1_KKM_HIGH_TIM1CH1_Pin | CH2_KKM_LOW_TIM1CH2N_Pin | CH2_KKM_HIGH_TIM1CH2_Pin | CH3_KKM_LOW_TIM1CH3N_Pin | CH3_KKM_HIGH_TIM1CH3_Pin | GPIO_PIN_15);

        /* TIM1 DMA DeInit */
        HAL_DMA_DeInit(htim_base->hdma[TIM_DMA_ID_CC1]);
        HAL_DMA_DeInit(htim_base->hdma[TIM_DMA_ID_CC2]);
        HAL_DMA_DeInit(htim_base->hdma[TIM_DMA_ID_CC3]);
        /* USER CODE BEGIN TIM1_MspDeInit 1 */

        /* USER CODE END TIM1_MspDeInit 1 */
    }
    else if (htim_base->Instance == TIM2)
    {
        /* USER CODE BEGIN TIM2_MspDeInit 0 */

        /* USER CODE END TIM2_MspDeInit 0 */
        /* Peripheral clock disable */
        __HAL_RCC_TIM2_CLK_DISABLE();
        /* USER CODE BEGIN TIM2_MspDeInit 1 */

        /* USER CODE END TIM2_MspDeInit 1 */
    }
    else if (htim_base->Instance == TIM8)
    {
        /* USER CODE BEGIN TIM8_MspDeInit 0 */

        /* USER CODE END TIM8_MspDeInit 0 */
        /* Peripheral clock disable */
        __HAL_RCC_TIM8_CLK_DISABLE();

        /**TIM8 GPIO Configuration    
    PB14     ------> TIM8_CH2N
    PB15     ------> TIM8_CH3N
    PC7     ------> TIM8_CH2
    PC8     ------> TIM8_CH3
    PA8     ------> TIM8_BKIN2 
    */
        HAL_GPIO_DeInit(GPIOB, CH1_ET_LOW_TIM8_CH2N_Pin | CH2_ET_LOW_TIM8_CH3N_Pin);

        HAL_GPIO_DeInit(GPIOC, CH1_ET_HIGH_TIM8_CH2_Pin | CH2_ET_HIGH_TIM8_CH3_Pin);

        HAL_GPIO_DeInit(GPIOA, GPIO_PIN_8);

        /* TIM8 DMA DeInit */
        HAL_DMA_DeInit(htim_base->hdma[TIM_DMA_ID_CC2]);
        HAL_DMA_DeInit(htim_base->hdma[TIM_DMA_ID_CC3]);
        /* USER CODE BEGIN TIM8_MspDeInit 1 */

        /* USER CODE END TIM8_MspDeInit 1 */
    }
}

/**
* @brief TIM_PWM MSP De-Initialization
* This function freeze the hardware resources used in this example
* @param htim_pwm: TIM_PWM handle pointer
* @retval None
*/

void HAL_TIM_PWM_MspDeInit(TIM_HandleTypeDef* htim_pwm)
{
    if (htim_pwm->Instance == TIM9)
    {
        /* USER CODE BEGIN TIM9_MspDeInit 0 */

        /* USER CODE END TIM9_MspDeInit 0 */
        /* Peripheral clock disable */
        __HAL_RCC_TIM9_CLK_DISABLE();
        /* USER CODE BEGIN TIM9_MspDeInit 1 */

        /* USER CODE END TIM9_MspDeInit 1 */
    }
}

/**
  * @brief TIM1 Initialization Function
  * @param None
  * @retval None
  */
void MX_TIM1_Init(void)
{
    /* USER CODE BEGIN TIM1_Init 0 */

    /* USER CODE END TIM1_Init 0 */

    TIM_ClockConfigTypeDef sClockSourceConfig = {0};
    TIM_MasterConfigTypeDef sMasterConfig = {0};
    TIM_OC_InitTypeDef sConfigOC = {0};
    TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig = {0};

    /* USER CODE BEGIN TIM1_Init 1 */

    /* USER CODE END TIM1_Init 1 */
    htim1.Instance = TIM1;
    htim1.Init.Prescaler = 4;
    htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim1.Init.Period = PWM_PERIOD;
    htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV4;
    htim1.Init.RepetitionCounter = 0;
    htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    if (HAL_TIM_Base_Init(&htim1) != HAL_OK)
    {
        Error_Handler();
    }
    sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
    if (HAL_TIM_ConfigClockSource(&htim1, &sClockSourceConfig) != HAL_OK)
    {
        Error_Handler();
    }
    if (HAL_TIM_PWM_Init(&htim1) != HAL_OK)
    {
        Error_Handler();
    }
    sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
    sMasterConfig.MasterOutputTrigger2 = TIM_TRGO2_RESET;
    sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
    if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
    {
        Error_Handler();
    }
    sConfigOC.OCMode = TIM_OCMODE_PWM1;
    sConfigOC.Pulse = 0;
    sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
    sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
    sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
    sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
    sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
    if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
    {
        Error_Handler();
    }
    if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
    {
        Error_Handler();
    }
    if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_3) != HAL_OK)
    {
        Error_Handler();
    }
    sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_ENABLE;
    sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_ENABLE;
    sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
    sBreakDeadTimeConfig.DeadTime = 100;
    sBreakDeadTimeConfig.BreakState = TIM_BREAK_DISABLE;
    sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
    sBreakDeadTimeConfig.BreakFilter = 3;
    sBreakDeadTimeConfig.Break2State = TIM_BREAK2_DISABLE;
    sBreakDeadTimeConfig.Break2Polarity = TIM_BREAK2POLARITY_HIGH;
    sBreakDeadTimeConfig.Break2Filter = 0;
    sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE;
    if (HAL_TIMEx_ConfigBreakDeadTime(&htim1, &sBreakDeadTimeConfig) != HAL_OK)
    {
        Error_Handler();
    }
    /* USER CODE BEGIN TIM1_Init 2 */
    //~400ns
    /* USER CODE END TIM1_Init 2 */
    HAL_TIM_MspPostInit(&htim1);
}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
void MX_TIM2_Init(void)
{
    /* USER CODE BEGIN TIM2_Init 0 */

    /* USER CODE END TIM2_Init 0 */

    TIM_ClockConfigTypeDef sClockSourceConfig = {0};
    TIM_MasterConfigTypeDef sMasterConfig = {0};

    /* USER CODE BEGIN TIM2_Init 1 */

    /* USER CODE END TIM2_Init 1 */
    htim2.Instance = TIM2;
    htim2.Init.Prescaler = 0;
    htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim2.Init.Period = 15625;
    htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV2;
    htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
    {
        Error_Handler();
    }
    sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
    if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
    {
        Error_Handler();
    }
    sMasterConfig.MasterOutputTrigger = TIM_TRGO_UPDATE;
    sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_ENABLE;
    if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
    {
        Error_Handler();
    }
    /* USER CODE BEGIN TIM2_Init 2 */

    /* USER CODE END TIM2_Init 2 */
}

/**
  * @brief TIM8 Initialization Function
  * @param None
  * @retval None
  */
void MX_TIM8_Init(void)
{
    /* USER CODE BEGIN TIM8_Init 0 */

    /* USER CODE END TIM8_Init 0 */

    TIM_ClockConfigTypeDef sClockSourceConfig = {0};
    TIM_MasterConfigTypeDef sMasterConfig = {0};
    TIM_OC_InitTypeDef sConfigOC = {0};
    TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig = {0};

    /* USER CODE BEGIN TIM8_Init 1 */

    /* USER CODE END TIM8_Init 1 */
    htim8.Instance = TIM8;
    htim8.Init.Prescaler = 0;
    htim8.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim8.Init.Period = PWM_PERIOD;
    htim8.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim8.Init.RepetitionCounter = 0;
    htim8.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    if (HAL_TIM_Base_Init(&htim8) != HAL_OK)
    {
        Error_Handler();
    }
    sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
    if (HAL_TIM_ConfigClockSource(&htim8, &sClockSourceConfig) != HAL_OK)
    {
        Error_Handler();
    }
    if (HAL_TIM_PWM_Init(&htim8) != HAL_OK)
    {
        Error_Handler();
    }
    sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
    sMasterConfig.MasterOutputTrigger2 = TIM_TRGO2_RESET;
    sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
    if (HAL_TIMEx_MasterConfigSynchronization(&htim8, &sMasterConfig) != HAL_OK)
    {
        Error_Handler();
    }
    sConfigOC.OCMode = TIM_OCMODE_PWM1;
    sConfigOC.Pulse = 0;
    sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
    sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
    sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
    sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
    sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
    if (HAL_TIM_PWM_ConfigChannel(&htim8, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
    {
        Error_Handler();
    }
    if (HAL_TIM_PWM_ConfigChannel(&htim8, &sConfigOC, TIM_CHANNEL_3) != HAL_OK)
    {
        Error_Handler();
    }
    sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_ENABLE;
    sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_ENABLE;
    sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
    sBreakDeadTimeConfig.DeadTime = 100;
    sBreakDeadTimeConfig.BreakState = TIM_BREAK_DISABLE;
    sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
    sBreakDeadTimeConfig.BreakFilter = 0;
    sBreakDeadTimeConfig.Break2State = TIM_BREAK2_ENABLE;
    sBreakDeadTimeConfig.Break2Polarity = TIM_BREAK2POLARITY_HIGH;
    sBreakDeadTimeConfig.Break2Filter = 3;
    sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE;
    if (HAL_TIMEx_ConfigBreakDeadTime(&htim8, &sBreakDeadTimeConfig) != HAL_OK)
    {
        Error_Handler();
    }
    /* USER CODE BEGIN TIM8_Init 2 */

    /* USER CODE END TIM8_Init 2 */
    HAL_TIM_MspPostInit(&htim8);
}

/**
  * @brief TIM9 Initialization Function
  * @param None
  * @retval None
  */
void MX_TIM9_Init(void)
{
    /* USER CODE BEGIN TIM9_Init 0 */

    /* USER CODE END TIM9_Init 0 */

    TIM_OC_InitTypeDef sConfigOC = {0};

    /* USER CODE BEGIN TIM9_Init 1 */

    /* USER CODE END TIM9_Init 1 */
    htim9.Instance = TIM9;
    htim9.Init.Prescaler = 20000;
    htim9.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim9.Init.Period = 0;
    htim9.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim9.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    if (HAL_TIM_PWM_Init(&htim9) != HAL_OK)
    {
        Error_Handler();
    }
    sConfigOC.OCMode = TIM_OCMODE_PWM1;
    sConfigOC.Pulse = 0;
    sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
    sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
    if (HAL_TIM_PWM_ConfigChannel(&htim9, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
    {
        Error_Handler();
    }
    /* USER CODE BEGIN TIM9_Init 2 */

    /* USER CODE END TIM9_Init 2 */
    HAL_TIM_MspPostInit(&htim9);
}
