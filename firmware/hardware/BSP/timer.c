/**
 * @file timer.c
 * @author Stanislav Karpikov
 * @brief Board support package: Timer
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

#define TIMER2_PERIOD (15625)
#define TIMER9_PRESCALER (20000)

#define PWM_DEAD_TIME (100)

/*--------------------------------------------------------------
                       PRIVATE DATA
--------------------------------------------------------------*/

static TIM_HandleTypeDef htim1 = {0};
static TIM_HandleTypeDef htim2 = {0};
static TIM_HandleTypeDef htim8 = {0};
static TIM_HandleTypeDef htim9 = {0};
static DMA_HandleTypeDef hdma_tim1_ch1 = {0};
static DMA_HandleTypeDef hdma_tim1_ch2 = {0};
static DMA_HandleTypeDef hdma_tim1_ch3 = {0};
static DMA_HandleTypeDef hdma_tim8_ch2 = {0};
static DMA_HandleTypeDef hdma_tim8_ch3 = {0};

/*--------------------------------------------------------------
                       PRIVATE FUNCTIONS
--------------------------------------------------------------*/

static void tim_msp_post_init(TIM_HandleTypeDef* htim)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    if (htim->Instance == TIM1)
    {
        __HAL_RCC_GPIOE_CLK_ENABLE();

        GPIO_InitStruct.Pin = CH1_PFC_LOW_TIM1CH1N_Pin | CH1_PFC_HIGH_TIM1CH1_Pin | CH2_PFC_LOW_TIM1CH2N_Pin | CH2_PFC_HIGH_TIM1CH2_Pin | CH3_PFC_LOW_TIM1CH3N_Pin | CH3_PFC_HIGH_TIM1CH3_Pin;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
        GPIO_InitStruct.Alternate = GPIO_AF1_TIM1;
        HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);
    }
    else if (htim->Instance == TIM8)
    {
        __HAL_RCC_GPIOB_CLK_ENABLE();
        __HAL_RCC_GPIOC_CLK_ENABLE();

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
    }
    else if (htim->Instance == TIM9)
    {
        __HAL_RCC_GPIOE_CLK_ENABLE();

        GPIO_InitStruct.Pin = PWM_KYLER_Pin;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
        GPIO_InitStruct.Alternate = GPIO_AF3_TIM9;
        HAL_GPIO_Init(PWM_KYLER_GPIO_Port, &GPIO_InitStruct);
    }
}

/**
  * @brief TIM1 Initialization Function
  */
static status_t tim1_Init(void)
{
    TIM_ClockConfigTypeDef sClockSourceConfig = {0};
    TIM_MasterConfigTypeDef sMasterConfig = {0};
    TIM_OC_InitTypeDef sConfigOC = {0};
    TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig = {0};

    htim1.Instance = TIM1;
    htim1.Init.Prescaler = 4;
    htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim1.Init.Period = PWM_PERIOD;
    htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV4;
    htim1.Init.RepetitionCounter = 0;
    htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    if (HAL_TIM_Base_Init(&htim1) != HAL_OK)
    {
        error_handler();
    }
    sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
    if (HAL_TIM_ConfigClockSource(&htim1, &sClockSourceConfig) != HAL_OK)
    {
        error_handler();
    }
    if (HAL_TIM_PWM_Init(&htim1) != HAL_OK)
    {
        error_handler();
    }
    sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
    sMasterConfig.MasterOutputTrigger2 = TIM_TRGO2_RESET;
    sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
    if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
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
    if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
    {
        error_handler();
    }
    if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
    {
        error_handler();
    }
    if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_3) != HAL_OK)
    {
        error_handler();
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
        error_handler();
    }
    //~400ns
    tim_msp_post_init(&htim1);
		return PFC_SUCCESS;
}

/**
  * @brief TIM2 Initialization Function
  */
