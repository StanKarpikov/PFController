/**
 * @file board_stm32f7.h
 * @author Stanislav Karpikov
 * @brief Board definitions
 */

#ifndef _BOARD_STM32F7_H
#define _BOARD_STM32F7_H

/*--------------------------------------------------------------
                       PUBLIC DEFINES
--------------------------------------------------------------*/

#define TAHOMETER_1_Pin                GPIO_PIN_3
#define TAHOMETER_1_GPIO_Port          GPIOE
#define TAHOMETER_2_Pin                GPIO_PIN_4
#define TAHOMETER_2_GPIO_Port          GPIOE
#define PWM_KYLER_Pin                  GPIO_PIN_5
#define PWM_KYLER_GPIO_Port            GPIOE

#define U_DC_ADC_Pin                   GPIO_PIN_0
#define U_DC_ADC_GPIO_Port             GPIOC

#define A_HALF_ADC_Pin                 GPIO_PIN_1
#define A_HALF_ADC_GPIO_Port           GPIOC
#define B_HALF_ADC_Pin                 GPIO_PIN_2
#define B_HALF_ADC_GPIO_Port           GPIOC
#define C_HALF_ADC_Pin                 GPIO_PIN_3
#define C_HALF_ADC_GPIO_Port           GPIOC

#define I_ADC_PFC_A_Pin                GPIO_PIN_0
#define I_ADC_PFC_A_GPIO_Port          GPIOA
#define I_ADC_PFC_B_Pin                GPIO_PIN_1
#define I_ADC_PFC_B_GPIO_Port          GPIOA
#define I_ADC_PFC_C_Pin                GPIO_PIN_2
#define I_ADC_PFC_C_GPIO_Port          GPIOA
#define I_ADC_ET_Pin                   GPIO_PIN_3
#define I_ADC_ET_GPIO_Port             GPIOA

#define TEMP_1_Pin                     GPIO_PIN_5
#define TEMP_1_GPIO_Port               GPIOA
#define TEMP_2_Pin                     GPIO_PIN_6
#define TEMP_2_GPIO_Port               GPIOA

#define A_EMS_ADC_Pin                  GPIO_PIN_4
#define A_EMS_ADC_GPIO_Port            GPIOC
#define B_EMS_ADC_Pin                  GPIO_PIN_5
#define B_EMS_ADC_GPIO_Port            GPIOC
#define C_EMS_ADC_Pin                  GPIO_PIN_0
#define C_EMS_ADC_GPIO_Port            GPIOB
#define I_EMS_OUT_Pin                  GPIO_PIN_1
#define I_EMS_OUT_GPIO_Port            GPIOB

#define CH1_PFC_LOW_TIM1CH1N_Pin       GPIO_PIN_8
#define CH1_PFC_LOW_TIM1CH1N_GPIO_Port GPIOE
#define CH1_PFC_HIGH_TIM1CH1_Pin       GPIO_PIN_9
#define CH1_PFC_HIGH_TIM1CH1_GPIO_Port GPIOE
#define CH2_PFC_LOW_TIM1CH2N_Pin       GPIO_PIN_10
#define CH2_PFC_LOW_TIM1CH2N_GPIO_Port GPIOE
#define CH2_PFC_HIGH_TIM1CH2_Pin       GPIO_PIN_11
#define CH2_PFC_HIGH_TIM1CH2_GPIO_Port GPIOE
#define CH3_PFC_LOW_TIM1CH3N_Pin       GPIO_PIN_12
#define CH3_PFC_LOW_TIM1CH3N_GPIO_Port GPIOE
#define CH3_PFC_HIGH_TIM1CH3_Pin       GPIO_PIN_13
#define CH3_PFC_HIGH_TIM1CH3_GPIO_Port GPIOE
#define CH1_ET_LOW_TIM8_CH2N_Pin       GPIO_PIN_14
#define CH1_ET_LOW_TIM8_CH2N_GPIO_Port GPIOB
#define CH2_ET_LOW_TIM8_CH3N_Pin       GPIO_PIN_15
#define CH2_ET_LOW_TIM8_CH3N_GPIO_Port GPIOB

#define DISABLE_DRIVER_PFC_Pin         GPIO_PIN_8
#define DISABLE_DRIVER_PFC_GPIO_Port   GPIOD
#define DISABLE_DRIVER_ET_Pin          GPIO_PIN_9
#define DISABLE_DRIVER_ET_GPIO_Port    GPIOD

