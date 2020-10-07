/**
 * @file events_process.c
 * @author Stanislav Karpikov
 * @brief Process events (status, warnings, errors, faults)
 */

#ifndef __EVENTS_PROCESS_H__
#define __EVENTS_PROCESS_H__

/*--------------------------------------------------------------
                       INCLUDE
--------------------------------------------------------------*/

#include "defines.h"
#include "command_processor.h"
#include "events.h"
#include "protocol.h"

/*--------------------------------------------------------------
                       PUBLIC FUNCTIONS
--------------------------------------------------------------*/

char events_was_CT_overload(float *adc_Iloading_values);
int events_check_Ud(float Ud);
void events_check_temperature(void);
void events_check_voltage_RMS(void);
void events_check_overvoltage_transient(float *U);
int events_check_overcurrent_rms(void);
int events_check_overcurrent_peak(float *Iafg);
void events_check_period(unsigned int period_length);

void events_check_voltage_rms(void);
void events_check_supply(void);
void events_check_voltage_phase_rotation(void);
void events_check_loading_current(void);

void events_preload_start(void);
void events_preload_stop(void);
void events_preload_is_started(void);
void events_check_preload(void);

#endif /*__EVENTS_PROCESS_H__ */
