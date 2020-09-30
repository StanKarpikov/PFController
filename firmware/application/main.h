/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  ** This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * COPYRIGHT(c) 2018 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f7xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "math.h"
#include "string.h"
#include "stdbool.h"

#define DINT __disable_irq()
#define EINT __enable_irq()

bool is_debug_session(void);
#define BREAKPOINT() if( is_debug_session() )\
										{\
											__asm  {BKPT 0}\
										}
										
extern uint64_t currentTime;
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define TAHOMETER_1_Pin GPIO_PIN_3
#define TAHOMETER_1_GPIO_Port GPIOE
#define TAHOMETER_2_Pin GPIO_PIN_4
#define TAHOMETER_2_GPIO_Port GPIOE
#define PWM_KYLER_Pin GPIO_PIN_5
#define PWM_KYLER_GPIO_Port GPIOE
#define U_DC_ADC_Pin GPIO_PIN_0
#define U_DC_ADC_GPIO_Port GPIOC
#define A_HALF_ADC_Pin GPIO_PIN_1
#define A_HALF_ADC_GPIO_Port GPIOC
#define B_HALF_ADC_Pin GPIO_PIN_2
#define B_HALF_ADC_GPIO_Port GPIOC
#define C_HALF_ADC_Pin GPIO_PIN_3
#define C_HALF_ADC_GPIO_Port GPIOC
#define I_ADC_KKM_A_Pin GPIO_PIN_0
#define I_ADC_KKM_A_GPIO_Port GPIOA
#define I_ADC_KKM_B_Pin GPIO_PIN_1
#define I_ADC_KKM_B_GPIO_Port GPIOA
#define I_ADC_KKM_C_Pin GPIO_PIN_2
#define I_ADC_KKM_C_GPIO_Port GPIOA
#define I_ADC_ET_Pin GPIO_PIN_3
#define I_ADC_ET_GPIO_Port GPIOA
#define TEMP_1_Pin GPIO_PIN_5
#define TEMP_1_GPIO_Port GPIOA
#define TEMP_2_Pin GPIO_PIN_6
#define TEMP_2_GPIO_Port GPIOA
#define A_EMS_ADC_Pin GPIO_PIN_4
#define A_EMS_ADC_GPIO_Port GPIOC
#define B_EMS_ADC_Pin GPIO_PIN_5
#define B_EMS_ADC_GPIO_Port GPIOC
#define C_EMS_ADC_Pin GPIO_PIN_0
#define C_EMS_ADC_GPIO_Port GPIOB
#define I_EMS_OUT_Pin GPIO_PIN_1
#define I_EMS_OUT_GPIO_Port GPIOB
#define CH1_KKM_LOW_TIM1CH1N_Pin GPIO_PIN_8
#define CH1_KKM_LOW_TIM1CH1N_GPIO_Port GPIOE
#define CH1_KKM_HIGH_TIM1CH1_Pin GPIO_PIN_9
#define CH1_KKM_HIGH_TIM1CH1_GPIO_Port GPIOE
#define CH2_KKM_LOW_TIM1CH2N_Pin GPIO_PIN_10
#define CH2_KKM_LOW_TIM1CH2N_GPIO_Port GPIOE
#define CH2_KKM_HIGH_TIM1CH2_Pin GPIO_PIN_11
#define CH2_KKM_HIGH_TIM1CH2_GPIO_Port GPIOE
#define CH3_KKM_LOW_TIM1CH3N_Pin GPIO_PIN_12
#define CH3_KKM_LOW_TIM1CH3N_GPIO_Port GPIOE
#define CH3_KKM_HIGH_TIM1CH3_Pin GPIO_PIN_13
#define CH3_KKM_HIGH_TIM1CH3_GPIO_Port GPIOE
#define CH1_ET_LOW_TIM8_CH2N_Pin GPIO_PIN_14
#define CH1_ET_LOW_TIM8_CH2N_GPIO_Port GPIOB
#define CH2_ET_LOW_TIM8_CH3N_Pin GPIO_PIN_15
#define CH2_ET_LOW_TIM8_CH3N_GPIO_Port GPIOB
#define DISABLE_DRIVER_KKM_Pin GPIO_PIN_8
#define DISABLE_DRIVER_KKM_GPIO_Port GPIOD
#define DISABLE_DRIVER_ET_Pin GPIO_PIN_9
#define DISABLE_DRIVER_ET_GPIO_Port GPIOD
#define LED_3_Pin GPIO_PIN_11
#define LED_3_GPIO_Port GPIOD
#define LED_2_Pin GPIO_PIN_12
#define LED_2_GPIO_Port GPIOD
#define LED_1_Pin GPIO_PIN_13
#define LED_1_GPIO_Port GPIOD
#define RELE_2_Pin GPIO_PIN_14
#define RELE_2_GPIO_Port GPIOD
#define RELE_1_Pin GPIO_PIN_15
#define RELE_1_GPIO_Port GPIOD
#define CH1_ET_HIGH_TIM8_CH2_Pin GPIO_PIN_7
#define CH1_ET_HIGH_TIM8_CH2_GPIO_Port GPIOC
#define CH2_ET_HIGH_TIM8_CH3_Pin GPIO_PIN_8
#define CH2_ET_HIGH_TIM8_CH3_GPIO_Port GPIOC
#define RS485_UART1_TX_Pin GPIO_PIN_9
#define RS485_UART1_TX_GPIO_Port GPIOA
#define RS485_UART1_RX_Pin GPIO_PIN_10
#define RS485_UART1_RX_GPIO_Port GPIOA
#define RE_485_Pin GPIO_PIN_11
#define RE_485_GPIO_Port GPIOA
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
