/**
 * @file board_stm32f723_disco.h
 * @author Stanislav Karpikov
 * @brief Board definitions (STM32F723E-DISCO development board)
 */

#ifndef _BOARD_STM32F723_DISCO_H
#define _BOARD_STM32F723_DISCO_H

/** @addtogroup hdw_bsp_board
 * @{
 */
 
/*
Board LEDs:

LD1  BLUE   ARDUINO         PA5    -> RS485 RE
LD5  RED    USER1           PA7    -> RELAY_1
LD6  GREEN  USER2           PB1    -> RELAY_2
LD7  RED    USB_OTG_HS_OVCR PH10   -> LED_3
LD8  GREEN  VBUS_USB_HS     PB13   -> LED_2
LD9  RED    USB_OTG_FS_OVCR PB10   -> LED_1
LD10 GREEN  VBUS_USB_FS     PA9

*/
 
/*--------------------------------------------------------------
                       PUBLIC DEFINES
--------------------------------------------------------------*/

#define BOARD_REVISION 				"0.1" /**< The current PCB revision */

#ifndef STM32F723xx
	#error "STM32F723xx should be defined in the project settings"
#endif

/*--------------------------------------------------------------
											PUBLIC DEFINES::CONFIGURATION
--------------------------------------------------------------*/

#define ADC_MOCKING /**< Define to imitate the ADC insteade of using real data */
#define PWM_MOCKING /**< Define to imitate the PWM output */
#define EFMC_MOCKING /**< Define to imitate the EFMC output */
#define COOLER_MOCKING /**< Define to imitate the cooler output */

/*--------------------------------------------------------------
											PUBLIC DEFINES::EEPROM
--------------------------------------------------------------*/

#define EEPROM_START_ADDRESS ((uint32_t)(0x08008000)) /**< The start address of the EEPROM block */
#define EEPROM_PAGE0_SECTOR  (FLASH_SECTOR_2)        /**< The ID of the first EEPROM sector */
#define EEPROM_PAGE1_SECTOR  (FLASH_SECTOR_3)        /**< The ID of the second EEPROM sector */

/*--------------------------------------------------------------
											PUBLIC DEFINES::GPIO
--------------------------------------------------------------*/

#define TAHOMETER_1_Pin       GPIO_PIN_3
#define TAHOMETER_1_GPIO_Port GPIOE
#define TAHOMETER_2_Pin       GPIO_PIN_4
#define TAHOMETER_2_GPIO_Port GPIOE

#define PWM_COOLER_Pin        GPIO_PIN_5
#define PWM_COOLER_GPIO_Port  GPIOE

#define U_DC_ADC_Pin          GPIO_PIN_0
#define U_DC_ADC_GPIO_Port    GPIOC

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

#define LED_3_Pin       GPIO_PIN_10
#define LED_3_GPIO_Port GPIOH
#define LED_2_Pin       GPIO_PIN_13
#define LED_2_GPIO_Port GPIOB
#define LED_1_Pin       GPIO_PIN_10
#define LED_1_GPIO_Port GPIOB

#define RELAY_2_Pin       GPIO_PIN_1
#define RELAY_2_GPIO_Port GPIOB
#define RELAY_1_Pin       GPIO_PIN_7
#define RELAY_1_GPIO_Port GPIOA

/* UART module configuration */
#define RS485_UART_TX_Pin       GPIO_PIN_6
#define RS485_UART_TX_GPIO_Port GPIOC

#define RS485_UART_RX_Pin       GPIO_PIN_7
#define RS485_UART_RX_GPIO_Port GPIOC
#define RS485_GPIO_PORT_ENABLE  __HAL_RCC_GPIOC_CLK_ENABLE
#define USART_INTERFACE_AF          GPIO_AF8_USART6

#define RE_485_Pin              GPIO_PIN_5
#define RE_485_GPIO_Port        GPIOA
#define RS485_RE_GPIO_PORT_ENABLE  __HAL_RCC_GPIOA_CLK_ENABLE
#define USART_INTERFACE_RE_AF          0