static status_t tim2_Init(void)
{
    TIM_ClockConfigTypeDef sClockSourceConfig = {0};
    TIM_MasterConfigTypeDef sMasterConfig = {0};

    htim2.Instance = TIM2;
    htim2.Init.Prescaler = 0;
    htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim2.Init.Period = TIMER2_PERIOD;
    htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV2;
    htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
    {
        error_handler();
    }
    sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
    if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
    {
        error_handler();
    }
    sMasterConfig.MasterOutputTrigger = TIM_TRGO_UPDATE;
    sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_ENABLE;
    if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
    {
        error_handler();
    }
		return PFC_SUCCESS;
}

/**
  * @brief TIM8 Initialization Function
  */
static status_t tim8_Init(void)
{
    TIM_ClockConfigTypeDef sClockSourceConfig = {0};
    TIM_MasterConfigTypeDef sMasterConfig = {0};
    TIM_OC_InitTypeDef sConfigOC = {0};
    TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig = {0};

    htim8.Instance = TIM8;
    htim8.Init.Prescaler = 0;
    htim8.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim8.Init.Period = PWM_PERIOD;
    htim8.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim8.Init.RepetitionCounter = 0;
    htim8.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    if (HAL_TIM_Base_Init(&htim8) != HAL_OK)
    {
        error_handler();
    }
    sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
    if (HAL_TIM_ConfigClockSource(&htim8, &sClockSourceConfig) != HAL_OK)
    {
        error_handler();
    }
    if (HAL_TIM_PWM_Init(&htim8) != HAL_OK)
    {
        error_handler();
    }
    sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
    sMasterConfig.MasterOutputTrigger2 = TIM_TRGO2_RESET;
    sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
    if (HAL_TIMEx_MasterConfigSynchronization(&htim8, &sMasterConfig) != HAL_OK)
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
    if (HAL_TIM_PWM_ConfigChannel(&htim8, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
    {
        error_handler();
    }
    if (HAL_TIM_PWM_ConfigChannel(&htim8, &sConfigOC, TIM_CHANNEL_3) != HAL_OK)
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
    if (HAL_TIMEx_ConfigBreakDeadTime(&htim8, &sBreakDeadTimeConfig) != HAL_OK)
    {
        error_handler();
    }
    tim_msp_post_init(&htim8);
		return PFC_SUCCESS;
}

/**
  * @brief TIM9 Initialization Function
  */
static status_t tim9_Init(void)
{
    TIM_OC_InitTypeDef sConfigOC = {0};

    htim9.Instance = TIM9;
    htim9.Init.Prescaler = TIMER9_PRESCALER;
    htim9.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim9.Init.Period = 0;
    htim9.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim9.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    if (HAL_TIM_PWM_Init(&htim9) != HAL_OK)
    {
        error_handler();
    }
    sConfigOC.OCMode = TIM_OCMODE_PWM1;
    sConfigOC.Pulse = 0;
    sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
    sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
    if (HAL_TIM_PWM_ConfigChannel(&htim9, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
    {
        error_handler();
    }
    tim_msp_post_init(&htim9);
		return PFC_SUCCESS;
}

/*--------------------------------------------------------------
                       PUBLIC FUNCTIONS
--------------------------------------------------------------*/

/**
* @brief TIM_Base MSP Initialization
* This function configures the hardware resources used in this example
* @param htim_base: TIM_Base handle pointer
*/
void HAL_TIM_Base_MspInit(TIM_HandleTypeDef* htim_base)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    if (htim_base->Instance == TIM1)
    {
        /* Peripheral clock enable */
        __HAL_RCC_TIM1_CLK_ENABLE();

        __HAL_RCC_GPIOE_CLK_ENABLE();

        GPIO_InitStruct.Pin = GPIO_PIN_15;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
        GPIO_InitStruct.Alternate = GPIO_AF1_TIM1;
        HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

        /* TIM1 DMA Init */
        /* TIM1_CH1 Init */
        hdma_tim1_ch1.Instance = TIMER1_CH1_DMA_STREAM;
        hdma_tim1_ch1.Init.Channel = TIMER1_CH1_DMA_CHANNEL;
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
            error_handler();
        }

        __HAL_LINKDMA(htim_base, hdma[TIM_DMA_ID_CC1], hdma_tim1_ch1);

        /* TIM1_CH2 Init */
        hdma_tim1_ch2.Instance = TIMER1_CH2_DMA_STREAM;
        hdma_tim1_ch2.Init.Channel = TIMER1_CH2_DMA_CHANNEL;
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
            error_handler();
        }

        __HAL_LINKDMA(htim_base, hdma[TIM_DMA_ID_CC2], hdma_tim1_ch2);

        /* TIM1_CH3 Init */
        hdma_tim1_ch3.Instance = TIMER1_CH3_DMA_STREAM;
        hdma_tim1_ch3.Init.Channel = TIMER1_CH3_DMA_CHANNEL;
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
            error_handler();
        }

        __HAL_LINKDMA(htim_base, hdma[TIM_DMA_ID_CC3], hdma_tim1_ch3);
    }
    else if (htim_base->Instance == TIM2)
    {
        __HAL_RCC_TIM2_CLK_ENABLE();
    }
    else if (htim_base->Instance == TIM8)
    {
        __HAL_RCC_TIM8_CLK_ENABLE();

        __HAL_RCC_GPIOA_CLK_ENABLE();

        GPIO_InitStruct.Pin = GPIO_PIN_8;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
        GPIO_InitStruct.Alternate = GPIO_AF3_TIM8;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

        hdma_tim8_ch2.Instance = TIMER8_CH2_DMA_STREAM;
        hdma_tim8_ch2.Init.Channel = TIMER8_CH2_DMA_CHANNEL;
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
            error_handler();
        }

        __HAL_LINKDMA(htim_base, hdma[TIM_DMA_ID_CC2], hdma_tim8_ch2);

        /* TIM8_CH3 Init */
        hdma_tim8_ch3.Instance = TIMER8_CH3_DMA_STREAM;
        hdma_tim8_ch3.Init.Channel = TIMER8_CH3_DMA_CHANNEL;
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
            error_handler();
        }

        __HAL_LINKDMA(htim_base, hdma[TIM_DMA_ID_CC3], hdma_tim8_ch3);
    }
}

