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

void gpio_init(void);
status_t gpio_error_led_on(void);
status_t gpio_status_led_on(void);
status_t gpio_pwm_test_on(void);
status_t gpio_pwm_test_off(void);
status_t Relay_Main_Off(void);
status_t Relay_Main_On(void);
status_t Relay_Preload_Off(void);
status_t Relay_Preload_On(void);
status_t ventilators_on(void);
status_t ventilators_off(void);
	
#endif /* _GPIO_H */