#define USART_INTERFACE             USART6
#define USART_INTERFACE_IRQ         USART6_IRQHandler
#define USART_INTERFACE_IRQN        USART6_IRQn
#define USART_INTERFACE_RCC_DISABLE __HAL_RCC_USART6_CLK_DISABLE
#define USART_INTERFACE_RCC_ENABLE  __HAL_RCC_USART6_CLK_ENABLE

#define USART_INTERFACE_DMA_RX_IRQ     DMA2_Stream1_IRQHandler
#define USART_INTERFACE_DMA_RX_IRQN    DMA2_Stream1_IRQn
#define USART_INTERFACE_DMA_RX_STREAM  DMA2_Stream1
#define USART_INTERFACE_DMA_RX_CHANNEL DMA_CHANNEL_5

#define USART_INTERFACE_DMA_TX_IRQ     DMA2_Stream6_IRQHandler
#define USART_INTERFACE_DMA_TX_IRQN    DMA2_Stream6_IRQn
#define USART_INTERFACE_DMA_TX_STREAM  DMA2_Stream6
#define USART_INTERFACE_DMA_TX_CHANNEL DMA_CHANNEL_5

/* ADC module configuration */
#define ADC_ID          ADC1
#define ADC_CLK_ENABLE  __HAL_RCC_ADC1_CLK_ENABLE
#define ADC_CLK_DISABLE __HAL_RCC_ADC1_CLK_DISABLE
#define ADC_DMA_IRQ     DMA2_Stream0_IRQHandler
#define ADC_DMA_IRQN    DMA2_Stream0_IRQn

#define ADC_DMA_STREAM  DMA2_Stream0
#define ADC_DMA_CHANNEL DMA_CHANNEL_0

/* Timer module configuration */
#define TIMER_PWM                 TIM1
#define TIMER_PWM_CLK_ENABLE      __HAL_RCC_TIM1_CLK_ENABLE
#define TIMER_PWM_CLK_DISABLE     __HAL_RCC_TIM1_CLK_DISABLE
#define TIMER_PWM_CH1_DMA_STREAM  DMA2_Stream1
#define TIMER_PWM_CH1_DMA_CHANNEL DMA_CHANNEL_6
#define TIMER_PWM_CH1_DMA_IRQ     irq_place_holder_1
#define TIMER_PWM_CH1_DMA_IRQN     DMA2_Stream1_IRQn
#define TIMER_PWM_CH2_DMA_STREAM  DMA2_Stream2
#define TIMER_PWM_CH2_DMA_CHANNEL DMA_CHANNEL_6
#define TIMER_PWM_CH2_DMA_IRQ     DMA2_Stream2_IRQHandler
#define TIMER_PWM_CH2_DMA_IRQN    DMA2_Stream2_IRQn
#define TIMER_PWM_CH3_DMA_STREAM  DMA2_Stream6
#define TIMER_PWM_CH3_DMA_CHANNEL DMA_CHANNEL_6
#define TIMER_PWM_CH3_DMA_IRQ     irq_place_holder_2
#define TIMER_PWM_CH3_DMA_IRQN    DMA2_Stream6_IRQn

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
#define TIMER_EFMC_CH2_DMA_IRQN       DMA2_Stream4_IRQn
#define TIMER_EFMC_CH1_DMA_STREAM     DMA2_Stream3
#define TIMER_EFMC_CH1_DMA_CHANNEL    DMA_CHANNEL_7
#define TIMER_EFMC_CH1_DMA_IRQ        DMA2_Stream3_IRQHandler
#define TIMER_EFMC_CH1_DMA_IRQN       DMA2_Stream3_IRQn
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
#define TIMER_SYNC_IRQ				 TIM2_IRQHandler
#define TIMER_SYNC_IRQN				 TIM2_IRQn

#define TIMER_COOLER             TIM9
#define TIMER_COOLER_CLK_ENABLE  __HAL_RCC_TIM9_CLK_ENABLE
#define TIMER_COOLER_CLK_DISABLE __HAL_RCC_TIM9_CLK_DISABLE

/** @} */
#endif /* _BOARD_STM32F723_DISCO_H */
