/**
 * @file command_processor.c
 * @author Stanislav Karpikov
 * @brief Analyse commands from the remote panel, handle the interface
 */


#ifndef _COMMAND_PROCESSOR_H
#define _COMMAND_PROCESSOR_H

/*--------------------------------------------------------------
                       INCLUDE
--------------------------------------------------------------*/

#include "defines.h"
#include "protocol.h"
#include "settings.h"

/*--------------------------------------------------------------
                       PUBLIC TYPES
--------------------------------------------------------------*/

typedef enum
{
    PFC_COMMAND_TEST,                /**< Test the connection */
    PFC_COMMAND_SWITCH_ON_OFF,       /**< PFC switch on and off */
    PFC_COMMAND_GET_ADC_ACTIVE,      /**< Get instantaneous adc values */
    PFC_COMMAND_GET_ADC_ACTIVE_RAW,  /**< Get instantaneous adc values before calibrations */
    PFC_COMMAND_GET_OSCILLOG,        /**< Get oscillograms */
    PFC_COMMAND_GET_NET_PARAMS,

    PFC_COMMAND_SET_SETTINGS_CALIBRATIONS,
    PFC_COMMAND_SET_SETTINGS_PROTECTION,
    PFC_COMMAND_SET_SETTINGS_CAPACITORS,
    PFC_COMMAND_SET_SETTINGS_FILTERS,

    PFC_COMMAND_GET_SETTINGS_CALIBRATIONS,
    PFC_COMMAND_GET_SETTINGS_PROTECTION,
    PFC_COMMAND_GET_SETTINGS_CAPACITORS,
    PFC_COMMAND_GET_SETTINGS_FILTERS,

    PFC_COMMAND_GET_WORK_STATE,	   /**< Get current PFC state */

    PFC_COMMAND_GET_VERSION_INFO,  /**< Get firmware info */
    PFC_COMMAND_GET_EVENTS,

    PFC_COMMAND_COUNT
}pfc_interface_commands_t;

/*--------------------------------------------------------------
                       PUBLIC FUNCTIONS
--------------------------------------------------------------*/

void prothandlers_init(SciPort *_port);
void protocol_write_osc_data(float** osc_adc_ch);

#endif  //_COMMAND_PROCESSOR_H
