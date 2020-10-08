/**
 * @file protocol.h
 * @author Stanislav Karpikov
 * @brief Process received messages from the panel
 */
 
#ifndef _PROTOCOL_H
#define _PROTOCOL_H

/*--------------------------------------------------------------
                       INCLUDES
--------------------------------------------------------------*/

#include "BSP/debug.h"
#include "defines.h"
#include "adc_logic.h"
#include "events.h"

/*--------------------------------------------------------------
                       DEFINES
--------------------------------------------------------------*/

/** The packet attribute definition for structs (removes redundant gaps) */
#define _PACKED __attribute__((__packed__))

#define MINIMUM_PACKET_LENGTH (4) /**< The minimum length of a packet */
#define MAXIMUM_PACKET_LENGTH (216)  /**< The maximum length of a packet: Max data - 210 + 6 service bytes */
#define OSCILLOG_TRANSFER_SIZE (128) /**< The size of an oscillogram */

#define MAX_EVENTS_PACKET_SIZE (150)  /**< The maximum size of the packet that can be occupied by events */

/** The maximum number of events that can be transferred */
#define MAX_NUM_TRANSFERED_EVENTS (MAX_EVENTS_PACKET_SIZE / (sizeof(struct event_record_s)))  
	
/*--------------------------------------------------------------
                       PUBLIC TYPES
--------------------------------------------------------------*/

/**
 * @brief The command callback for protocol commands
 *
 * @param pc The pointer to the protocol context
 */
typedef void (*PFC_COMMAND_CALLBACK)(void *pc);

/** Protocol stage (accordingly to the structure) */
typedef enum 
{
    PROTOCOL_START,
    PROTOCOL_STATUS,
    PROTOCOL_COMMAND,
    PROTOCOL_LEN,
    PROTOCOL_DATA,
    PROTOCOL_CRC,
    PROTOCOL_STOP,
}protocol_stage_t;

/** Protocol status byte structure */
typedef union
{
    struct
    {
        uint8_t reserved1 : 1;
        uint8_t error : 1;
        uint8_t crc_error : 1;
        uint8_t unexpected_stop : 1;
        uint8_t parity_error : 1;
        uint8_t reserved2 : 3;
    } fields;
    uint8_t raw;
} status_byte_t;

/** The packet structure */
typedef union
{
    struct
    {
        uint8_t start;
        status_byte_t status;
        uint8_t len;
        uint8_t command;
    } fields;
    uint8_t data[MAXIMUM_PACKET_LENGTH];
} packet_t;

/** The protocol context */
typedef struct
{
	  PFC_COMMAND_CALLBACK *handlers;
    uint8_t handlers_count;
	
    protocol_stage_t stage;
	
    packet_t packet_received;
    packet_t packet_to_send;

    uint8_t *data_pointer;
	  uint8_t size;
} protocol_context_t;

/*--------------------------------------------------------------
										PUBLIC TYPES:: COMMANDS
--------------------------------------------------------------*/

/** Command: get active ADC data */
struct _PACKED s_command_get_adc_active
{
    uint8_t null;
};

/** Answer: get active ADC data */
struct _PACKED s_answer_get_adc_active
{
    float ADC_UD;       //CH10
    float ADC_U_A;      //CH11
    float ADC_U_B;      //CH12
    float ADC_U_C;      //CH13
    float ADC_I_A;      //CH0
    float ADC_I_B;      //CH1
    float ADC_I_C;      //CH2
    float ADC_I_ET;     //CH3
    float ADC_I_TEMP1;  //CH5
    float ADC_I_TEMP2;  //CH6
    float ADC_EMS_A;    //CH14
    float ADC_EMS_B;    //CH15
    float ADC_EMS_C;    //CH8
    float ADC_EMS_I;    //CH9

    float ADC_MATH_A;
    float ADC_MATH_B;
    float ADC_MATH_C;
};

