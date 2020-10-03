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

#include "stdint.h"
#include "BSP/debug.h"

/*--------------------------------------------------------------
                       PUBLIC FUNCTIONS
--------------------------------------------------------------*/

status_t timer_init(void);
status_t timer_start_adc_timer(void);
status_t timer_disable_pwm(void);
status_t timer_restore_pwm(void);
status_t timer_write_pwm(uint32_t ccr1, uint32_t ccr2, uint32_t ccr3);
status_t timer_correct_period(uint32_t arr);

#endif /* _TIMER_H */
