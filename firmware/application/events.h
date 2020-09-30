#ifndef EVENTS_H_
#define EVENTS_H_

#include "stdint.h"

#define EVENTS_RECORDS_NUM 0x3F

struct __attribute__((__packed__)) sEventRecord{
	uint64_t unixTime_s_ms;

	uint32_t type;
	uint32_t info;
    float    value;
};
extern struct sEventRecord newevent;
#define NEWEVENT(MAIN, SUB, INFO, VALUE) \
		do{\
			__disable_irq(); \
			newevent.unixTime_s_ms=currentTime; \
			__enable_irq(); \
			newevent.type=( MAIN )|(((uint32_t) SUB )<<16); \
			newevent.info= INFO ; \
			newevent.value= VALUE ; \
			EventsAdd(&newevent); \
		}while(0)

extern struct sEventRecord events[EVENTS_RECORDS_NUM+1];
extern uint16_t eventsIN,eventsOUT,eventsNUM;

void EventsClear(void);
void EventsAdd(struct sEventRecord *event);
uint16_t EventsGet(uint64_t afterIndex, uint16_t num, struct sEventRecord* buf);

#endif /* EVENTS_H_ */
