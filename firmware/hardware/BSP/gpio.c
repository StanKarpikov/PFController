/**
 * @file gpio.c
 * @author Stanislav Karpikov
 * @brief Board support package: GPIO
 */

/*--------------------------------------------------------------
                       INCLUDES
--------------------------------------------------------------*/

#include "BSP/gpio.h"

#include "BSP/bsp.h"
#include "BSP/debug.h"
#include "stm32f7xx_hal.h"

/*--------------------------------------------------------------
                       PUBLIC FUNCTIONS
--------------------------------------------------------------*/

status_t Relay_Main_Off(void)
{
    HAL_GPIO_WritePin(GPIOD, RELAY_2_Pin, GPIO_PIN_RESET);
    return PFC_SUCCESS;
}

status_t Relay_Main_On(void)
{
    HAL_GPIO_WritePin(GPIOD, RELAY_2_Pin, GPIO_PIN_SET);
    return PFC_SUCCESS;
}

status_t Relay_Preload_Off(void)
{
    HAL_GPIO_WritePin(GPIOD, RELAY_1_Pin, GPIO_PIN_RESET);
    return PFC_SUCCESS;
}

status_t Relay_Preload_On(void)
{
    HAL_GPIO_WritePin(GPIOD, RELAY_1_Pin, GPIO_PIN_SET);
    return PFC_SUCCESS;
}

status_t ventilators_on(void)
{
    //TODO:
    return PFC_SUCCESS;
}

status_t ventilators_off(void)
{
    //TODO:
    return PFC_SUCCESS;
}
status_t gpio_pwm_test_on(void)
{
    /* TODO: Test pin can be added to measure PWM set time */
    return PFC_SUCCESS;
}

status_t gpio_pwm_test_off(void)
{
    /* TODO: Test pin can be added to measure PWM set time */
    return PFC_SUCCESS;
}

status_t gpio_error_led_on(void)
{
    HAL_GPIO_WritePin(GPIOD, LED_1_Pin, GPIO_PIN_SET);
    return PFC_SUCCESS;
}

status_t gpio_status_led_on(void)
{
    HAL_GPIO_WritePin(GPIOD, LED_2_Pin, GPIO_PIN_SET);
    return PFC_SUCCESS;
}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
status_t gpio_init(void)
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
    HAL_GPIO_WritePin(GPIOD, DISABLE_DRIVER_PFC_Pin | DISABLE_DRIVER_ET_Pin | LED_3_Pin | LED_2_Pin | LED_1_Pin | RELAY_2_Pin | RELAY_1_Pin, GPIO_PIN_RESET);

    /*Configure GPIO pin Output Level */
    HAL_GPIO_WritePin(RE_485_GPIO_Port, RE_485_Pin, GPIO_PIN_RESET);

    /*Configure GPIO pins : TAHOMETER_1_Pin TAHOMETER_2_Pin */
    GPIO_InitStruct.Pin = TAHOMETER_1_Pin | TAHOMETER_2_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

    /*Configure GPIO pins : DISABLE_DRIVER_PFC_Pin DISABLE_DRIVER_ET_Pin LED_3_Pin LED_2_Pin 
                           LED_1_Pin RELAY_2_Pin RELAY_1_Pin */
    GPIO_InitStruct.Pin = DISABLE_DRIVER_PFC_Pin | DISABLE_DRIVER_ET_Pin | LED_3_Pin | LED_2_Pin | LED_1_Pin | RELAY_2_Pin | RELAY_1_Pin;
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

    return PFC_SUCCESS;
}