/** Command: get active ADC data in raw format */
struct _PACKED s_command_get_adc_active_raw
{
    uint8_t null;
};

/** Answer: get active ADC data in raw format */
struct _PACKED s_answer_get_adc_active_raw
{
    uint16_t ADC_UD;       //CH10
    uint16_t ADC_U_A;      //CH11
    uint16_t ADC_U_B;      //CH12
    uint16_t ADC_U_C;      //CH13
    uint16_t ADC_I_A;      //CH0
    uint16_t ADC_I_B;      //CH1
    uint16_t ADC_I_C;      //CH2
    uint16_t ADC_I_ET;     //CH3
    uint16_t ADC_I_TEMP1;  //CH5
    uint16_t ADC_I_TEMP2;  //CH6
    uint16_t ADC_EMS_A;    //CH14
    uint16_t ADC_EMS_B;    //CH15
    uint16_t ADC_EMS_C;    //CH8
    uint16_t ADC_EMS_I;    //CH9
};

/** Command: Switch ON/OFF control */
struct _PACKED s_command_switch_on_off
{
    uint8_t command;
    uint32_t data;
};

/** Answer: Switch ON/OFF control */
struct _PACKED s_answer_switch_on_off
{
    uint8_t result;
};

/** Command: Get the work state */
struct _PACKED s_command_get_work_state
{
    uint64_t currentTime;
};

/** Answer: Get the work state */
struct _PACKED s_answer_get_work_state
{
    uint8_t state;
    uint32_t activeChannels[PFC_NCHAN];
};

/** Command: Get the version info */
struct _PACKED s_command_get_version_info
{
    uint8_t null;
};

/** Answer: Get the version info */
struct _PACKED s_answer_get_version_info
{
    uint16_t major;
    uint16_t minor;
    uint16_t micro;
    uint16_t build;

    uint8_t day;
    uint8_t month;
    uint16_t year;
    uint8_t hour;
    uint8_t minute;
    uint8_t second;
};

/** Command: Get oscillograms */
struct _PACKED s_command_get_oscillog
{
    uint8_t num;
};

/** Answer: Get oscillograms */
struct _PACKED s_answer_get_oscillog
{
    uint8_t ch;
    float max;
    float min;
    uint16_t len;
    uint8_t data[OSCILLOG_TRANSFER_SIZE];
};

/** Command: Get calibration settings */
struct _PACKED s_command_get_settings_calibrations
{
    uint8_t null;
};

/** Answer: Get calibration settings */
struct _PACKED s_answer_get_settings_calibrations
{
    float calibration[ADC_CHANNEL_NUMBER];
    float offset[ADC_CHANNEL_NUMBER];
};

/** Command: Get protection settings */
struct _PACKED s_command_get_settings_protection
{
    uint8_t null;
};

/** Answer: Get protection settings */
struct _PACKED s_answer_get_settings_protection
{
    float Ud_min;
    float Ud_max;
    float temperature;
    float U_min;
    float U_max;
    float Fnet_min;
    float Fnet_max;
    float I_max_rms;
    float I_max_peak;
};

/** Command: Get capacitor settings */
struct _PACKED s_command_get_settings_capacitors
{
    uint8_t null;
};

/** Answer: Get capacitor settings */
struct _PACKED s_answer_get_settings_capacitors
{
    float ctrlUd_Kp;
    float ctrlUd_Ki;
    float ctrlUd_Kd;
    float Ud_nominal;
    float Ud_precharge;
};

/** Command: Get filter settings */
struct _PACKED s_command_get_settings_filters
{
    uint8_t null;
};

/** Answer: Get filter settings */
struct _PACKED s_answer_get_settings_filters
{
    float K_I;
    float K_U;
    float K_UD;
};

/** Command: Get network settings */
struct _PACKED s_command_get_net_params
{
    uint8_t null;
};

/** Answer: Get network settings */
struct _PACKED s_answer_get_net_params
{
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
};

