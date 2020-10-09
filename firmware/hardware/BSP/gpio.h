/**
 * @file gpio.h
 * @author Stanislav Karpikov
 * @brief Board support package: GPIO (header)
 */

#ifndef _GPIO_H
#define _GPIO_H

/*--------------------------------------------------------------
                       INCLUDES
--------------------------------------------------------------*/

#include "BSP/debug.h"

/*--------------------------------------------------------------
                       PUBLIC FUNCTIONS
--------------------------------------------------------------*/

/**
 * @brief GPIO initialization
 * 
 * @return The status of the operation
 */
status_t gpio_init(void);

/**
 * @breif Switch on the error LED output
 * 
 * @return The status of the operation
 */
status_t gpio_error_led_on(void);

/**
 * @breif Switch off the error LED output
 * 
 * @return The status of the operation
 */
status_t gpio_error_led_off(void);

/**
 * @breif Switch on the status LED output
 * 
 * @return The status of the operation
 */
status_t gpio_status_led_on(void);

/**
 * @breif Switch off the status LED output
 * 
 * @return The status of the operation
 */
status_t gpio_status_led_off(void);

/**
 * @breif Switch on the PWM test output
 * 
 * @return The status of the operation
 */
status_t gpio_pwm_test_on(void);

/**
 * @breif Switch off the PWM test output
 * 
 * @return The status of the operation
 */
status_t gpio_pwm_test_off(void);

/**
 * @breif Switch off the main relay output output
 * 
 * @return The status of the operation
 */
status_t gpio_main_relay_switch_off(void);

/**
 * @breif Switch on the main relay output output
 * 
 * @return The status of the operation
 */
status_t gpio_main_relay_switch_on(void);

/**
 * @breif Switch off the preload relay output output
 * 
 * @return The status of the operation
 */
status_t gpio_preload_relay_switch_off(void);

/**
 * @breif Switch on the preload relay output output
 * 
 * @return The status of the operation
 */
status_t gpio_preload_relay_switch_on(void);

/**
 * @breif Switch on the ventilators output
 * 
 * @return The status of the operation
 */
status_t gpio_ventilators_switch_on(void);

/**
 * @breif Switch off the ventilators output
 * 
 * @return The status of the operation
 */
status_t gpio_ventilators_switch_off(void);

#endif /* _GPIO_H */
