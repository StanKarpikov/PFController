/**
 * @file protocol.h
 * @author Stanislav Karpikov
 * @brief Process received messages from the panel
 */
 
#ifndef __PROTOCOL_H__
#define __PROTOCOL_H__

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

#define _PACKED __attribute__((__packed__))

#define MIN_PACKET_LEN      (4)
#define PROTOCOL_MAX_LENGTH (216)  /**< Max data - 210 + 6 service bytes */
#define OSCILLOG_TRANSFER_SIZE (128)

#define MAX_NUM_TRANSFERED_EVENTS (120 / (sizeof(struct event_record_s)))  /**< not more than 120 bytes */
	
/*--------------------------------------------------------------
                       PUBLIC TYPES
--------------------------------------------------------------*/

typedef void (*PFC_COMMAND_CALLBACK)(void *pc); /**<  */

typedef enum 
{
    PROTOCOL_START,
    PROTOCOL_STATUS,
    PROTOCOL_COMMAND,
    PROTOCOL_LEN,
    PROTOCOL_DATA,
    PROTOCOL_CRC,
    PROTOCOL_STOP,
}protocol_state_t;

typedef union _PACKED
{
    struct _PACKED
    {
        uint8_t reserv1 : 1;
        uint8_t error : 1;
        uint8_t crc_error : 1;
        uint8_t wrong_stop : 1;
        uint8_t parity_error : 1;
        uint8_t reserv2 : 3;
    } fields;
    uint8_t raw;
} STATUS_BYTE;

typedef union _PACKED
{
    struct _PACKED
    {
        uint8_t start;
        STATUS_BYTE status;
        uint8_t len;
        uint8_t command;
    } fields;
    uint8_t data[PROTOCOL_MAX_LENGTH];
} packet_t;

typedef union
{
    struct
    {
        uint8_t queryBufOver : 1;
        uint8_t waitForResponce : 1;
        uint8_t responceTimeout : 1;
        uint8_t sendInProgress : 1;
        uint8_t connectionOk : 1;
        uint8_t reserv : 3;
    } fields;
    uint8_t raw;
} protocol_status_t;

typedef struct _protocol_context
{
    protocol_state_t state;
    packet_t receivedPackage;
    packet_t packageToSend;

    PFC_COMMAND_CALLBACK *handlers;
    uint8_t handlers_count;

    uint8_t size;
    uint8_t *pdata;

    protocol_status_t status;
} protocol_context_t;


/*--------------------------------------------------------------
										PUBLIC TYPES:: COMMANDS
--------------------------------------------------------------*/

struct _PACKED s_command_get_adc_active
{
    uint8_t null;
};

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

struct _PACKED s_command_get_adc_active_raw
{
    uint8_t null;
};

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
struct _PACKED s_command_switch_on_off
{
    uint8_t command;
    uint32_t data;
};
struct _PACKED s_answer_switch_on_off
{
    uint8_t result;
};
struct _PACKED s_command_get_work_state
{
    uint64_t currentTime;
};
struct _PACKED s_answer_get_work_state
{
    uint8_t state;
    uint32_t activeChannels[3];
};

struct _PACKED s_command_get_version_info
{
    uint8_t null;
};
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
struct _PACKED s_command_get_oscillog
{
    uint8_t num;
};

struct _PACKED s_answer_get_oscillog
{
    uint8_t ch;
    float max;
    float min;
    uint16_t len;
    uint8_t data[OSCILLOG_TRANSFER_SIZE];
};
struct _PACKED s_command_get_settings_calibrations
{
    uint8_t null;
};
struct _PACKED s_answer_get_settings_calibrations
{
    float calibration[ADC_CHANNEL_NUMBER];
    float offset[ADC_CHANNEL_NUMBER];
};
struct _PACKED s_command_get_settings_protection
{
    uint8_t null;
};
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
struct _PACKED s_command_get_settings_capacitors
{
    uint8_t null;
};
struct _PACKED s_answer_get_settings_capacitors
{
    float ctrlUd_Kp;
    float ctrlUd_Ki;
    float ctrlUd_Kd;
    float Ud_nominal;
    float Ud_precharge;
};
struct _PACKED s_command_get_settings_filters
{
    uint8_t null;
};
struct _PACKED s_answer_get_settings_filters
{
    float K_I;
    float K_U;
    float K_UD;
};
struct _PACKED s_command_get_net_params
{
    uint8_t null;
};
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

struct _PACKED s_command_get_events
{
    uint64_t after_index;
};
struct _PACKED s_answer_get_events
{
    uint16_t num;
    struct event_record_s events[MAX_NUM_TRANSFERED_EVENTS];
};

enum
{
    SUB_EVENT_TYPE_POWER_ON
};
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

enum
{
    PROTECTION_IGNORE,
    PROTECTION_WARNING_STOP,
    PROTECTION_ERROR_STOP
};

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
status_t protocol_send_package(protocol_context_t *pc);

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
void package_set_data_len(packet_t *packet, uint32_t len);

/**
 * @brief Extract a command value from a packet
 *
 * @param packet A pointer to the packet structure
 *
 * @return The command value
 */
uint8_t package_get_command(packet_t *packet);

/**
 * @brief Extract a CRC value from a packet
 *
 * @param packet A pointer to the packet structure
 *
 * @return The CRC value
 */
uint16_t package_get_crc(packet_t *packet);

/**
 * @brief Return the CRC value for a packet
 *
 * @param packet A pointer to the packet structure
 *
 * @return The CRC value
 */
uint16_t package_calculate_crc(packet_t *packet);

/**
 * @brief Clear the status in a packet
 *
 * @param packet A pointer to the packet structure
 */
void package_clear_status(packet_t *packet);

/**
 * @brief Set the error field in a packet
 *
 * @param error The error value
 * @param packet A pointer to the packet structure
 */
void package_set_error(packet_t *packet, uint8_t error);

/**
 * @brief Set the command field in a packet
 *
 * @param error The command value
 * @param packet A pointer to the packet structure
 */
void package_set_command(packet_t *packet, uint8_t command);
									 
#endif /* __PROTOCOL_H__ */
