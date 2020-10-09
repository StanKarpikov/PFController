/**
 * @file timer.c
 * @author Stanislav Karpikov
 * @brief Board support package: Timer
 */

/** @addtogroup hdw_bsp_timer
 * @{
 */
 
/*--------------------------------------------------------------
                       INCLUDES
--------------------------------------------------------------*/

#include "BSP/timer.h"

#include "BSP/bsp.h"
#include "BSP/debug.h"
#include "defines.h"
#include "stm32f7xx_hal.h"

/*--------------------------------------------------------------
                       DEFINES
--------------------------------------------------------------*/

#define TIMER_SYNC_PERIOD      (15625UL) /**< Syncronisation timer period value */
#define TIMER_COOLER_PRESCALER (20000UL) /**< Timer for cooler prescaler value */

#define PWM_DEAD_TIME (100U) /**< Dead time for the PWM signal [ticks] */

/*--------------------------------------------------------------
                       PRIVATE DATA
--------------------------------------------------------------*/

static TIM_HandleTypeDef timer_pwm = {0};         /**< Timer PWM hardware handle */
static TIM_HandleTypeDef timer_adc = {0};         /**< Timer ADC hardware handle */
static TIM_HandleTypeDef htim_efmc = {0};         /**< Timer EFMC hardware handle */
static TIM_HandleTypeDef timer_cooler = {0};      /**< Timer 9 hardware handle */
static DMA_HandleTypeDef hdma_tim_pwm_ch1 = {0};  /**< Timer PWM channel 1 DMA handle */
static DMA_HandleTypeDef hdma_tim_pwm_ch2 = {0};  /**< Timer PWM channel 2 DMA handle */
static DMA_HandleTypeDef hdma_tim_pwm_ch3 = {0};  /**< Timer PWM channel 3 DMA handle */
static DMA_HandleTypeDef hdma_tim_efmc_ch2 = {0}; /**< Timer EFMC channel 1 DMA handle */
static DMA_HandleTypeDef hdma_tim_efmc_ch3 = {0}; /**< Timer EFMC channel 2 DMA handle */

/*--------------------------------------------------------------
                       PRIVATE FUNCTIONS
--------------------------------------------------------------*/

/**
 * @brief Init the hardware for a timer
 *
 * @param htim The hardware handle
 */
static void tim_msp_post_init(TIM_HandleTypeDef* htim)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    if (htim->Instance == TIMER_PWM)
    {
        __HAL_RCC_GPIOE_CLK_ENABLE();

        GPIO_InitStruct.Pin = TIMER_PWM_LOW_CH1N_Pin | TIMER_PWM_HIGH_CH1_Pin | TIMER_PWM_LOW_CH2N_Pin | TIMER_PWM_HIGH_CH2_Pin | TIMER_PWM_LOW_CH3N_Pin | TIMER_PWM_HIGH_CH3_Pin;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
        GPIO_InitStruct.Alternate = GPIO_AF1_TIM1;
        HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);
    }
    else if (htim->Instance == TIMER_EFMC)
    {
        __HAL_RCC_GPIOB_CLK_ENABLE();
        __HAL_RCC_GPIOC_CLK_ENABLE();

        GPIO_InitStruct.Pin = TIMER_EFMC_LOW_CH1N_Pin | TIMER_EFMC_LOW_CH2N_Pin;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
        GPIO_InitStruct.Alternate = GPIO_AF3_TIM8;
        HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

        GPIO_InitStruct.Pin = TIMER_EFMC_HIGH_CH1_Pin | TIMER_EFMC_HIGH_CH2_Pin;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
        GPIO_InitStruct.Alternate = GPIO_AF3_TIM8;
        HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
    }
    else if (htim->Instance == TIMER_COOLER)
    {
        __HAL_RCC_GPIOE_CLK_ENABLE();

        GPIO_InitStruct.Pin = PWM_COOLER_Pin;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
        GPIO_InitStruct.Alternate = GPIO_AF3_TIM9;
        HAL_GPIO_Init(PWM_COOLER_GPIO_Port, &GPIO_InitStruct);
    }
}

/**
 * @brief Timer 1 Initialization Function
 *
 * @return The status of the operation
 */
