/**
 * @file settings.h
 * @author Stanislav Karpikov
 * @brief PFC settings to store in NV memory (header)
 */

#ifndef __SETTINGS_H__
#define __SETTINGS_H__

/*--------------------------------------------------------------
                       INCLUDES
--------------------------------------------------------------*/

#include "defines.h"
#include "stdint.h"
#include "adc_logic.h"

/*--------------------------------------------------------------
                       PUBLIC TYPES
--------------------------------------------------------------*/

typedef struct
{
		float L_coefficient;
		uint32_t activeChannels[3];
}settings_pwm_t;

typedef struct
{
		float K_I;
		float K_U;
		float K_UD;
}settings_filters_t;

typedef struct
{
		float calibration[ADC_CHANNEL_NUMBER];  //!< Калибровки для каналов
		float offset[ADC_CHANNEL_NUMBER];       //!< Смещения для каналов
}settings_calibrations_t;

typedef struct
{
		/* Protection */
		float Ud_min;       //!< Граничные значения для Ud
		float Ud_max;       //!< Граничные значения для Ud
		float temperature;  //!< Граничные значения для Температуры
		float U_min;        //!< Граничные значения для напряжения
		float U_max;
		float Fnet_min;    //!< минимальная частота сети
		float Fnet_max;    //!< максимальная частота сети
		float I_max_rms;   //!< Максимальное граничное значение тока по RMS
		float I_max_peak;  //!< Максимальное граничное мгновенное значение тока
} settings_protection_t;

typedef struct
{
		/* Charge */
		float ctrlUd_Kp;
		float ctrlUd_Ki;
		float ctrlUd_Kd;
		float Ud_nominal;
		float Ud_precharge;  //Precharge level to start PWM
} settings_capacitors_t;
		
typedef struct
{
    settings_calibrations_t CALIBRATIONS;
    settings_filters_t FILTERS;
    settings_pwm_t PWM;
    settings_protection_t PROTECTION;
    settings_capacitors_t CAPACITORS;
    uint16_t magic;
} SETTINGS;

/*--------------------------------------------------------------
                       PUBLIC FUNCTIONS
--------------------------------------------------------------*/

settings_pwm_t settings_get_pwm(void);
settings_filters_t settings_get_filters(void);
settings_calibrations_t settings_get_calibrations(void);
settings_protection_t settings_get_protection(void);
settings_capacitors_t settings_get_capacitors(void);

status_t settings_set_pwm(settings_pwm_t pwm);
status_t settings_set_filters(settings_filters_t filters);
status_t settings_set_calibrations(settings_calibrations_t calibrations);
status_t settings_set_protection(settings_protection_t protection);
status_t settings_set_capacitors(settings_capacitors_t capacitors);

int SaveSettings(SETTINGS *settings);
int ReadSettings(SETTINGS *settings);

#endif
