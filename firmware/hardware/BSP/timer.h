/**
 * @file timer.h
 * @author Stanislav Karpikov
 * @brief Board support package: Timer (header)
 */

#ifndef _TIMER_H
#define _TIMER_H

/*--------------------------------------------------------------
                       INCLUDES
--------------------------------------------------------------*/

#include "BSP/debug.h"
#include "stdint.h"

/*--------------------------------------------------------------
                       PUBLIC FUNCTIONS
--------------------------------------------------------------*/

/**
 * @brief Init the timer module
 *
 * @return The status of the operation
 */
status_t timer_init(void);

/**
 * @brief Start timer for the ADC module
 *
 * @return The status of the operation
 */
status_t timer_start_adc_timer(void);

/**
 * @brief Disable PWM
 *
 * @return The status of the operation
 */
status_t timer_disable_pwm(void);

/**
 * @brief Enable PWM (after disabling)
 *
 * @return The status of the operation
 */
status_t timer_restore_pwm(void);

/**
 * @brief Set PWM values
 *
 * @param ccr1 Timer CCR1 register contents
 * @param ccr2 Timer CCR2 register contents
 * @param ccr3 Timer CCR3 register contents
 *
 * @return The status of the operation
 */
status_t timer_write_pwm(uint32_t ccr1, uint32_t ccr2, uint32_t ccr3);

/**
 * @brief Change syncronisation timer period
 *
 * @param arr The new value of the ARR register
 *
 * @return The status of the operation
 */
status_t timer_correct_period(uint32_t arr);

#endif /* _TIMER_H */