/**
* @brief TIM_PWM MSP Initialization
* This function configures the hardware resources used in this example
* @param htim_pwm: TIM_PWM handle pointer
*/
void HAL_TIM_PWM_MspInit(TIM_HandleTypeDef* htim_pwm)
{
    if (htim_pwm->Instance == TIM9)
    {
        __HAL_RCC_TIM9_CLK_ENABLE();
    }
}


/**
* @brief TIM_Base MSP De-Initialization
* This function freeze the hardware resources used in this example
* @param htim_base: TIM_Base handle pointer
*/

void HAL_TIM_Base_MspDeInit(TIM_HandleTypeDef* htim_base)
{
    if (htim_base->Instance == TIM1)
    {
        /* Peripheral clock disable */
        __HAL_RCC_TIM1_CLK_DISABLE();

        HAL_GPIO_DeInit(GPIOE, CH1_PFC_LOW_TIM1CH1N_Pin | CH1_PFC_HIGH_TIM1CH1_Pin | CH2_PFC_LOW_TIM1CH2N_Pin | CH2_PFC_HIGH_TIM1CH2_Pin | CH3_PFC_LOW_TIM1CH3N_Pin | CH3_PFC_HIGH_TIM1CH3_Pin | GPIO_PIN_15);

        HAL_DMA_DeInit(htim_base->hdma[TIM_DMA_ID_CC1]);
        HAL_DMA_DeInit(htim_base->hdma[TIM_DMA_ID_CC2]);
        HAL_DMA_DeInit(htim_base->hdma[TIM_DMA_ID_CC3]);
    }
    else if (htim_base->Instance == TIM2)
    {
        __HAL_RCC_TIM2_CLK_DISABLE();
    }
    else if (htim_base->Instance == TIM8)
    {
        __HAL_RCC_TIM8_CLK_DISABLE();

        HAL_GPIO_DeInit(GPIOB, CH1_ET_LOW_TIM8_CH2N_Pin | CH2_ET_LOW_TIM8_CH3N_Pin);
        HAL_GPIO_DeInit(GPIOC, CH1_ET_HIGH_TIM8_CH2_Pin | CH2_ET_HIGH_TIM8_CH3_Pin);
        HAL_GPIO_DeInit(GPIOA, GPIO_PIN_8);

        HAL_DMA_DeInit(htim_base->hdma[TIM_DMA_ID_CC2]);
        HAL_DMA_DeInit(htim_base->hdma[TIM_DMA_ID_CC3]);
    }
}

