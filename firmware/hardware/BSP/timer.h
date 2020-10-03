/**
 * @file timer.h
 * @author Stanislav Karpikov
 * @brief Board support package: Timer (header)
 */

#ifndef _TIMER_H
#define _TIMER_H

/*---------------------  INCLUDES  -------------------------------------------*/

#include "BSP/debug.h"

/*------------------  PUBLIC FUNCTIONS  --------------------------------------*/

void MX_TIM8_Init(void);
void MX_TIM1_Init(void);
void MX_TIM9_Init(void);
void MX_TIM2_Init(void);
status_t timer_start_adc_timer(void);

#endif /* _TIMER_H */
