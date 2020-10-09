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

/** Protocol commands list */
typedef enum
{
    PFC_COMMAND_TEST,               /**< Test the connection */
    PFC_COMMAND_SWITCH_ON_OFF,      /**< PFC switch on and off */
    PFC_COMMAND_GET_ADC_ACTIVE,     /**< Get instantaneous adc values */
    PFC_COMMAND_GET_ADC_ACTIVE_RAW, /**< Get instantaneous adc values before calibrations */
    PFC_COMMAND_GET_OSCILLOG,       /**< Get oscillograms */
    PFC_COMMAND_GET_NET_PARAMS,     /**< Get network parameters */

    PFC_COMMAND_SET_SETTINGS_CALIBRATIONS, /**< Set calibration settings */
    PFC_COMMAND_SET_SETTINGS_PROTECTION,   /**< Set protection settings */
    PFC_COMMAND_SET_SETTINGS_CAPACITORS,   /**< Set capacitors settings */
    PFC_COMMAND_SET_SETTINGS_FILTERS,      /**< Set filter settings */

    PFC_COMMAND_GET_SETTINGS_CALIBRATIONS, /**< Get calibration settings */
    PFC_COMMAND_GET_SETTINGS_PROTECTION,   /**< Get protection settings */
    PFC_COMMAND_GET_SETTINGS_CAPACITORS,   /**< Get capacitors settings */
    PFC_COMMAND_GET_SETTINGS_FILTERS,      /**< Get filter settings */

    PFC_COMMAND_GET_WORK_STATE, /**< Get current PFC state */

    PFC_COMMAND_GET_VERSION_INFO, /**< Get firmware info */
    PFC_COMMAND_GET_EVENTS,       /**< Get events */

    PFC_COMMAND_COUNT /**< The length of the structure */
} pfc_interface_commands_t;

/*--------------------------------------------------------------
                       PUBLIC FUNCTIONS
--------------------------------------------------------------*/

/**
 * @brief Init handlers for protocol commands
 * 
 * @return The operation status
 */
status_t command_handlers_init(void);

/**
 * @brief Collect oscillogram data
 * 
 * @param osc_adc_ch A pointer to the ADC structure (should be in the specific format)
 *
 * @return The operation status
 */
status_t protocol_write_osc_data(float** osc_adc_ch);

#endif /*_COMMAND_PROCESSOR_H */
