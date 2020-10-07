/**
 * @file events.h
 * @author Stanislav Karpikov
 * @brief Process events (status, warnings, errors, faults)
 */
 
#ifndef EVENTS_H_
#define EVENTS_H_

/*--------------------------------------------------------------
                       INCLUDE
--------------------------------------------------------------*/

#include "stdint.h"

/*--------------------------------------------------------------
                       DEFINES
--------------------------------------------------------------*/

#define EVENTS_RECORDS_NUM 0x3F

/*--------------------------------------------------------------
                       PUBLIC TYPES
--------------------------------------------------------------*/

struct __attribute__((__packed__)) event_record_s
{
    uint64_t unixTime_s_ms;

    uint32_t type;
    uint32_t info;
    float value;
};

typedef enum
{
    EVENT_TYPE_POWER,
    EVENT_TYPE_CHANGESTATE,
    EVENT_TYPE_PROTECTION,
    EVENT_TYPE_EVENT
}event_type_t;

/*--------------------------------------------------------------
                       PUBLIC FUNCTIONS
--------------------------------------------------------------*/

void events_new_event(event_type_t main, uint32_t sub, uint32_t info, float value);
void events_clear(void);
uint16_t events_get(uint64_t after_index, uint16_t num, struct event_record_s* buf);

#endif /* EVENTS_H_ */
