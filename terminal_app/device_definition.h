/**
 * @file device_definition.h
 * @author Stanislav Karpikov
 * @brief Definitions for the PFC. Should match the firmware
 */

#ifndef DEVICE_DEFINITION_H
#define DEVICE_DEFINITION_H

#include <stdint.h>
#include <type_traits>
#include <QSerialPort>

template <typename E>
constexpr auto enum_uint(E e) noexcept
{
    return static_cast<size_t>(e);
}

template <typename E>
constexpr auto enum_int(E e) noexcept
{
    return static_cast<int32_t>(e);
}

#pragma pack(push)
#pragma pack(1) /**< Enable packing to work with protocol structures */

namespace PFCconfig {
    auto const PFC_NCHAN = 3U;   /**< Three-phase network */

    auto const PFC_ACHAN = 0U;   /**< Channel A */
    auto const PFC_BCHAN = 1U;   /**< Channel B */
    auto const PFC_CCHAN = 2U;   /**< Channel C */

    auto const PFC_NOMINAL_VOLTAGE = 230.0f;   /**< Nominal grid voltage, 230x3, 380 [V] */
    auto const PFC_CALIBRATION_CURRENT = 10.0f; /**< PFC current used for calibration [Amp] */

    namespace Serial {
        const static auto PORT_BADRATE = QSerialPort::Baud115200;
        const static auto PORT_DATA_BITS = QSerialPort::Data8;
        const static auto PORT_PARITY = QSerialPort::NoParity;
        const static auto PORT_STOP_BITS = QSerialPort::OneStop;
        const static auto PORT_FLOW_CONTROL = QSerialPort::NoFlowControl;
        const static auto PORT_ECHO_ENABLED = 0;
        const static auto PORT_TIMEOUT = 1000;
        const static auto PORT_NUMBER_OF_RETRIES = 3;
    }

    namespace ADC {
        enum class ADCchannel{
            ADC_UD,
            ADC_U_A,
            ADC_U_B,
            ADC_U_C,
            ADC_I_A,
            ADC_I_B,
            ADC_I_C,
            ADC_I_ET,
            ADC_I_TEMP1,
            ADC_I_TEMP2,
            ADC_EMS_A,
            ADC_EMS_B,
            ADC_EMS_C,
            ADC_EMS_I,
            ADC_MATH_A,
            ADC_MATH_B,
            ADC_MATH_C
        };
        auto const ADC_CHANNEL_NUMBER = static_cast<size_t>(ADCchannel::ADC_MATH_A);
        auto const ADC_MATH_NUMBER = 3;
        auto const ADC_UREF = 0;
    }

    namespace Events {
        /** The structure to store an event */
        struct EventRecord
        {
            uint64_t unix_time_s_ms;

            uint32_t type;
            uint32_t info;
            float value;
        };
        auto const TIME_MAX_VALUE = 4133894400000ULL; /**< Maximum time constant (used to exclude wrong packets) */

        /** Event types: main */
        enum class EventType
        {
            EVENT_TYPE_POWER,       /**< Power parameters has been changed */
            EVENT_TYPE_CHANGESTATE, /**< The PFC state has been changed */
            EVENT_TYPE_PROTECTION,  /**< The protection has been activated */
            EVENT_TYPE_EVENT        /**< An other event */
        };

        /** brief Event types: subevents for power control */
        enum class SubEventPower
        {
            SUB_EVENT_TYPE_POWER_ON
        };

        /** Event types: subevents for protection */
        enum class SubEventProtection
        {
            SUB_EVENT_TYPE_PROTECTION_UCAP_MIN,
            SUB_EVENT_TYPE_PROTECTION_UCAP_MAX,
            SUB_EVENT_TYPE_PROTECTION_TEMPERATURE,
            SUB_EVENT_TYPE_PROTECTION_U_MIN,
            SUB_EVENT_TYPE_PROTECTION_U_MAX,
            SUB_EVENT_TYPE_PROTECTION_F_MIN,
            SUB_EVENT_TYPE_PROTECTION_F_MAX,
            SUB_EVENT_TYPE_PROTECTION_IPFC_MAX_RMS,
            SUB_EVENT_TYPE_PROTECTION_IAFG_MAX_PEAK,
            SUB_EVENT_TYPE_PROTECTION_PHASES,
            SUB_EVENT_TYPE_PROTECTION_ADC_OVERLOAD,
            SUB_EVENT_TYPE_PROTECTION_BAD_SYNC,
            SUB_EVENT_TYPE_PROTECTION_IGBT,
        };

        /** Protection event types */
        enum class ProtectionActions
        {
            PROTECTION_IGNORE,
            PROTECTION_WARNING_STOP,
            PROTECTION_ERROR_STOP
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
        auto const MAX_NUM_TRANSFERED_EVENTS = (MAX_EVENTS_PACKET_SIZE / (sizeof(Events::EventRecord)));

