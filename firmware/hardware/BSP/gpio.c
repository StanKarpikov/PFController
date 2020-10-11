/**
 * @file gpio.c
 * @author Stanislav Karpikov
 * @brief Board support package: GPIO
 */

/** @addtogroup hdw_bsp_gpio
 * @{
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

/*
 * @breif Switch on the main relay output
 * 
 * @return The status of the operation
 */
status_t gpio_main_relay_switch_off(void)
{
    HAL_GPIO_WritePin(GPIOD, RELAY_2_Pin, GPIO_PIN_RESET);
    return PFC_SUCCESS;
}

/*
 * @breif Switch off the main relay output
 * 
 * @return The status of the operation
 */
status_t gpio_main_relay_switch_on(void)
{
    HAL_GPIO_WritePin(GPIOD, RELAY_2_Pin, GPIO_PIN_SET);
    return PFC_SUCCESS;
}

/*
 * @breif Switch on the preload relay output
 * 
 * @return The status of the operation
 */
status_t gpio_preload_relay_switch_off(void)
{
    HAL_GPIO_WritePin(GPIOD, RELAY_1_Pin, GPIO_PIN_RESET);
    return PFC_SUCCESS;
}

/*
 * @breif Switch off the preload relay output
 * 
 * @return The status of the operation
 */
status_t gpio_preload_relay_switch_on(void)
{
    HAL_GPIO_WritePin(GPIOD, RELAY_1_Pin, GPIO_PIN_SET);
    return PFC_SUCCESS;
}

/*
 * @breif Switch on the ventilators output
 * 
 * @return The status of the operation
 */
status_t gpio_ventilators_switch_on(void)
{
    //TODO:
    return PFC_SUCCESS;
}

/*
 * @breif Switch off the ventilators output
 * 
 * @return The status of the operation
 */
status_t gpio_ventilators_switch_off(void)
{
    //TODO:
    return PFC_SUCCESS;
}

/*
 * @breif Switch on the PWM test output
 * 
 * @return The status of the operation
 */
status_t gpio_pwm_test_on(void)
{
    /* TODO: Test pin can be added to measure PWM set time */
    return PFC_SUCCESS;
}

/*
 * @breif Switch off the PWM test output
 * 
 * @return The status of the operation
 */
status_t gpio_pwm_test_off(void)
{
    /* TODO: Test pin can be added to measure PWM set time */
    return PFC_SUCCESS;
}

/*
 * @breif Switch on the error LED output
 * 
 * @return The status of the operation
 */
status_t gpio_error_led_on(void)
{
    HAL_GPIO_WritePin(GPIOD, LED_1_Pin, GPIO_PIN_SET);
    return PFC_SUCCESS;
}

/*
 * @breif Switch on the status LED output
 * 
 * @return The status of the operation
 */
status_t gpio_status_led_on(void)
{
    HAL_GPIO_WritePin(GPIOD, LED_2_Pin, GPIO_PIN_SET);
    return PFC_SUCCESS;
}

/*
 * @breif Switch on the error LED output
 * 
 * @return The status of the operation
 */
status_t gpio_error_led_off(void)
{
    HAL_GPIO_WritePin(GPIOD, LED_1_Pin, GPIO_PIN_RESET);
    return PFC_SUCCESS;
}

/*
 * @breif Switch off the status LED output
 * 
 * @return The status of the operation
 */
status_t gpio_status_led_off(void)
{
    HAL_GPIO_WritePin(GPIOD, LED_2_Pin, GPIO_PIN_RESET);
    return PFC_SUCCESS;
}

/*
 * @brief GPIO initialization
 * 
 * @return The status of the operation
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
/** @} */
