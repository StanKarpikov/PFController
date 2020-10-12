/**
 * @file board_stm32f767_main_v0_2.h
 * @author Stanislav Karpikov
 * @brief Board definitions (main production board)
 */

#ifndef _BOARD_STM32F767_MAIN_H
#define _BOARD_STM32F767_MAIN_H

/** @addtogroup hdw_bsp_board
 * @{
 */
 
/*--------------------------------------------------------------
                       PUBLIC DEFINES
--------------------------------------------------------------*/

#define BOARD_REVISION 				"0.2" /**< The current PCB revision */

#define TAHOMETER_1_Pin       GPIO_PIN_3
#define TAHOMETER_1_GPIO_Port GPIOE
#define TAHOMETER_2_Pin       GPIO_PIN_4
#define TAHOMETER_2_GPIO_Port GPIOE

#define PWM_COOLER_Pin       GPIO_PIN_5
#define PWM_COOLER_GPIO_Port GPIOE

#define U_DC_ADC_Pin       GPIO_PIN_0
#define U_DC_ADC_GPIO_Port GPIOC

#define A_HALF_ADC_Pin       GPIO_PIN_1
#define A_HALF_ADC_GPIO_Port GPIOC
#define B_HALF_ADC_Pin       GPIO_PIN_2
#define B_HALF_ADC_GPIO_Port GPIOC
#define C_HALF_ADC_Pin       GPIO_PIN_3
#define C_HALF_ADC_GPIO_Port GPIOC

#define I_ADC_PFC_A_Pin       GPIO_PIN_0
#define I_ADC_PFC_A_GPIO_Port GPIOA
#define I_ADC_PFC_B_Pin       GPIO_PIN_1
#define I_ADC_PFC_B_GPIO_Port GPIOA
#define I_ADC_PFC_C_Pin       GPIO_PIN_2
#define I_ADC_PFC_C_GPIO_Port GPIOA
#define I_ADC_EFMC_Pin        GPIO_PIN_3
#define I_ADC_EFMC_GPIO_Port  GPIOA

#define TEST_1_Pin       GPIO_PIN_5
#define TEST_1_GPIO_Port GPIOA
#define TEST_2_Pin       GPIO_PIN_6
#define TEST_2_GPIO_Port GPIOA

#define A_EDC_ADC_Pin       GPIO_PIN_4
#define A_EDC_ADC_GPIO_Port GPIOC
#define B_EDC_ADC_Pin       GPIO_PIN_5
#define B_EDC_ADC_GPIO_Port GPIOC
#define C_EDC_ADC_Pin       GPIO_PIN_0
#define C_EDC_ADC_GPIO_Port GPIOB
#define I_EDC_OUT_Pin       GPIO_PIN_1
#define I_EDC_OUT_GPIO_Port GPIOB

#define DISABLE_DRIVER_PFC_Pin       GPIO_PIN_8
#define DISABLE_DRIVER_PFC_GPIO_Port GPIOD
#define DISABLE_DRIVER_ET_Pin        GPIO_PIN_9
#define DISABLE_DRIVER_ET_GPIO_Port  GPIOD

#define LED_3_Pin       GPIO_PIN_11
#define LED_3_GPIO_Port GPIOD
#define LED_2_Pin       GPIO_PIN_12
#define LED_2_GPIO_Port GPIOD
#define LED_1_Pin       GPIO_PIN_13
#define LED_1_GPIO_Port GPIOD

#define RELAY_2_Pin       GPIO_PIN_14
#define RELAY_2_GPIO_Port GPIOD
#define RELAY_1_Pin       GPIO_PIN_15
#define RELAY_1_GPIO_Port GPIOD

/* UART module configuration */
#define RS485_UART_TX_Pin       GPIO_PIN_9
#define RS485_UART_TX_GPIO_Port GPIOA

#define RS485_UART_RX_Pin       GPIO_PIN_10
#define RS485_UART_RX_GPIO_Port GPIOA

#define RE_485_Pin             GPIO_PIN_12
#define RE_485_GPIO_Port       GPIOA
#define RS485_GPIO_PORT_ENABLE __HAL_RCC_GPIOA_CLK_ENABLE

#define USART_INTERFACE             USART1
#define USART_INTERFACE_IRQ         USART1_IRQHandler
#define USART_INTERFACE_IRQN        USART1_IRQn
#define USART_INTERFACE_AF          GPIO_AF7_USART1
#define USART_INTERFACE_RCC_DISABLE __HAL_RCC_USART1_CLK_DISABLE
#define USART_INTERFACE_RCC_ENABLE  __HAL_RCC_USART1_CLK_ENABLE

#define USART_INTERFACE_DMA_RX_IRQ     DMA2_Stream5_IRQHandler
#define USART_INTERFACE_DMA_RX_STREAM  DMA2_Stream5
#define USART_INTERFACE_DMA_RX_CHANNEL DMA_CHANNEL_4

