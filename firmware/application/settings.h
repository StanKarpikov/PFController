#ifndef __SETTINGS_H__
#define __SETTINGS_H__
#include "defines.h"
#include "stdint.h"

typedef struct _settings
{
    struct
    {
        float calibration[ADC_CHANNEL_NUMBER];  //!< Калибровки для каналов
        float offset[ADC_CHANNEL_NUMBER];       //!< Смещения для каналов
    } CALIBRATIONS;
    struct
    {
        float K_I;
        float K_U;
        float K_UD;
    } FILTERS;
    struct
    {
        float L_coefficient;
        uint32_t activeChannels[3];
    } PWM;
    struct
    {
        //=== защиты ======
        float Ud_min;       //!< Граничные значения для Ud
        float Ud_max;       //!< Граничные значения для Ud
        float temperature;  //!< Граничные значения для Температуры
        float U_min;        //!< Граничные значения для напряжения
        float U_max;
        float Fnet_min;    //!< минимальная частота сети
        float Fnet_max;    //!< максимальная частота сети
        float I_max_rms;   //!< Максимальное граничное значение тока по RMS
        float I_max_peak;  //!< Максимальное граничное мгновенное значение тока
    } PROTECTION;
    struct
    {
        //==== накачка =====
        float ctrlUd_Kp;  //!< Управление накачкой
        float ctrlUd_Ki;
        float ctrlUd_Kd;
        float Ud_nominal;
        float Ud_precharge;  //Precharge level to start PWM
    } CAPACITORS;
    uint16_t magic;
} SETTINGS;

int SaveSettings(SETTINGS *settings);
int ReadSettings(SETTINGS *settings);

#endif
