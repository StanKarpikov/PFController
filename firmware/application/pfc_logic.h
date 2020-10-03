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

typedef enum
{
    COMMAND_WORK_ON = 1,
    COMMAND_WORK_OFF,
    COMMAND_CHARGE_ON,
    COMMAND_CHARGE_OFF,
    COMMAND_CHANNEL0_DATA,
    COMMAND_CHANNEL1_DATA,
    COMMAND_CHANNEL2_DATA,
    COMMAND_SETTINGS_SAVE
}pfc_commands_t;

/*--------------------------------------------------------------
                       PUBLIC FUNCTIONS
--------------------------------------------------------------*/

status_t pfc_apply_command(pfc_commands_t command, uint32_t data);
void pfc_process(void);
pfc_state_t pfc_get_state(void);

#endif /* _PFC_LOGIC_H */