static status_t timer_pwm_init(void)
{
    TIM_ClockConfigTypeDef sClockSourceConfig = {0};
    TIM_MasterConfigTypeDef sMasterConfig = {0};
    TIM_OC_InitTypeDef sConfigOC = {0};
    TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig = {0};

    timer_pwm.Instance = TIMER_PWM;
    timer_pwm.Init.Prescaler = 4;
    timer_pwm.Init.CounterMode = TIM_COUNTERMODE_UP;
    timer_pwm.Init.Period = PWM_PERIOD;
    timer_pwm.Init.ClockDivision = TIM_CLOCKDIVISION_DIV4;
    timer_pwm.Init.RepetitionCounter = 0;
    timer_pwm.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    if (HAL_TIM_Base_Init(&timer_pwm) != HAL_OK)
    {
        error_handler();
    }
    sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
    if (HAL_TIM_ConfigClockSource(&timer_pwm, &sClockSourceConfig) != HAL_OK)
    {
        error_handler();
    }
    if (HAL_TIM_PWM_Init(&timer_pwm) != HAL_OK)
    {
        error_handler();
    }
    sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
    sMasterConfig.MasterOutputTrigger2 = TIM_TRGO2_RESET;
    sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
    if (HAL_TIMEx_MasterConfigSynchronization(&timer_pwm, &sMasterConfig) != HAL_OK)
    {
        error_handler();
    }
    sConfigOC.OCMode = TIM_OCMODE_PWM1;
    sConfigOC.Pulse = 0;
    sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
    sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
    sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
    sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
    sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
    if (HAL_TIM_PWM_ConfigChannel(&timer_pwm, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
    {
        error_handler();
    }
    if (HAL_TIM_PWM_ConfigChannel(&timer_pwm, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
    {
        error_handler();
    }
    if (HAL_TIM_PWM_ConfigChannel(&timer_pwm, &sConfigOC, TIM_CHANNEL_3) != HAL_OK)
    {
        error_handler();
    }
    sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_ENABLE;
    sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_ENABLE;
    sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
    sBreakDeadTimeConfig.DeadTime = PWM_DEAD_TIME;
    sBreakDeadTimeConfig.BreakState = TIM_BREAK_DISABLE;
    sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
    sBreakDeadTimeConfig.BreakFilter = 3;
    sBreakDeadTimeConfig.Break2State = TIM_BREAK2_DISABLE;
    sBreakDeadTimeConfig.Break2Polarity = TIM_BREAK2POLARITY_HIGH;
    sBreakDeadTimeConfig.Break2Filter = 0;
    sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE;
    if (HAL_TIMEx_ConfigBreakDeadTime(&timer_pwm, &sBreakDeadTimeConfig) != HAL_OK)
    {
        error_handler();
    }
    //~400ns
    tim_msp_post_init(&timer_pwm);
    return PFC_SUCCESS;
}

/**
 * @brief Timer for syncronisation Initialization Function
 *
 * @return The status of the operation
 */
static status_t timer_sync_init(void)
{
    TIM_ClockConfigTypeDef sClockSourceConfig = {0};
    TIM_MasterConfigTypeDef sMasterConfig = {0};

    timer_adc.Instance = TIMER_SYNC;
    timer_adc.Init.Prescaler = 0;
    timer_adc.Init.CounterMode = TIM_COUNTERMODE_UP;
    timer_adc.Init.Period = TIMER_SYNC_PERIOD;
    timer_adc.Init.ClockDivision = TIM_CLOCKDIVISION_DIV2;
    timer_adc.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    if (HAL_TIM_Base_Init(&timer_adc) != HAL_OK)
    {
        error_handler();
    }
    sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
    if (HAL_TIM_ConfigClockSource(&timer_adc, &sClockSourceConfig) != HAL_OK)
    {
        error_handler();
    }
    sMasterConfig.MasterOutputTrigger = TIM_TRGO_UPDATE;
    sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_ENABLE;
    if (HAL_TIMEx_MasterConfigSynchronization(&timer_adc, &sMasterConfig) != HAL_OK)
    {
        error_handler();
    }
    return PFC_SUCCESS;
}

/**
 * @brief Timer EFMC Initialization Function
 *
 * @return The status of the operation
 */
static status_t tim_efmc_Init(void)
{
    TIM_ClockConfigTypeDef sClockSourceConfig = {0};
    TIM_MasterConfigTypeDef sMasterConfig = {0};
    TIM_OC_InitTypeDef sConfigOC = {0};
    TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig = {0};

    htim_efmc.Instance = TIMER_EFMC;
    htim_efmc.Init.Prescaler = 0;
    htim_efmc.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim_efmc.Init.Period = PWM_PERIOD;
    htim_efmc.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim_efmc.Init.RepetitionCounter = 0;
    htim_efmc.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    if (HAL_TIM_Base_Init(&htim_efmc) != HAL_OK)
    {
        error_handler();
    }
    sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
    if (HAL_TIM_ConfigClockSource(&htim_efmc, &sClockSourceConfig) != HAL_OK)
    {
        error_handler();
    }
    if (HAL_TIM_PWM_Init(&htim_efmc) != HAL_OK)
    {
        error_handler();
    }
    sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
    sMasterConfig.MasterOutputTrigger2 = TIM_TRGO2_RESET;
    sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
    if (HAL_TIMEx_MasterConfigSynchronization(&htim_efmc, &sMasterConfig) != HAL_OK)
    {
        error_handler();
    }
    sConfigOC.OCMode = TIM_OCMODE_PWM1;
    sConfigOC.Pulse = 0;
    sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
    sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
    sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
    sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
    sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
    if (HAL_TIM_PWM_ConfigChannel(&htim_efmc, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
    {
        error_handler();
    }
    if (HAL_TIM_PWM_ConfigChannel(&htim_efmc, &sConfigOC, TIM_CHANNEL_3) != HAL_OK)
    {
        error_handler();
    }
    sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_ENABLE;
    sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_ENABLE;
    sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
    sBreakDeadTimeConfig.DeadTime = PWM_DEAD_TIME;
    sBreakDeadTimeConfig.BreakState = TIM_BREAK_DISABLE;
    sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
    sBreakDeadTimeConfig.BreakFilter = 0;
    sBreakDeadTimeConfig.Break2State = TIM_BREAK2_ENABLE;
    sBreakDeadTimeConfig.Break2Polarity = TIM_BREAK2POLARITY_HIGH;
    sBreakDeadTimeConfig.Break2Filter = 3;
    sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE;
    if (HAL_TIMEx_ConfigBreakDeadTime(&htim_efmc, &sBreakDeadTimeConfig) != HAL_OK)
    {
        error_handler();
    }
    tim_msp_post_init(&htim_efmc);
    return PFC_SUCCESS;
}

/**
 * @brief Timer 9 Initialization Function
 *
 * @return The status of the operation
 */
static status_t timer_cooler_init(void)
{
    TIM_OC_InitTypeDef sConfigOC = {0};

    timer_cooler.Instance = TIMER_COOLER;
    timer_cooler.Init.Prescaler = TIMER_COOLER_PRESCALER;
    timer_cooler.Init.CounterMode = TIM_COUNTERMODE_UP;
    timer_cooler.Init.Period = 0;
    timer_cooler.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    timer_cooler.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    if (HAL_TIM_PWM_Init(&timer_cooler) != HAL_OK)
    {
        error_handler();
    }
    sConfigOC.OCMode = TIM_OCMODE_PWM1;
    sConfigOC.Pulse = 0;
    sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
    sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
    if (HAL_TIM_PWM_ConfigChannel(&timer_cooler, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
    {
        error_handler();
    }
    tim_msp_post_init(&timer_cooler);
    return PFC_SUCCESS;
}

/*--------------------------------------------------------------
                       PUBLIC FUNCTIONS
--------------------------------------------------------------*/

/**
* @brief TIM_Base MSP Initialization
* This function configures the hardware resources
*
* @param htim_base TIM_Base handle pointer
*/
void HAL_TIM_Base_MspInit(TIM_HandleTypeDef* htim_base)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    if (htim_base->Instance == TIMER_PWM)
    {
        /* Peripheral clock enable */
        TIMER_PWM_CLK_ENABLE();

        __HAL_RCC_GPIOE_CLK_ENABLE();

        GPIO_InitStruct.Pin = GPIO_PIN_15;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
        GPIO_InitStruct.Alternate = GPIO_AF1_TIM1;
        HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

        /* TIMER_PWM DMA Init */
        /* TIM1_CH1 Init */
        hdma_tim_pwm_ch1.Instance = TIMER_PWM_CH1_DMA_STREAM;
        hdma_tim_pwm_ch1.Init.Channel = TIMER_PWM_CH1_DMA_CHANNEL;
        hdma_tim_pwm_ch1.Init.Direction = DMA_MEMORY_TO_PERIPH;
        hdma_tim_pwm_ch1.Init.PeriphInc = DMA_PINC_DISABLE;
        hdma_tim_pwm_ch1.Init.MemInc = DMA_MINC_ENABLE;
        hdma_tim_pwm_ch1.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
        hdma_tim_pwm_ch1.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
        hdma_tim_pwm_ch1.Init.Mode = DMA_NORMAL;
        hdma_tim_pwm_ch1.Init.Priority = DMA_PRIORITY_LOW;
        hdma_tim_pwm_ch1.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
        if (HAL_DMA_Init(&hdma_tim_pwm_ch1) != HAL_OK)
        {
            error_handler();
        }

        __HAL_LINKDMA(htim_base, hdma[TIM_DMA_ID_CC1], hdma_tim_pwm_ch1);

        /* TIM1_CH2 Init */
        hdma_tim_pwm_ch2.Instance = TIMER_PWM_CH2_DMA_STREAM;
        hdma_tim_pwm_ch2.Init.Channel = TIMER_PWM_CH2_DMA_CHANNEL;
        hdma_tim_pwm_ch2.Init.Direction = DMA_MEMORY_TO_PERIPH;
        hdma_tim_pwm_ch2.Init.PeriphInc = DMA_PINC_DISABLE;
        hdma_tim_pwm_ch2.Init.MemInc = DMA_MINC_ENABLE;
        hdma_tim_pwm_ch2.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
        hdma_tim_pwm_ch2.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
        hdma_tim_pwm_ch2.Init.Mode = DMA_NORMAL;
        hdma_tim_pwm_ch2.Init.Priority = DMA_PRIORITY_LOW;
        hdma_tim_pwm_ch2.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
        if (HAL_DMA_Init(&hdma_tim_pwm_ch2) != HAL_OK)
        {
            error_handler();
        }

        __HAL_LINKDMA(htim_base, hdma[TIM_DMA_ID_CC2], hdma_tim_pwm_ch2);

        /* TIM1_CH3 Init */
        hdma_tim_pwm_ch3.Instance = TIMER_PWM_CH3_DMA_STREAM;
        hdma_tim_pwm_ch3.Init.Channel = TIMER_PWM_CH3_DMA_CHANNEL;
        hdma_tim_pwm_ch3.Init.Direction = DMA_MEMORY_TO_PERIPH;
        hdma_tim_pwm_ch3.Init.PeriphInc = DMA_PINC_DISABLE;
        hdma_tim_pwm_ch3.Init.MemInc = DMA_MINC_ENABLE;
        hdma_tim_pwm_ch3.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
        hdma_tim_pwm_ch3.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
        hdma_tim_pwm_ch3.Init.Mode = DMA_NORMAL;
        hdma_tim_pwm_ch3.Init.Priority = DMA_PRIORITY_LOW;
        hdma_tim_pwm_ch3.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
        if (HAL_DMA_Init(&hdma_tim_pwm_ch3) != HAL_OK)
        {
            error_handler();
        }

        __HAL_LINKDMA(htim_base, hdma[TIM_DMA_ID_CC3], hdma_tim_pwm_ch3);
    }
    else if (htim_base->Instance == TIMER_SYNC)
    {
        TIMER_SYNC_CLK_ENABLE();
    }
    else if (htim_base->Instance == TIMER_EFMC)
    {
        TIMER_EFMC_CLK_ENABLE();

        __HAL_RCC_GPIOA_CLK_ENABLE();

        GPIO_InitStruct.Pin = GPIO_PIN_8;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
        GPIO_InitStruct.Alternate = GPIO_AF3_TIM8;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

        hdma_tim_efmc_ch2.Instance = TIMER_EFMC_CH1_DMA_STREAM;
        hdma_tim_efmc_ch2.Init.Channel = TIMER_EFMC_CH1_DMA_CHANNEL;
        hdma_tim_efmc_ch2.Init.Direction = DMA_MEMORY_TO_PERIPH;
        hdma_tim_efmc_ch2.Init.PeriphInc = DMA_PINC_DISABLE;
        hdma_tim_efmc_ch2.Init.MemInc = DMA_MINC_ENABLE;
        hdma_tim_efmc_ch2.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
        hdma_tim_efmc_ch2.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
        hdma_tim_efmc_ch2.Init.Mode = DMA_NORMAL;
        hdma_tim_efmc_ch2.Init.Priority = DMA_PRIORITY_LOW;
        hdma_tim_efmc_ch2.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
        if (HAL_DMA_Init(&hdma_tim_efmc_ch2) != HAL_OK)
        {
            error_handler();
        }

        __HAL_LINKDMA(htim_base, hdma[TIM_DMA_ID_CC2], hdma_tim_efmc_ch2);

        hdma_tim_efmc_ch3.Instance = TIMER_EFMC_CH2_DMA_STREAM;
        hdma_tim_efmc_ch3.Init.Channel = TIMER_EFMC_CH2_DMA_CHANNEL;
        hdma_tim_efmc_ch3.Init.Direction = DMA_MEMORY_TO_PERIPH;
        hdma_tim_efmc_ch3.Init.PeriphInc = DMA_PINC_DISABLE;
        hdma_tim_efmc_ch3.Init.MemInc = DMA_MINC_ENABLE;
        hdma_tim_efmc_ch3.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
        hdma_tim_efmc_ch3.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
        hdma_tim_efmc_ch3.Init.Mode = DMA_NORMAL;
        hdma_tim_efmc_ch3.Init.Priority = DMA_PRIORITY_LOW;
        hdma_tim_efmc_ch3.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
        if (HAL_DMA_Init(&hdma_tim_efmc_ch3) != HAL_OK)
        {
            error_handler();
        }

        __HAL_LINKDMA(htim_base, hdma[TIM_DMA_ID_CC3], hdma_tim_efmc_ch3);
    }
}

/**
* @brief TIM PWM MSP Initialization
* This function configures the hardware resources
*
* @param htim TIM handle pointer
*/
void HAL_TIM_PWM_MspInit(TIM_HandleTypeDef* htim)
{
    if (htim->Instance == TIMER_COOLER)
    {
        TIMER_COOLER_CLK_ENABLE();
    }
}

/**
* @brief TIM_Base MSP De-Initialization
* This function freeze the hardware resources
*
* @param htim_base TIM_Base handle pointer
*/
void HAL_TIM_Base_MspDeInit(TIM_HandleTypeDef* htim_base)
{
    if (htim_base->Instance == TIMER_PWM)
    {
        TIMER_PWM_CLK_DISABLE();

        HAL_GPIO_DeInit(GPIOE, TIMER_PWM_LOW_CH1N_Pin | TIMER_PWM_HIGH_CH1_Pin | TIMER_PWM_LOW_CH2N_Pin | TIMER_PWM_HIGH_CH2_Pin | TIMER_PWM_LOW_CH3N_Pin | TIMER_PWM_HIGH_CH3_Pin | GPIO_PIN_15);

        HAL_DMA_DeInit(htim_base->hdma[TIM_DMA_ID_CC1]);
        HAL_DMA_DeInit(htim_base->hdma[TIM_DMA_ID_CC2]);
        HAL_DMA_DeInit(htim_base->hdma[TIM_DMA_ID_CC3]);
    }
    else if (htim_base->Instance == TIMER_SYNC)
    {
        TIMER_SYNC_CLK_DISABLE();
    }
    else if (htim_base->Instance == TIMER_EFMC)
    {
        TIMER_EFMC_CLK_DISABLE();

        HAL_GPIO_DeInit(GPIOB, TIMER_EFMC_LOW_CH1N_Pin | TIMER_EFMC_LOW_CH2N_Pin);
        HAL_GPIO_DeInit(GPIOC, TIMER_EFMC_HIGH_CH1_Pin | TIMER_EFMC_HIGH_CH2_Pin);
        HAL_GPIO_DeInit(GPIOA, GPIO_PIN_8);

        HAL_DMA_DeInit(htim_base->hdma[TIM_DMA_ID_CC2]);
        HAL_DMA_DeInit(htim_base->hdma[TIM_DMA_ID_CC3]);
    }
}

/**
* @brief TIM PWM MSP De-Initialization
* This function freeze the hardware resources
*
* @param htim TIM handle pointer
*/
void HAL_TIM_PWM_MspDeInit(TIM_HandleTypeDef* htim)
{
    if (htim->Instance == TIMER_COOLER)
    {
        TIMER_COOLER_CLK_DISABLE();
    }
}

/*
 * @brief Set PWM values
 *
 * @param ccr1 Timer CCR1 register contents
 * @param ccr2 Timer CCR2 register contents
 * @param ccr3 Timer CCR3 register contents
 *
 * @return The status of the operation
 */
status_t timer_write_pwm(uint32_t ccr1, uint32_t ccr2, uint32_t ccr3)
{
    TIMER_PWM->CCR1 = ccr1;
    TIMER_PWM->CCR2 = ccr2;
    TIMER_PWM->CCR3 = ccr3;
    return PFC_SUCCESS;
}

/*
 * @brief Change syncronisation timer period
 *
 * @param arr The new value of the ARR register
 *
 * @return The status of the operation
 */
status_t timer_correct_period(uint32_t arr)
{
    ENTER_CRITICAL();
    TIMER_SYNC->ARR = arr;
    TIMER_SYNC->EGR = TIM_EGR_UG;
    EXIT_CRITICAL();
    return PFC_SUCCESS;
}

/*
 * @brief Enable PWM (after disabling)
 *
 * @return The status of the operation
 */
status_t timer_restore_pwm(void)
{
    TIMER_PWM->CCER |= (TIM_CCER_CC1E);
    TIMER_PWM->CCER |= (TIM_CCER_CC1NE);
    TIMER_PWM->CCER |= (TIM_CCER_CC2E);
    TIMER_PWM->CCER |= (TIM_CCER_CC2NE);
    TIMER_PWM->CCER |= (TIM_CCER_CC3E);
    TIMER_PWM->CCER |= (TIM_CCER_CC3NE);

    __HAL_TIM_MOE_ENABLE(&timer_pwm);
    __HAL_TIM_ENABLE(&timer_pwm);

    return PFC_SUCCESS;
}

/*
 * @brief Disable PWM
 *
 * @return The status of the operation
 */
status_t timer_disable_pwm(void)
{
    __HAL_TIM_MOE_DISABLE(&timer_pwm);
    __HAL_TIM_DISABLE(&timer_pwm);

    TIMER_PWM->CCER &= ~(TIM_CCER_CC1E);
    TIMER_PWM->CCER &= ~(TIM_CCER_CC1NE);
    TIMER_PWM->CCER &= ~(TIM_CCER_CC2E);
    TIMER_PWM->CCER &= ~(TIM_CCER_CC2NE);
    TIMER_PWM->CCER &= ~(TIM_CCER_CC3E);
    TIMER_PWM->CCER &= ~(TIM_CCER_CC3NE);
    return PFC_SUCCESS;
}

/*
 * @brief Start timer for the ADC module
 *
 * @return The status of the operation
 */
status_t timer_start_adc_timer(void)
{
    HAL_TIM_Base_Start(&timer_adc);
    return PFC_SUCCESS;
}

/*
 * @brief Init the timer module
 *
 * @return The status of the operation
 */
status_t timer_init(void)
{
    tim_efmc_Init();
    timer_pwm_init();
    timer_cooler_init();
    timer_sync_init();
    return PFC_SUCCESS;
}

/**
  * @brief This function handles PWM timer channel 1 global interrupt.
  */
void TIMER_PWM_CH1_DMA_IRQ(void)
{
    HAL_DMA_IRQHandler(&hdma_tim_pwm_ch1);
}

/**
  * @brief This function handles PWM timer channel 2 global interrupt.
  */
void TIMER_PWM_CH2_DMA_IRQ(void)
{
    HAL_DMA_IRQHandler(&hdma_tim_pwm_ch2);
}

/**
  * @brief This function handles PWM timer channel 3 global interrupt.
  */
void TIMER_PWM_CH3_DMA_IRQ(void)
{
    HAL_DMA_IRQHandler(&hdma_tim_pwm_ch3);
}

/**
  * @brief This function handles timer EFMC DMA channel 1 global interrupt.
  */
void TIMER_EFMC_CH1_DMA_IRQ(void)
{
    HAL_DMA_IRQHandler(&hdma_tim_efmc_ch2);
}

/**
  * @brief This function handles timer EFMC DMA channel 2  interrupt.
  */
void TIMER_EFMC_CH2_DMA_IRQ(void)
{
    HAL_DMA_IRQHandler(&hdma_tim_efmc_ch3);
}
/** @} */