#ifndef __EVENTS_PROCESS_H__
#define __EVENTS_PROCESS_H__

#include "defines.h"
#include "device.h"
#include "events.h"
#include "main.h"
#include "protocol.h"

#define EVENT_OK 0

void events_check_loading_current(void);
char events_was_CT_overload(float *adc_Iloading_values);

int events_check_Ud(float Ud);
void events_check_temperature(void);
void events_check_voltage_RMS(void);
void events_check_overvoltage_transient(float *U);
void events_check_voltage_rms(void);
int events_check_overcurrent_rms(void);
int events_check_overcurrent_peak(float *Iafg);
void events_check_supply(void);
void events_check_period(unsigned int period_length);
void events_check_voltage_phase_rotation(void);

int events_tz_error_occured(char tz_ch);
void events_tz_clear_errors(void);

void events_check_ext_sync_from_isr(void);
unsigned long events_get_last_ext_sync(void);
void events_ext_sync_now(void);

void events_preload_start(void);
void events_preload_stop(void);
char events_preload_is_started(void);
void events_check_preload(void);

void events_check_ct_connection(void);

void events_autoconfig_error(unsigned char channel, float reactive);
void events_underdosing_error(unsigned char channel, float compensation_ratio);

#endif /*__EVENTS_PROCESS_H__ */