/** Command: Get events */
struct _PACKED s_command_get_events
{
    uint64_t after_index;
};

/** Answer: Get events */
struct _PACKED s_answer_get_events
{
    uint16_t num;
    struct event_record_s events[MAX_NUM_TRANSFERED_EVENTS];
};

/** Event types: subevents for power control */
enum
{
    SUB_EVENT_TYPE_POWER_ON
};

/** Event types: subevents for protection */
enum
{
    SUB_EVENT_TYPE_PROTECTION_UD_MIN,
    SUB_EVENT_TYPE_PROTECTION_UD_MAX,
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

/* Reverse commands definitions */
#define s_command_set_settings_calibrations s_answer_get_settings_calibrations
#define s_answer_set_settings_calibrations  s_command_get_settings_calibrations
#define s_command_set_settings_protection   s_answer_get_settings_protection
#define s_answer_set_settings_protection    s_command_get_settings_protection
#define s_command_set_settings_capacitors   s_answer_get_settings_capacitors
#define s_answer_set_settings_capacitors    s_command_get_settings_capacitors
#define s_command_set_settings_filters      s_answer_get_settings_filters
#define s_answer_set_settings_filters       s_command_get_settings_filters

/*--------------------------------------------------------------
                       PUBLIC FUNCTIONTS
--------------------------------------------------------------*/

/**
 * @brief Process messages from the panel. Called in a cycle
 * 
 * @return The status of the operation
 */
status_t protocol_work(void);

/**
 * @brief Unknown command handler
 * 
 * @param pc The protocol context structure
 * 
 * @return The status of the operation
 */
void protocol_unknown_command_handle(protocol_context_t *pc);

/**
 * @brief Initialize potocol interface with handlers
 * 
 * @param handlers Pointer to handlers array
 * @param handlers_count Handlers array size
 * 
 * @return Status of the operation
 */
status_t protocol_init(PFC_COMMAND_CALLBACK *handlers,
											 uint8_t handlers_count);
											 
/**
 * @brief Send a packet to the panel
 * 
 * @param pc Pointer to handlers array
 * 
 * @return Status of the operation
 */
status_t protocol_send_packet(protocol_context_t *pc);

/**
 * @brief Send an error packet to the panel
 * 
 * @param pc The protocol context structure
 * @param command The received command with error
 * 
 * @return The status of the operation
 */
void protocol_error_handle(protocol_context_t *pc, uint8_t command);

/**
 * @brief Initialize potocol interface
 * 
 * @return Status of the operation
 */
status_t protocol_hw_init(void);

/**
 * @brief Write data len into a packet
 *
 * @param packet A pointer to the packet structure
 * @param len The length to write
 *
 * @return The status of the operation
 */
void packet_set_data_len(packet_t *packet, uint32_t len);

/**
 * @brief Extract a command value from a packet
 *
 * @param packet A pointer to the packet structure
 *
 * @return The command value
 */
uint8_t packet_get_command(packet_t *packet);

/**
 * @brief Extract a CRC value from a packet
 *
 * @param packet A pointer to the packet structure
 *
 * @return The CRC value
 */
uint16_t packet_get_crc(packet_t *packet);

/**
 * @brief Return the CRC value for a packet
 *
 * @param packet A pointer to the packet structure
 *
 * @return The CRC value
 */
uint16_t packet_calculate_crc(packet_t *packet);

/**
 * @brief Clear the status in a packet
 *
 * @param packet A pointer to the packet structure
 */
void packet_clear_status(packet_t *packet);

/**
 * @brief Set the error field in a packet
 *
 * @param error The error value
 * @param packet A pointer to the packet structure
 */
void packet_set_error(packet_t *packet, uint8_t error);

/**
 * @brief Set the command field in a packet
 *
 * @param error The command value
 * @param packet A pointer to the packet structure
 */
void packet_set_command(packet_t *packet, uint8_t command);
									 
#endif /* _PROTOCOL_H */
