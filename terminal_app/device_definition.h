/**
 * @file device_definition.h
 * @author Stanislav Karpikov
 * @brief Definitions for the PFC. Should match the firmware
 */

#ifndef DEVICE_DEFINITION_H
#define DEVICE_DEFINITION_H

#include <stdint.h>
#include <type_traits>

template <typename E>
constexpr auto enum_int(E e) noexcept
{
    return static_cast<std::underlying_type_t<E>>(e);
}

#pragma pack(1) /**< Enable packing to work with protocol structures */

namespace PFCconfig {
    auto const PFC_NCHAN = 3U;   /**< Three-phase network */

    namespace ADC {
        enum {
            ADC_UD,			//CH10
            ADC_U_A,		//CH11
            ADC_U_B,		//CH12
            ADC_U_C,		//CH13
            ADC_I_A,		//CH0
            ADC_I_B,		//CH1
            ADC_I_C,		//CH2
            ADC_I_ET,		//CH3
            ADC_I_TEMP1,//CH5
            ADC_I_TEMP2,//CH6
            ADC_EMS_A,	//CH14
            ADC_EMS_B,	//CH15
            ADC_EMS_C,	//CH8
            ADC_EMS_I,
            ADC_MATH_A,
            ADC_MATH_B,
            ADC_MATH_C
        };
        auto const ADC_CHANNEL_NUMBER = ADC_MATH_A;
        auto const ADC_MATH_NUMBER = 3;
    }

    namespace Events {
        /** The structure to store an event */
        struct event_record_s
        {
            uint64_t unix_time_s_ms;

            uint32_t type;
            uint32_t info;
            float value;
        };
    }

    namespace Interface {

        enum OscillogCnannel{
            OSC_UD,
            OSC_U_A,
            OSC_U_B,
            OSC_U_C,
            OSC_I_A,
            OSC_I_B,
            OSC_I_C,
            OSC_COMP_A,
            OSC_COMP_B,
            OSC_COMP_C,
            OSC_CHANNEL_NUMBER
        };

        auto const OSCILLOG_TRANSFER_SIZE = 128; /**< The size of an oscillogram */
        auto const MAX_EVENTS_PACKET_SIZE = 150; /**< The maximum size of the packet that can be occupied by events */

        /** The maximum number of events that can be transferred */
        auto const MAX_NUM_TRANSFERED_EVENTS = (MAX_EVENTS_PACKET_SIZE / (sizeof(Events::event_record_s)));

        /** brief Event types: subevents for power control */
        enum
        {
            SUB_EVENT_TYPE_POWER_ON
        };

        /** Event types: subevents for protection */
        enum
        {
            SUB_EVENT_TYPE_PROTECTION_UCAP_MIN,
            SUB_EVENT_TYPE_PROTECTION_UCAP_MAX,
            SUB_EVENT_TYPE_PROTECTION_TEMPERATURE,
            SUB_EVENT_TYPE_PROTECTION_U_MIN,
            SUB_EVENT_TYPE_PROTECTION_U_MAX,
            SUB_EVENT_TYPE_PROTECTION_F_MIN,
            SUB_EVENT_TYPE_PROTECTION_F_MAX,
            SUB_EVENT_TYPE_PROTECTION_IAFG_MAX_RMS,
            SUB_EVENT_TYPE_PROTECTION_IAFG_MAX_PEAK,
            SUB_EVENT_TYPE_PROTECTION_PHASES,
            SUB_EVENT_TYPE_PROTECTION_ADC_OVERLOAD,
            SUB_EVENT_TYPE_PROTECTION_BAD_SYNC,
            SUB_EVENT_TYPE_PROTECTION_IGBT,
        };

        /** Protection event types */
        enum
        {
            PROTECTION_IGNORE,
            PROTECTION_WARNING_STOP,
            PROTECTION_ERROR_STOP
        };

        /** Protocol commands list */
        enum class pfc_interface_commands_t
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
        };
    }

}

#endif // DEVICE_DEFINITION_H
