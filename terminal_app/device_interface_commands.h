/**
 * @file device_interface_commands.h
 * @author Stanislav Karpikov
 * @brief Defines the commands format for the interface
 *
 * @note The file matches "firmware\middleware\serial_interface\protocol.h"
 */

#ifndef DEVICE_INTERFACE_COMMANDS_H
#define DEVICE_INTERFACE_COMMANDS_H

/*--------------------------------------------------------------
                       INCLUDES
--------------------------------------------------------------*/

#include <stdint.h>
#include "device_definition.h"

/*--------------------------------------------------------------
                       DEFINES
--------------------------------------------------------------*/

#pragma pack(1) /**< Enable packing to work with protocol structures */
#define _PACKED /**< Packed indentifier is none to provide capability with the firmware code */

/*--------------------------------------------------------------
                       NAMESPACES
--------------------------------------------------------------*/

using namespace PFCconfig;
using namespace PFCconfig::ADC;
using namespace PFCconfig::Interface;

/*--------------------------------------------------------------
                  PUBLIC TYPES::COMMANDS
--------------------------------------------------------------*/

/** Command: get active ADC data */
struct _PACKED s_command_get_adc_active
{
    uint8_t null;
};

/** Answer: get active ADC data */
struct _PACKED s_answer_get_adc_active
{
    float ADC_UCAP;     //CH10
    float ADC_U_A;      //CH11
    float ADC_U_B;      //CH12
    float ADC_U_C;      //CH13
    float ADC_I_A;      //CH0
    float ADC_I_B;      //CH1
    float ADC_I_C;      //CH2
    float ADC_I_ET;     //CH3
    float ADC_I_TEMP1;  //CH5
    float ADC_I_TEMP2;  //CH6
    float ADC_EDC_A;    //CH14
    float ADC_EDC_B;    //CH15
    float ADC_EDC_C;    //CH8
    float ADC_EDC_I;    //CH9

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
    uint16_t ADC_UCAP;     //CH10
    uint16_t ADC_U_A;      //CH11
    uint16_t ADC_U_B;      //CH12
    uint16_t ADC_U_C;      //CH13
    uint16_t ADC_I_A;      //CH0
    uint16_t ADC_I_B;      //CH1
    uint16_t ADC_I_C;      //CH2
    uint16_t ADC_I_ET;     //CH3
    uint16_t ADC_I_TEMP1;  //CH5
    uint16_t ADC_I_TEMP2;  //CH6
    uint16_t ADC_EDC_A;    //CH14
    uint16_t ADC_EDC_B;    //CH15
    uint16_t ADC_EDC_C;    //CH8
    uint16_t ADC_EDC_I;    //CH9
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
    uint32_t active_channels[PFC_NCHAN];
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
    float Ucap_min;
    float Ucap_max;
    float temperature;
    float U_min;
    float U_max;
    float F_min;
    float F_max;
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
    float ctrl_Ucap_Kp;
    float ctrl_Ucap_Ki;
    float ctrl_Ucap_Kd;
    float Ucap_nominal;
    float Ucap_precharge;
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
    float K_Ucap;
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

/* Reverse commands definitions */
#define s_command_set_settings_calibrations s_answer_get_settings_calibrations
#define s_answer_set_settings_calibrations  s_command_get_settings_calibrations
#define s_command_set_settings_protection   s_answer_get_settings_protection
#define s_answer_set_settings_protection    s_command_get_settings_protection
#define s_command_set_settings_capacitors   s_answer_get_settings_capacitors
#define s_answer_set_settings_capacitors    s_command_get_settings_capacitors
#define s_command_set_settings_filters      s_answer_get_settings_filters
#define s_answer_set_settings_filters       s_command_get_settings_filters

#endif // DEVICE_INTERFACE_COMMANDS_H
