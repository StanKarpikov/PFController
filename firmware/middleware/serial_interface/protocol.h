#ifndef __PROTOCOL_H__
#define __PROTOCOL_H__

#include "BSP/debug.h"
#include "defines.h"

#define PROTOCOL_MAX_LENGTH (216)  // Max data - 210 + 6 service bytes
#define MIN_PACKET_LEN      (4)

#define PROTOCOL_START_BYTE ((unsigned char)0xAA)
#define PROTOCOL_STOP_BYTE  ((unsigned char)0xBB)
#define PROTOCOL_STATUS_MAX (31)  // 2**5bits - 1 = 31

#define PROTOCOL_CRC_IGNORE             (0)
#define PROTOCOL_UNKNOWN_COMMAND_IGNORE (1)

enum protocol_state
{
    P_START,
    P_STATUS,
    P_COMMAND,
    P_LEN,
    P_DATA,
    P_CRC,
    P_STOP,
};
enum protocol_mode
{
    p_mode_client,
    p_mode_adf
};
typedef union __attribute__((__packed__))
{
    struct __attribute__((__packed__))
    {
        unsigned char owner : 1;
        unsigned char error : 1;
        unsigned char crc_error : 1;
        unsigned char wrong_stop : 1;
        unsigned char parity_error : 1;
        unsigned char reserv : 3;
    } fields;
    unsigned char raw;
} STATUS_BYTE;
typedef union __attribute__((__packed__))
{
    struct __attribute__((__packed__))
    {
        unsigned char start;
        STATUS_BYTE status;
        unsigned char len;
        unsigned char command;
    } fields;
    unsigned char data[PROTOCOL_MAX_LENGTH];
} PACKAGE;
typedef void (*prot_handler)(void *);
typedef union
{
    struct _protocol_status
    {
        unsigned char queryBufOver : 1;
        unsigned char waitForResponce : 1;
        unsigned char responceTimeout : 1;
        unsigned char sendInProgress : 1;
        unsigned char connectionOk : 1;
        unsigned char reserv : 3;
    } fields;
    unsigned char raw;
} PROTOCOL_STATUS;
#define RX_BUFFER_SIZE (0x3FF)
#define TX_BUFFER_SIZE (0x3FF)

typedef struct _SciPort
{
    unsigned char tx_buffer[TX_BUFFER_SIZE];
    unsigned char tx_index;
    unsigned char tx_end;

    uint16_t rx_buffer[RX_BUFFER_SIZE];
    int rx_index;               // Позиция для следующей записи
    int rx_readed;              // Позиция для следующего чтения
    unsigned char rx_overflow;  // 0 - normal, 1 - rx buffer overflow
} SciPort;

typedef struct _protocol_context
{
    enum protocol_mode mode;

    enum protocol_state state;
    PACKAGE receivedPackage;
    PACKAGE packageToSend;

    prot_handler *handlers;
    unsigned char handlersLen;

    unsigned char size;
    unsigned char *pdata;

    PROTOCOL_STATUS status;

    SciPort *port;
} protocol_context_t;
//==================  COMMANDS  ============================

struct __attribute__((__packed__)) s_command_get_adc_active
{
    uint8_t null;
};
struct __attribute__((__packed__)) s_answer_get_adc_active
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

struct __attribute__((__packed__)) s_command_get_adc_active_raw
{
    uint8_t null;
};

struct __attribute__((__packed__)) s_answer_get_adc_active_raw
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
struct __attribute__((__packed__)) s_command_switch_on_off
{
    uint8_t command;
    uint32_t data;
};
struct __attribute__((__packed__)) s_answer_switch_on_off
{
    uint8_t result;
};
struct __attribute__((__packed__)) s_command_get_work_state
{
    uint64_t currentTime;
};
struct __attribute__((__packed__)) s_answer_get_work_state
{
    uint8_t state;
    uint32_t activeChannels[3];
};

struct __attribute__((__packed__)) s_command_get_version_info
{
    uint8_t null;
};
struct __attribute__((__packed__)) s_answer_get_version_info
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
struct __attribute__((__packed__)) s_command_get_oscillog
{
    uint8_t num;
};
#define OSCILLOG_TRANSFER_SIZE 128
struct __attribute__((__packed__)) s_answer_get_oscillog
{
    uint8_t ch;
    float max;
    float min;
    uint16_t len;
    uint8_t data[OSCILLOG_TRANSFER_SIZE];
};
struct __attribute__((__packed__)) s_command_get_settings_calibrations
{
    uint8_t null;
};
struct __attribute__((__packed__)) s_answer_get_settings_calibrations
{
    float calibration[ADC_CHANNEL_NUMBER];
    float offset[ADC_CHANNEL_NUMBER];
};
struct __attribute__((__packed__)) s_command_get_settings_protection
{
    uint8_t null;
};
struct __attribute__((__packed__)) s_answer_get_settings_protection
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
struct __attribute__((__packed__)) s_command_get_settings_capacitors
{
    uint8_t null;
};
struct __attribute__((__packed__)) s_answer_get_settings_capacitors
{
    float ctrlUd_Kp;
    float ctrlUd_Ki;
    float ctrlUd_Kd;
    float Ud_nominal;
    float Ud_precharge;
};
struct __attribute__((__packed__)) s_command_get_settings_filters
{
    uint8_t null;
};
struct __attribute__((__packed__)) s_answer_get_settings_filters
{
    float K_I;
    float K_U;
    float K_UD;
};
struct __attribute__((__packed__)) s_command_get_net_params
{
    uint8_t null;
};
struct __attribute__((__packed__)) s_answer_get_net_params
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
#include "events.h"
#define MAX_NUM_TRANSFERED_EVENTS (120 / (sizeof(struct sEventRecord)))  //not more than 120 bytes
struct __attribute__((__packed__)) s_command_get_events
{
    uint64_t afterIndex;
};
struct __attribute__((__packed__)) s_answer_get_events
{
    uint16_t num;
    struct sEventRecord events[MAX_NUM_TRANSFERED_EVENTS];
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
//=============  END COMMANDS  ============================
int protocol_work(protocol_context_t *pc);
void protocol_unknown_command_handle(protocol_context_t *pc);
void protocol_init(protocol_context_t *pc,
                   enum protocol_mode mode,
                   prot_handler *handlers,
                   unsigned char handlersLen,
                   SciPort *_port);
void protocol_send_package(protocol_context_t *pc);
void protocol_error_handle(protocol_context_t *pc, unsigned char command);
status_t protocol_hw_init(void);

#define package_get_crc(pac)           ((pac)->data[(pac)->fields.len] | ((pac)->data[(pac)->fields.len + 1] << 8))
#define package_calculate_crc(pac)     crc16((pac)->data + 1, (pac)->fields.len - 1)
#define package_clear_status(pac)      (pac)->fields.status.raw = 0
#define package_set_data_len(pac, l)   (pac)->fields.len = MIN_PACKET_LEN + l
#define package_get_command(pac)       (pac)->fields.command
#define package_set_error(pac, err)    (pac)->fields.status.fields.error = err
#define package_set_command(pac, comm) (pac)->fields.command = comm

#endif /* __PROTOCOL_H__ */