#define LED_3_Pin                      GPIO_PIN_11
#define LED_3_GPIO_Port                GPIOD
#define LED_2_Pin                      GPIO_PIN_12
#define LED_2_GPIO_Port                GPIOD
#define LED_1_Pin                      GPIO_PIN_13
#define LED_1_GPIO_Port                GPIOD
#define RELE_2_Pin                     GPIO_PIN_14
#define RELE_2_GPIO_Port               GPIOD
#define RELE_1_Pin                     GPIO_PIN_15
#define RELE_1_GPIO_Port               GPIOD

#define CH1_ET_HIGH_TIM8_CH2_Pin       GPIO_PIN_7
#define CH1_ET_HIGH_TIM8_CH2_GPIO_Port GPIOC
#define CH2_ET_HIGH_TIM8_CH3_Pin       GPIO_PIN_8
#define CH2_ET_HIGH_TIM8_CH3_GPIO_Port GPIOC

#define RS485_UART1_TX_Pin             GPIO_PIN_9
#define RS485_UART1_TX_GPIO_Port       GPIOA

#define RS485_UART1_RX_Pin             GPIO_PIN_10
#define RS485_UART1_RX_GPIO_Port       GPIOA

#define RE_485_Pin                     GPIO_PIN_12
#define RE_485_GPIO_Port               GPIOA
#define RS485_GPIO_PORT_ENABLE 				 __HAL_RCC_GPIOA_CLK_ENABLE

#define USART_INTERFACE USART1
#define USART_INTERFACE_IRQ USART1_IRQHandler
#define USART_INTERFACE_IRQN USART1_IRQn
#define USART_INTERFACE_AF GPIO_AF7_USART1
#define USART_INTERFACE_RCC_DISABLE __HAL_RCC_USART1_CLK_DISABLE
#define USART_INTERFACE_RCC_ENABLE __HAL_RCC_USART1_CLK_ENABLE

#define USART_INTERFACE_DMA_RX_IRQ DMA2_Stream5_IRQHandler
#define USART_INTERFACE_DMA_RX_STREAM DMA2_Stream5
#define USART_INTERFACE_DMA_RX_CHANNEL DMA_CHANNEL_4

#define USART_INTERFACE_DMA_TX_IRQ DMA2_Stream7_IRQHandler
#define USART_INTERFACE_DMA_TX_STREAM DMA2_Stream7
#define USART_INTERFACE_DMA_TX_CHANNEL DMA_CHANNEL_4

#define ADC_ID ADC1
#define ADC_CLK_ENABLE __HAL_RCC_ADC1_CLK_ENABLE
#define ADC_CLK_DISABLE __HAL_RCC_ADC1_CLK_DISABLE
#define ADC_DMA_IRQ DMA2_Stream0_IRQHandler

#define ADC_DMA_STREAM DMA2_Stream0
#define ADC_DMA_CHANNEL DMA_CHANNEL_0

#define TIMER1_CH1_DMA_STREAM DMA2_Stream1
#define TIMER1_CH1_DMA_CHANNEL DMA_CHANNEL_6
#define TIMER1_CH1_DMA_IRQ DMA2_Stream1_IRQHandler
#define TIMER1_CH2_DMA_STREAM DMA2_Stream2
#define TIMER1_CH2_DMA_CHANNEL DMA_CHANNEL_6
#define TIMER1_CH2_DMA_IRQ DMA2_Stream2_IRQHandler
#define TIMER1_CH3_DMA_STREAM DMA2_Stream6
#define TIMER1_CH3_DMA_CHANNEL DMA_CHANNEL_6
#define TIMER1_CH3_DMA_IRQ DMA2_Stream6_IRQHandler

#define TIMER8_CH3_DMA_STREAM DMA2_Stream4
#define TIMER8_CH3_DMA_CHANNEL DMA_CHANNEL_7
#define TIMER8_CH2_DMA_IRQ DMA2_Stream3_IRQHandler
#define TIMER8_CH2_DMA_STREAM DMA2_Stream3
#define TIMER8_CH2_DMA_CHANNEL DMA_CHANNEL_7
#define TIMER8_CH3_DMA_IRQ DMA2_Stream4_IRQHandler

#endif /* _BOARD_STM32F7_H */