        /** PFC commands fromt the panel */
        enum class PFCCommands
        {
            COMMAND_WORK_ON = 1,   /**< Switch on the PFC operation */
            COMMAND_WORK_OFF,      /**< Switch off the PFC operation */
            COMMAND_CHARGE_ON,     /**< Switch charge on */
            COMMAND_CHARGE_OFF,    /**< Switch charge off */
            COMMAND_CHANNEL0_DATA, /**< Set the channel A data (enable the channel) */
            COMMAND_CHANNEL1_DATA, /**< Set the channel B data (enable the channel) */
            COMMAND_CHANNEL2_DATA, /**< Set the channel C data (enable the channel) */
            COMMAND_SETTINGS_SAVE  /**< Save settings */
        };

        /** Protocol commands list */
        enum class InterfaceCommands
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

    /** @brief State of the PFC */
    enum class PFCstate
    {
        PFC_STATE_INIT,              /**< Initial state */
        PFC_STATE_STOP,              /**< Stop state (power hardware is switched off) */
        PFC_STATE_SYNC,              /**< Syncronisation with the network */
        PFC_STATE_PRECHARGE_PREPARE, /**< Prepare precharge */
        PFC_STATE_PRECHARGE,         /**< Precharge (connector is in on state) */
        PFC_STATE_MAIN,              /**< Main state. Precharge is finished */
        PFC_STATE_PRECHARGE_DISABLE, /**< Precharge is switching off */
        PFC_STATE_WORK,              /**< Ready state */
        PFC_STATE_CHARGE,            /**< Main capacitors charge is ongoing */
        PFC_STATE_TEST,              /**< Test state */
        PFC_STATE_STOPPING,          /**< Stoping work: disable sensitive and power peripheral */
        PFC_STATE_FAULTBLOCK,        /**< Fault state */
        PFC_STATE_COUNT              /**< The count of the states */
    };

    class PFCsettings{
        /* TODO: is not thread safe */
    public:
        struct {
            float ADC_UD;
            float ADC_U_A;
            float ADC_U_B;
            float ADC_U_C;
            float ADC_I_A;
            float ADC_I_B;
            float ADC_I_C;
            float ADC_I_ET;
            float ADC_I_TEMP1;
            float ADC_I_TEMP2;
            float ADC_EMS_A;
            float ADC_EMS_B;
            float ADC_EMS_C;
            float ADC_EMS_I;
            float ADC_MATH_A;
            float ADC_MATH_B;
            float ADC_MATH_C;
        }adc;
        struct {
            float ADC_UD;
            float ADC_U_A;
            float ADC_U_B;
            float ADC_U_C;
            float ADC_I_A;
            float ADC_I_B;
            float ADC_I_C;
            float ADC_I_ET;
            float ADC_I_TEMP1;
            float ADC_I_TEMP2;
            float ADC_EMS_A;
            float ADC_EMS_B;
            float ADC_EMS_C;
            float ADC_EMS_I;
        }adc_raw;
        struct {
            float period_fact;
            float U0Hz_A;
            float U0Hz_B;
            float U0Hz_C;
            float I0Hz_A;
            float I0Hz_B;
            float I0Hz_C;
            float thdu_A;
            float thdu_B;
            float thdu_C;
            float U_phase_A;
            float U_phase_B;
            float U_phase_C;
        }net_params;
        struct {
            struct {
                std::vector<float> calibration;
                std::vector<float> offset;
            }calibrations;
            struct {
                float Ud_min;
                float Ud_max;
                float temperature;
                float U_min;
                float U_max;
                float Fnet_min;
                float Fnet_max;
                float I_max_rms;
                float I_max_peak;
            }protection;
            struct {
                float ctrlUd_Kp;
                float ctrlUd_Ki;
                float ctrlUd_Kd;
                float Ud_nominal;
                float Ud_precharge;
            }capacitors;
            struct{
                float K_I;
                float K_U;
                float K_Ud;
            }filters;
        }settings;
        uint32_t status;
        uint32_t active_channels[PFCconfig::PFC_NCHAN];

        PFCsettings(void)
        {
            memset(&adc, 0, sizeof(adc));
            memset(&adc_raw, 0, sizeof(adc_raw));
            memset(&net_params, 0, sizeof(net_params));
            memset(&settings.protection, 0, sizeof(settings.protection));
            memset(&settings.capacitors, 0, sizeof(settings.capacitors));
            memset(&settings.filters, 0, sizeof(settings.filters));
            status = 0;
            memset(&active_channels, 0, sizeof(active_channels));
        }
    };
}

#pragma pack(pop)
#endif // DEVICE_DEFINITION_H
