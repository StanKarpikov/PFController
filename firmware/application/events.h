/**
 * @file events.h
 * @author Stanislav Karpikov
 * @brief Events management (status, warnings, errors, faults)
 */

#ifndef EVENTS_H_
#define EVENTS_H_

/** @addtogroup app_events
 * @{
 */
 

/*--------------------------------------------------------------
                       INCLUDE
--------------------------------------------------------------*/

#include "stdint.h"

/*--------------------------------------------------------------
                       DEFINES
--------------------------------------------------------------*/

#define EVENTS_RECORDS_NUM (0x3F) /**< The number of the events storage */

/*--------------------------------------------------------------
                       PUBLIC TYPES
--------------------------------------------------------------*/

/** The structure to store an event */
struct __attribute__((__packed__)) event_record_s
{
    uint64_t unix_time_s_ms;

    uint32_t type;
    uint32_t info;
    float value;
};

/** Event types: main */
typedef enum
{
    EVENT_TYPE_POWER,       /**< Power parameters has been changed */
    EVENT_TYPE_CHANGESTATE, /**< The PFC state has been changed */
    EVENT_TYPE_PROTECTION,  /**< The protection has been activated */
    EVENT_TYPE_EVENT        /**< An other event */
} event_type_t;

/*--------------------------------------------------------------
                       PUBLIC FUNCTIONS
--------------------------------------------------------------*/

/**
 * @brief Add a new event (external function)
 * 
 * @param main The main event type
 * @param sub The sub event type
 * @param info The event info
 * @param value The event data
 */
void events_new_event(event_type_t main, uint32_t sub, uint32_t info, float value);

/**
 * @brief Clear the events storage
 */
void events_clear(void);

/**
 * @brief Get events from the storage
 * 
 * @param after_index The start index in the events storage
 * @param num The number of events to write
 * @param buf The buffer to write events
 *
 * @return The count of events have been written
 */
uint16_t events_get(uint64_t after_index, uint16_t num, struct event_record_s* buf);

/** @} */
#endif /* EVENTS_H_ */
