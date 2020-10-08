/**
 * @file pfc_logic.h
 * @author Stanislav Karpikov
 * @brief State management and PFC control functions (header)
 */
 
#ifndef _PFC_LOGIC_H
#define _PFC_LOGIC_H

/*--------------------------------------------------------------
                       INCLUDES
--------------------------------------------------------------*/

#include "defines.h"
#include "settings.h"
#include "BSP/debug.h"

/*--------------------------------------------------------------
                       PUBLIC TYPES
--------------------------------------------------------------*/

/** State of the PFC */
typedef enum
{
    PFC_STATE_INIT,               /**< Initial state */
    PFC_STATE_STOP,               /**< Stop state (power hardware is switched off) */
    PFC_STATE_SYNC,               /**< Syncronisation with the network */
    PFC_STATE_PRECHARGE_PREPARE,  /**< Prepare precharge */
    PFC_STATE_PRECHARGE,          /**< Precharge (connector is in on state) */
    PFC_STATE_MAIN,               /**< Main state. Precharge is finished */
    PFC_STATE_PRECHARGE_DISABLE,  /**< Precharge is switching off */
    PFC_STATE_WORK,               /**< Ready state */
    PFC_STATE_CHARGE,             /**< Main capacitors charge is ongoing */
    PFC_STATE_TEST,               /**< Test state */
		PFC_STATE_STOPPING,           /**< Stoping work: disable sensitive and power peripheral */
    PFC_STATE_FAULTBLOCK,         /**< Fault state */
		PFC_STATE_COUNT		       	  	/**< The count of the states */
} pfc_state_t;

/** PFC commands fromt the panel */
typedef enum
{
    COMMAND_WORK_ON = 1, /**< Switch on the PFC operation */
    COMMAND_WORK_OFF, /**< Switch off the PFC operation */
    COMMAND_CHARGE_ON, /**< Switch charge on */
    COMMAND_CHARGE_OFF,/**< Switch charge off */
    COMMAND_CHANNEL0_DATA,/**< Set the channel A data (enable the channel) */
    COMMAND_CHANNEL1_DATA,/**< Set the channel B data (enable the channel) */
    COMMAND_CHANNEL2_DATA,/**< Set the channel C data (enable the channel) */
    COMMAND_SETTINGS_SAVE /**< Save settings */
}pfc_commands_t;

/*--------------------------------------------------------------
                       PUBLIC FUNCTIONS
--------------------------------------------------------------*/

/**
 * @brief Execute a command from the panel
 *
 * @param command The command to execute
 * @param data Command data
 * 
 * @return Status of the operation
 */
status_t pfc_apply_command(pfc_commands_t command, uint32_t data);

/**
 * @brief Process the current PFC state (called in a cycle)
 */
void pfc_process(void);

/**
 * @brief Get the current PFC state
 *
 * @return The PFC state
 */
pfc_state_t pfc_get_state(void);

/**
 * @brief Block the PFC in case of a fault event
 */
void pfc_faultblock(void);

/**
 * @brief Get the current PWM state
 *
 * @return The PWM state
 */
uint8_t pfc_is_pwm_on(void);

#endif /* _PFC_LOGIC_H */
