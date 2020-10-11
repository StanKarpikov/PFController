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

#pragma pack(push)
#pragma pack(1) /**< Enable packing to work with protocol structures */
#define _FW_PACKED /**< Packed indentifier is none to provide capability with the firmware code */

/*--------------------------------------------------------------
                  PUBLIC TYPES::COMMANDS
--------------------------------------------------------------*/

struct package_general
{};

struct command_general: public package_general
{};

struct answer_general: public package_general
{};

/** Command: get active ADC data */
struct _FW_PACKED command_get_adc_active: public command_general
{
    uint8_t null;
};

/** Answer: get active ADC data */
struct _FW_PACKED answer_get_adc_active: public answer_general
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
struct _FW_PACKED command_get_adc_active_raw: public command_general
{
    uint8_t null;
};

/** Answer: get active ADC data in raw format */
struct _FW_PACKED answer_get_adc_active_raw: public answer_general
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
struct _FW_PACKED command_switch_on_off: public command_general
{
    uint8_t command;
    uint32_t data;
};

/** Answer: Switch ON/OFF control */
struct _FW_PACKED answer_switch_on_off: public answer_general
{
    uint8_t result;
};

/** Command: Get the work state */
struct _FW_PACKED command_get_work_state: public command_general
{
    uint64_t currentTime;
};

/** Answer: Get the work state */
struct _FW_PACKED answer_get_work_state: public answer_general
{
    uint8_t state;
    uint32_t active_channels[PFCconfig::PFC_NCHAN];
};

/** Command: Get the version info */
struct _FW_PACKED command_get_version_info: public command_general
{
    uint8_t null;
};

/** Answer: Get the version info */
struct _FW_PACKED answer_get_version_info: public answer_general
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
struct _FW_PACKED command_get_oscillog: public command_general
{
    uint8_t num;
};

/** Answer: Get oscillograms */
struct _FW_PACKED answer_get_oscillog: public answer_general
{
    uint8_t ch;
    float max;
    float min;
    uint16_t len;
    uint8_t data[PFCconfig::Interface::OSCILLOG_TRANSFER_SIZE];
};

/** Command: Get calibration settings */
struct _FW_PACKED command_get_settings_calibrations: public command_general
{
    uint8_t null;
};

/** Answer: Get calibration settings */
struct _FW_PACKED answer_get_settings_calibrations: public answer_general
{
    float calibration[PFCconfig::ADC::ADC_CHANNEL_NUMBER];
    float offset[PFCconfig::ADC::ADC_CHANNEL_NUMBER];
};

/** Command: Get protection settings */
struct _FW_PACKED command_get_settings_protection: public command_general
{
    uint8_t null;
};

/** Answer: Get protection settings */
struct _FW_PACKED answer_get_settings_protection: public answer_general
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
struct _FW_PACKED command_get_settings_capacitors: public command_general
{
    uint8_t null;
};

/** Answer: Get capacitor settings */
struct _FW_PACKED answer_get_settings_capacitors: public answer_general
{
    float ctrl_Ucap_Kp;
    float ctrl_Ucap_Ki;
    float ctrl_Ucap_Kd;
    float Ucap_nominal;
    float Ucap_precharge;
};

/** Command: Get filter settings */
struct _FW_PACKED command_get_settings_filters: public command_general
{
    uint8_t null;
};

/** Answer: Get filter settings */
struct _FW_PACKED answer_get_settings_filters: public answer_general
{
    float K_I;
    float K_U;
    float K_Ucap;
};

/** Answer: Set calibration settings */
struct _FW_PACKED answer_set_settings_calibrations: public command_general
{
    uint8_t null;
};

/** Command: Set calibration settings */
struct _FW_PACKED command_set_settings_calibrations: public answer_general
{
    float calibration[PFCconfig::ADC::ADC_CHANNEL_NUMBER];
    float offset[PFCconfig::ADC::ADC_CHANNEL_NUMBER];
};

/** Answer: Set protection settings */
struct _FW_PACKED answer_set_settings_protection: public command_general
{
    uint8_t null;
};

/** Command: Set protection settings */
struct _FW_PACKED command_set_settings_protection: public answer_general
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

/** Answer: Set capacitor settings */
struct _FW_PACKED answer_set_settings_capacitors: public command_general
{
    uint8_t null;
};

/** Command: Set capacitor settings */
struct _FW_PACKED command_set_settings_capacitors: public answer_general
{
    float ctrl_Ucap_Kp;
    float ctrl_Ucap_Ki;
    float ctrl_Ucap_Kd;
    float Ucap_nominal;
    float Ucap_precharge;
};

/** Answer: Set filter settings */
struct _FW_PACKED answer_set_settings_filters: public command_general
{
    uint8_t null;
};

/** Command: Set filter settings */
struct _FW_PACKED command_set_settings_filters: public answer_general
{
    float K_I;
    float K_U;
    float K_Ucap;
};

/** Command: Get network settings */
struct _FW_PACKED command_get_net_params: public command_general
{
    uint8_t null;
};

/** Answer: Get network settings */
struct _FW_PACKED answer_get_net_params: public answer_general
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
struct _FW_PACKED command_get_events: public command_general
{
    uint64_t after_index;
};

/** Answer: Get events */
struct _FW_PACKED answer_get_events: public answer_general
{
    uint16_t num;
    PFCconfig::Events::EventRecord events[PFCconfig::Interface::MAX_NUM_TRANSFERED_EVENTS];
};

#pragma pack(pop)

#endif // DEVICE_INTERFACE_COMMANDS_H
