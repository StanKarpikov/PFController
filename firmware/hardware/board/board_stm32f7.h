/**
 * @file board_stm32f7.h
 * @author Stanislav Karpikov
 * @brief Board definitions
 */

#ifndef _BOARD_STM32F7_H
#define _BOARD_STM32F7_H

/*---------------------  INCLUDES  -------------------------------------------*/

/*-------------------  PUBLIC DEFINES  ---------------------------------------*/

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
#define RE_485_Pin                     GPIO_PIN_11
#define RE_485_GPIO_Port               GPIOA

#endif /* _BOARD_STM32F7_H */