/**
* @brief TIM_PWM MSP De-Initialization
* This function freeze the hardware resources used in this example
* @param htim_pwm: TIM_PWM handle pointer
*/

void HAL_TIM_PWM_MspDeInit(TIM_HandleTypeDef* htim_pwm)
{
    if (htim_pwm->Instance == TIM9)
    {
        __HAL_RCC_TIM9_CLK_DISABLE();
    }
}

status_t timer_write_pwm(uint32_t ccr1, uint32_t ccr2, uint32_t ccr3)
{
	TIM1->CCR1 = ccr1;
	TIM1->CCR2 = ccr2;
	TIM1->CCR3 = ccr3;
	return PFC_SUCCESS;
}

status_t timer_correct_period(uint32_t arr)
{
	DINT;
	TIM2->ARR = arr;
	TIM2->EGR = TIM_EGR_UG;
	EINT;
	return PFC_SUCCESS;
}

status_t timer_restore_pwm(void)
{
	TIM1->CCER |= (TIM_CCER_CC1E);
	TIM1->CCER |= (TIM_CCER_CC1NE);
	TIM1->CCER |= (TIM_CCER_CC2E);
	TIM1->CCER |= (TIM_CCER_CC2NE);
	TIM1->CCER |= (TIM_CCER_CC3E);
	TIM1->CCER |= (TIM_CCER_CC3NE);

	__HAL_TIM_MOE_ENABLE(&htim1);
	__HAL_TIM_ENABLE(&htim1);
	
	return PFC_SUCCESS;
}

status_t timer_disable_pwm(void)
{
    __HAL_TIM_MOE_DISABLE(&htim1);
    __HAL_TIM_DISABLE(&htim1);

    TIM1->CCER &= ~(TIM_CCER_CC1E);
    TIM1->CCER &= ~(TIM_CCER_CC1NE);
    TIM1->CCER &= ~(TIM_CCER_CC2E);
    TIM1->CCER &= ~(TIM_CCER_CC2NE);
    TIM1->CCER &= ~(TIM_CCER_CC3E);
    TIM1->CCER &= ~(TIM_CCER_CC3NE);
    return PFC_SUCCESS;
}

status_t timer_start_adc_timer(void)
{
    HAL_TIM_Base_Start(&htim2);
    return PFC_SUCCESS;
}

status_t timer_init(void)
{
	tim8_Init();
	tim1_Init();
	tim9_Init();
	tim2_Init();
	return PFC_SUCCESS;
}

/**
  * @brief This function handles DMA2 stream1 global interrupt.
  */
void TIMER1_CH1_DMA_IRQ(void)
{
  HAL_DMA_IRQHandler(&hdma_tim1_ch1);
}

/**
  * @brief This function handles DMA2 stream2 global interrupt.
  */
void TIMER1_CH2_DMA_IRQ(void)
{
  HAL_DMA_IRQHandler(&hdma_tim1_ch2);
}

/**
  * @brief This function handles DMA2 stream6 global interrupt.
  */
void TIMER1_CH3_DMA_IRQ(void)
{
  HAL_DMA_IRQHandler(&hdma_tim1_ch3);
}

/**
  * @brief This function handles DMA2 stream3 global interrupt.
  */
void TIMER8_CH2_DMA_IRQ(void)
{
  HAL_DMA_IRQHandler(&hdma_tim8_ch2);
}

/**
  * @brief This function handles DMA2 stream4 global interrupt.
  */
void TIMER8_CH3_DMA_IRQ(void)
{
  HAL_DMA_IRQHandler(&hdma_tim8_ch3);
}

