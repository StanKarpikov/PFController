/**
 * @file events.c
 * @author Stanislav Karpikov
 * @brief Events management (status, warnings, errors, faults)
 */

/*--------------------------------------------------------------
                       INCLUDE
--------------------------------------------------------------*/

#include "events.h"
#include "pfc_logic.h"
#include "command_processor.h"
#include "BSP/system.h"
#include "string.h"

/*--------------------------------------------------------------
                       DEFINES
--------------------------------------------------------------*/

#define EVENTS_REPEAT_TIME (2000)

/*--------------------------------------------------------------
                       PRIVATE DATA
--------------------------------------------------------------*/

static struct event_record_s events[EVENTS_RECORDS_NUM + 1]={0};

static uint16_t events_in = 0;
static uint16_t events_out = 0;
static uint32_t lastEventTime[SUB_EVENT_TYPE_PROTECTION_IGBT + 1][ADC_CHANNEL_NUMBER]={0};

static const uint16_t ProtectionLevels[] = {
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

/*--------------------------------------------------------------
                       PRIVATE FUNCTIONS
--------------------------------------------------------------*/

static void events_lock(void)
{
	DINT;
}

static void events_unlock(void)
{
	EINT;
}

/*--------------------------------------------------------------
                       PRIVATE FUNCTIONS
--------------------------------------------------------------*/

static void events_check(uint16_t subtype, uint32_t info)
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

static void events_add(struct event_record_s* event)
{
    if ((event->type & 0xFFFF) == EVENT_TYPE_PROTECTION)
    {
        uint16_t subtype = (event->type >> 16) & 0xFFFF;
        events_check(subtype, event->info);
        events_lock();
        if ((system_get_time() - lastEventTime[subtype][event->info]) < EVENTS_REPEAT_TIME)
        {
            events_unlock();
            return;
        }
        events_unlock();
        lastEventTime[subtype][event->info] = system_get_time();
    }

    events_lock();
    events[events_in] = *event;

    events_in++;
    events_in &= EVENTS_RECORDS_NUM;

    if (events_in == events_out)
    {
        events_out++;
        events_out &= EVENTS_RECORDS_NUM;
    }
    events_unlock();
}

/*--------------------------------------------------------------
                       PUBLIC FUNCTIONS
--------------------------------------------------------------*/

void events_new_event(event_type_t main, uint32_t sub, uint32_t info, float value)
{                              
	  struct event_record_s newevent={0};
		newevent.unixTime_s_ms = system_get_time();                               
		newevent.type = (main) | (((uint32_t)sub) << 16); 
		newevent.info = info;                             
		newevent.value = value;                           
		events_add(&newevent);                             
}

void events_clear(void)
{
    events_lock();
    int i = 0;
    for (i = 0; i < EVENTS_RECORDS_NUM; i++)
    {
        events[i].unixTime_s_ms = 0;
    }
    events_in = 0;
    events_out = 0;
    memset(lastEventTime, 0, sizeof(lastEventTime));
    events_unlock();
}

uint16_t events_get(uint64_t after_index, uint16_t num, struct event_record_s* buf)
{
    uint16_t count = 0;
    events_lock();
    uint16_t ev_out = events_out;
    while (ev_out != events_in)
    {
        if (events[ev_out].unixTime_s_ms >= after_index)
        {
            memcpy(buf, &events[ev_out], sizeof(struct event_record_s));
            buf++;
            after_index = events[ev_out].unixTime_s_ms;
            count++;
            if (count >= num)
            {
                events_unlock();
                return count;
            }
        }
        ev_out++;
        ev_out &= EVENTS_RECORDS_NUM;
    }
    events_unlock();
    return count;
}