#define USART_INTERFACE_DMA_TX_IRQ     DMA2_Stream7_IRQHandler
#define USART_INTERFACE_DMA_TX_STREAM  DMA2_Stream7
#define USART_INTERFACE_DMA_TX_CHANNEL DMA_CHANNEL_4

/* ADC module configuration */
#define ADC_ID          ADC1
#define ADC_CLK_ENABLE  __HAL_RCC_ADC1_CLK_ENABLE
#define ADC_CLK_DISABLE __HAL_RCC_ADC1_CLK_DISABLE
#define ADC_DMA_IRQ     DMA2_Stream0_IRQHandler

#define ADC_DMA_STREAM  DMA2_Stream0
#define ADC_DMA_CHANNEL DMA_CHANNEL_0

/* Timer module configuration */
#define TIMER_PWM                 TIM1
#define TIMER_PWM_CLK_ENABLE      __HAL_RCC_TIM1_CLK_ENABLE
#define TIMER_PWM_CLK_DISABLE     __HAL_RCC_TIM1_CLK_DISABLE
#define TIMER_PWM_CH1_DMA_STREAM  DMA2_Stream1
#define TIMER_PWM_CH1_DMA_CHANNEL DMA_CHANNEL_6
#define TIMER_PWM_CH1_DMA_IRQ     DMA2_Stream1_IRQHandler
#define TIMER_PWM_CH2_DMA_STREAM  DMA2_Stream2
#define TIMER_PWM_CH2_DMA_CHANNEL DMA_CHANNEL_6
#define TIMER_PWM_CH2_DMA_IRQ     DMA2_Stream2_IRQHandler
#define TIMER_PWM_CH3_DMA_STREAM  DMA2_Stream6
#define TIMER_PWM_CH3_DMA_CHANNEL DMA_CHANNEL_6
#define TIMER_PWM_CH3_DMA_IRQ     DMA2_Stream6_IRQHandler

#define TIMER_PWM_LOW_CH1N_Pin       GPIO_PIN_8
#define TIMER_PWM_LOW_CH1N_GPIO_Port GPIOE
#define TIMER_PWM_HIGH_CH1_Pin       GPIO_PIN_9
#define TIMER_PWM_HIGH_CH1_GPIO_Port GPIOE
#define TIMER_PWM_LOW_CH2N_Pin       GPIO_PIN_10
#define TIMER_PWM_LOW_CH2N_GPIO_Port GPIOE
#define TIMER_PWM_HIGH_CH2_Pin       GPIO_PIN_11
#define TIMER_PWM_HIGH_CH2_GPIO_Port GPIOE
#define TIMER_PWM_LOW_CH3N_Pin       GPIO_PIN_12
#define TIMER_PWM_LOW_CH3N_GPIO_Port GPIOE
#define TIMER_PWM_HIGH_CH3_Pin       GPIO_PIN_13
#define TIMER_PWM_HIGH_CH3_GPIO_Port GPIOE

#define TIMER_EFMC                    TIM8
#define TIMER_EFMC_CLK_ENABLE         __HAL_RCC_TIM8_CLK_ENABLE
#define TIMER_EFMC_CLK_DISABLE        __HAL_RCC_TIM8_CLK_DISABLE
#define TIMER_EFMC_CH2_DMA_STREAM     DMA2_Stream4
#define TIMER_EFMC_CH2_DMA_CHANNEL    DMA_CHANNEL_7
#define TIMER_EFMC_CH2_DMA_IRQ        DMA2_Stream4_IRQHandler
#define TIMER_EFMC_CH1_DMA_STREAM     DMA2_Stream3
#define TIMER_EFMC_CH1_DMA_CHANNEL    DMA_CHANNEL_7
#define TIMER_EFMC_CH1_DMA_IRQ        DMA2_Stream3_IRQHandler
#define TIMER_EFMC_HIGH_CH1_Pin       GPIO_PIN_7
#define TIMER_EFMC_HIGH_CH1_GPIO_Port GPIOC
#define TIMER_EFMC_HIGH_CH2_Pin       GPIO_PIN_8
#define TIMER_EFMC_HIGH_CH2_GPIO_Port GPIOC
#define TIMER_EFMC_LOW_CH1N_Pin       GPIO_PIN_14
#define TIMER_EFMC_LOW_CH1N_GPIO_Port GPIOB
#define TIMER_EFMC_LOW_CH2N_Pin       GPIO_PIN_15
#define TIMER_EFMC_LOW_CH2N_GPIO_Port GPIOB

#define TIMER_SYNC             TIM2
#define TIMER_SYNC_CLK_ENABLE  __HAL_RCC_TIM2_CLK_ENABLE
#define TIMER_SYNC_CLK_DISABLE __HAL_RCC_TIM2_CLK_DISABLE

#define TIMER_COOLER             TIM9
#define TIMER_COOLER_CLK_ENABLE  __HAL_RCC_TIM9_CLK_ENABLE
#define TIMER_COOLER_CLK_DISABLE __HAL_RCC_TIM9_CLK_DISABLE

/** @} */
#endif /* _BOARD_STM32F767_MAIN_H */
