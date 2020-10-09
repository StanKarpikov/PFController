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

/**
 * @brief ADC DMA half complete callback
 * 
 * @param adc_values ADC active values
 *
 * @return The status of the operation
 */
status_t events_check_adc_overload(float *adc_values);

/**
 * @brief Check capacitors voltage
 * 
 * @param Ucap The capacitors voltage
 *
 * @return The status of the operation
 */
status_t events_check_ud(float Ucap);

/**
 * @brief Check the temperature
 *
 * @return The status of the operation
 */
status_t events_check_temperature(void);

/**
 * @brief Check the RMS voltage
 *
 * @return The status of the operation
 */
status_t events_check_rms_voltage(void);

/**
 * @brief Check overvoltage (amplitude)
 *
 * @param U A pointer to the channel voltages
 *
 * @return The status of the operation
 */
status_t events_check_overvoltage(float *U);

/**
 * @brief Check RMS overcurrent
 *
 * @return The status of the operation
 */
status_t events_check_rms_overcurrent(void);

/**
 * @brief Check overcurrent (amplitude)
 *
 * @param I A pointer to the currents array
 *
 * @return The status of the operation
 */
status_t events_check_overcurrent(float *I);

/**
 * @brief Check the period and frequency
 *
 * @param period The value of the period
 *
 * @return The status of the operation
 */
status_t events_check_period(uint32_t period);

/**
 * @brief Check the input channel phases
 *
 * @return The status of the operation
 */
status_t events_check_voltage_phase_rotation(void);

/**
 * @brief Check the current on the load
 *
 * @return The status of the operation
 */
status_t events_check_loading_current(void);

/**
 * @brief Check preloading operation (preloading has been started and is ongoing in the right direction)
 *
 * @return The status of the operation
 */
status_t events_check_preload_start(void);

/**
 * @brief Check preloading operation (preloading was stopped)
 *
 * @return The status of the operation
 */
status_t events_check_preload_stop(void);

#endif /*__EVENTS_PROCESS_H__ */
