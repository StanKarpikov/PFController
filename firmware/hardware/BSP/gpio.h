/**
 * @file gpio.h
 * @author Stanislav Karpikov
 * @brief Board support package: GPIO (header)
 */

#ifndef _GPIO_H
#define _GPIO_H

/*---------------------  INCLUDES  -------------------------------------------*/

#include "BSP/debug.h"

/*------------------  PUBLIC FUNCTIONS  --------------------------------------*/

void MX_GPIO_Init(void);
status_t gpio_error_led_on(void);
status_t gpio_status_led_on(void);

#endif /* _GPIO_H */
