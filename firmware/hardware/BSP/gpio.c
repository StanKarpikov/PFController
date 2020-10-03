/**
 * @file gpio.c
 * @author Stanislav Karpikov
 * @brief Board support package: GPIO
 */

/*---------------------  INCLUDES  -------------------------------------------*/

#include "BSP/gpio.h"
#include "BSP/debug.h"
#include "stm32f7xx_hal.h"

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOD, DISABLE_DRIVER_KKM_Pin|DISABLE_DRIVER_ET_Pin|LED_3_Pin|LED_2_Pin 
                          |LED_1_Pin|RELE_2_Pin|RELE_1_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(RE_485_GPIO_Port, RE_485_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : TAHOMETER_1_Pin TAHOMETER_2_Pin */
  GPIO_InitStruct.Pin = TAHOMETER_1_Pin|TAHOMETER_2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /*Configure GPIO pins : DISABLE_DRIVER_KKM_Pin DISABLE_DRIVER_ET_Pin LED_3_Pin LED_2_Pin 
                           LED_1_Pin RELE_2_Pin RELE_1_Pin */
  GPIO_InitStruct.Pin = DISABLE_DRIVER_KKM_Pin|DISABLE_DRIVER_ET_Pin|LED_3_Pin|LED_2_Pin 
                          |LED_1_Pin|RELE_2_Pin|RELE_1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /*Configure GPIO pin : RE_485_Pin */
  GPIO_InitStruct.Pin = RE_485_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(RE_485_GPIO_Port, &GPIO_InitStruct);

}
