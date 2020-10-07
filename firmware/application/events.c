#include "events.h"

#include "pfc_logic.h"
#include "command_processor.h"
#include "BSP/system.h"
#include "string.h"

#define EVENTS_REPEAT_TIME 2000
struct sEventRecord events[EVENTS_RECORDS_NUM + 1];
struct sEventRecord newevent;

uint16_t eventsIN = 0, eventsOUT = 0, eventsNUM = 0;
uint32_t lastEventTime[SUB_EVENT_TYPE_PROTECTION_IGBT + 1][ADC_CHANNEL_NUMBER];

void NEWEVENT(event_type_t MAIN, uint32_t SUB, uint32_t INFO, float VALUE)
{                              
		newevent.unixTime_s_ms = system_get_time();                               
		newevent.type = (MAIN) | (((uint32_t)SUB) << 16); 
		newevent.info = INFO;                             
		newevent.value = VALUE;                           
		EventsAdd(&newevent);                             
}


void EventsClear()
{
    DINT;
    int i = 0;
    for (i = 0; i < EVENTS_RECORDS_NUM; i++)
    {
        events[i].unixTime_s_ms = 0;
    }
    eventsIN = 0;
    eventsOUT = 0;
    eventsNUM = 0;
    memset(lastEventTime, 0, sizeof(lastEventTime));
    EINT;
}

uint16_t ProtectionLevels[] = {
    PROTECTION_WARNING_STOP,  //SUB_EVENT_TYPE_PROTECTION_UD_MIN
    PROTECTION_ERROR_STOP,    //SUB_EVENT_TYPE_PROTECTION_UD_MAX
    PROTECTION_ERROR_STOP,    //SUB_EVENT_TYPE_PROTECTION_TEMPERATURE
    PROTECTION_WARNING_STOP,  //SUB_EVENT_TYPE_PROTECTION_U_MIN
    PROTECTION_WARNING_STOP,  //SUB_EVENT_TYPE_PROTECTION_U_MAX
    PROTECTION_WARNING_STOP,  //SUB_EVENT_TYPE_PROTECTION_F_MIN
    PROTECTION_WARNING_STOP,  //SUB_EVENT_TYPE_PROTECTION_F_MAX
    PROTECTION_ERROR_STOP,    //SUB_EVENT_TYPE_PROTECTION_I_MAX_RMS
    PROTECTION_ERROR_STOP,    //SUB_EVENT_TYPE_PROTECTION_I_MAX_PEAK
    PROTECTION_WARNING_STOP,  //SUB_EVENT_TYPE_PROTECTION_PHASES
    PROTECTION_IGNORE,        //SUB_EVENT_TYPE_PROTECTION_ADC_OVERLOAD
    PROTECTION_WARNING_STOP,  //SUB_EVENT_TYPE_PROTECTION_BAD_SYNC
    PROTECTION_ERROR_STOP     //SUB_EVENT_TYPE_PROTECTION_IGBT
};
inline void CheckEvents(uint16_t subtype, uint32_t info)
{
    switch (ProtectionLevels[subtype])
    {
        case PROTECTION_IGNORE:
            break;
        case PROTECTION_WARNING_STOP:
            if (pfc_get_state() >= PFC_STATE_SYNC)
            {
                pfc_faultblock();
            }
            break;
        case PROTECTION_ERROR_STOP:
            if (pfc_get_state() >= PFC_STATE_SYNC)
            {
                pfc_faultblock();
            }
            break;
    }
}
void EventsAdd(struct sEventRecord* event)
{
    if ((event->type & 0xFFFF) == EVENT_TYPE_PROTECTION)
    {
        uint16_t subtype = (event->type >> 16) & 0xFFFF;
        CheckEvents(subtype, event->info);
        DINT;
        if ((system_get_time() - lastEventTime[subtype][event->info]) < EVENTS_REPEAT_TIME)
        {
            EINT;
            return;
        }
        EINT;
        lastEventTime[subtype][event->info] = system_get_time();
    }

    DINT;
    events[eventsIN] = *event;

    eventsIN++;
    eventsIN &= EVENTS_RECORDS_NUM;

    if (eventsIN == eventsOUT)
    {
        eventsOUT++;
        eventsOUT &= EVENTS_RECORDS_NUM;
    }
    EINT;
}

uint16_t EventsGet(uint64_t afterIndex, uint16_t num, struct sEventRecord* buf)
{
    uint16_t count = 0;
    DINT;
    uint16_t eOUT = eventsOUT;
    while (eOUT != eventsIN)
    {
        if (events[eOUT].unixTime_s_ms >= afterIndex)
        {
            memcpy(buf, &events[eOUT], sizeof(struct sEventRecord));
            buf++;
            afterIndex = events[eOUT].unixTime_s_ms;
            count++;
            if (count >= num)
            {
                EINT;
                return count;
            }
        }
        eOUT++;
        eOUT &= EVENTS_RECORDS_NUM;
    }
    EINT;
    return count;
}
