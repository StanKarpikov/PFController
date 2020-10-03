#ifndef EVENTS_H_
#define EVENTS_H_

#include "stdint.h"

#define EVENTS_RECORDS_NUM 0x3F

struct __attribute__((__packed__)) sEventRecord
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

extern struct sEventRecord newevent;

void NEWEVENT(event_type_t MAIN, uint32_t SUB, uint32_t INFO, float VALUE);

extern struct sEventRecord events[EVENTS_RECORDS_NUM + 1];
extern uint16_t eventsIN, eventsOUT, eventsNUM;

void EventsClear(void);
void EventsAdd(struct sEventRecord* event);
uint16_t EventsGet(uint64_t afterIndex, uint16_t num, struct sEventRecord* buf);

#endif /* EVENTS_H_ */
