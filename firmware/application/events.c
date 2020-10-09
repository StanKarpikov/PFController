/**
 * @file events.c
 * @author Stanislav Karpikov
 * @brief Events management (status, warnings, errors, faults)
 */

/** @addtogroup app_events
 * @{
 */
 

/*--------------------------------------------------------------
                       INCLUDE
--------------------------------------------------------------*/

#include "events.h"

#include "BSP/system.h"
#include "command_processor.h"
#include "pfc_logic.h"
#include "string.h"

/*--------------------------------------------------------------
                       DEFINES
--------------------------------------------------------------*/

#define EVENTS_REPEAT_TIME (2000) /**< After this time an event will be repeated */

/*--------------------------------------------------------------
                       PRIVATE DATA
--------------------------------------------------------------*/

/** Events storage */
static struct event_record_s events[EVENTS_RECORDS_NUM + 1] = {0};

static uint16_t events_in = 0;                                                               /**< The number of events have been written to the storage */
static uint16_t events_out = 0;                                                              /**< The number of events have been written from the storage */
static uint32_t lastEventTime[SUB_EVENT_TYPE_PROTECTION_IGBT + 1][ADC_CHANNEL_NUMBER] = {0}; /**< The array of the last timestamps of events */

/** Protection levels for different subevents */
static const uint16_t protection_levels[] = {
    PROTECTION_WARNING_STOP,  //SUB_EVENT_TYPE_PROTECTION_UCAP_MIN
    PROTECTION_ERROR_STOP,    //SUB_EVENT_TYPE_PROTECTION_UCAP_MAX
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

/**
 * @brief Lock the events module data
 */
static void events_lock(void)
{
    ENTER_CRITICAL();
}

/**
 * @brief Lock the events module data
 */
static void events_unlock(void)
{
    EXIT_CRITICAL();
}

/*--------------------------------------------------------------
                       PRIVATE FUNCTIONS
--------------------------------------------------------------*/

/**
 * @brief Check an event: apply a protection action if needed
 * 
 * @param subtype Event type
 * @param info Event info
 */
static void events_check(uint16_t subtype, uint32_t info)
{
    switch (protection_levels[subtype])
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

/**
 * @brief Add a new event
 * 
 * @param event The event data
 */
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

/*
 * @brief Add a new event (external function)
 * 
 * @param main The main event type
 * @param sub The sub event type
 * @param info The event info
 * @param value The event data
 */
void events_new_event(event_type_t main, uint32_t sub, uint32_t info, float value)
{
    struct event_record_s newevent = {0};
    newevent.unix_time_s_ms = system_get_time();
    newevent.type = (main) | (((uint32_t)sub) << 16);
    newevent.info = info;
    newevent.value = value;
    events_add(&newevent);
}

/*
 * @brief Clear the events storage
 */
void events_clear(void)
{
    events_lock();
    int i = 0;
    for (i = 0; i < EVENTS_RECORDS_NUM; i++)
    {
        events[i].unix_time_s_ms = 0;
    }
    events_in = 0;
    events_out = 0;
    memset(lastEventTime, 0, sizeof(lastEventTime));
    events_unlock();
}

/*
 * @brief Get events from the storage
 * 
 * @param after_index The start index in the events storage
 * @param num The number of events to write
 * @param buf The buffer to write events
 *
 * @return The count of events have been written
 */
uint16_t events_get(uint64_t after_index, uint16_t num, struct event_record_s* buf)
{
    uint16_t count = 0;
    events_lock();
    uint16_t ev_out = events_out;
    while (ev_out != events_in)
    {
        if (events[ev_out].unix_time_s_ms >= after_index)
        {
            memcpy(buf, &events[ev_out], sizeof(struct event_record_s));
            buf++;
            after_index = events[ev_out].unix_time_s_ms;
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
/** @} */