/**
 * @file defines.h
 * @author Stanislav Karpikov
 * @brief Global settings and defines
 */

#ifndef __DEFINES_H__
#define __DEFINES_H__

/*---------------------  INCLUDES  -------------------------------------------*/

#include "stdint.h"

/*------------------  PUBLIC DEFINES  ----------------------------------------*/

#define PWM_PERIOD (2000)

#define KKM_NCHAN   (3)    //three-phase network
#define BUF_NUM     (2)    //Double buffering
#define ADC_VAL_NUM (128)  //Count per period

#define SYNC_PHASE_ERROR    (0.03f)
#define SYNC_PHASE_DELTAMIN (5e-4)

#define PRELOAD_DELAY 50

#define MATH_PI (3.141592653589793)

#define SQUARE_F(X) ((X) * (X))

#define IIR_1ORDER(Y, Xt, Xt_1, K, Kinv) \
    Y = (Xt) * (K) + (Xt_1) * (Kinv)

typedef struct ComplexAmpPhase
{
    float amplitude;  //!< Амплитуда
    float phase;      //!< Фаза (в радианах)
} ComplexAmpPhase;

enum
{
    ADC_UD,       //CH10
    ADC_U_A,      //CH11
    ADC_U_B,      //CH12
    ADC_U_C,      //CH13
    ADC_I_A,      //CH0
    ADC_I_B,      //CH1
    ADC_I_C,      //CH2
    ADC_I_ET,     //CH3
    ADC_I_TEMP1,  //CH5
    ADC_I_TEMP2,  //CH6
    ADC_EMS_A,    //CH14
    ADC_EMS_B,    //CH15
    ADC_EMS_C,    //CH8
    ADC_EMS_I,    //CH9
    ADC_MATH_A,
    ADC_MATH_B,
    ADC_MATH_C,
    ADC_MATH_C_A,
    ADC_MATH_C_B,
    ADC_MATH_C_C
};
#define ADC_CHANNEL_NUMBER ADC_MATH_A
#define ADC_MATH_NUMBER    6

extern uint8_t needSquare[];

enum
{
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

typedef enum
{
    KKM_STATE_INIT,               //!< CLOGIC_INIT Первоначальное состояние. Сброс контакторов.
    KKM_STATE_STOP,               //не работает
    KKM_STATE_SYNC,               //синхронизация с сетью
    KKM_STATE_PRECHARGE_PREPARE,  //!< CLOGIC_PRELOAD_PREPARE Подготовка к включению предзаряда. Выключает Ш� М
    KKM_STATE_PRECHARGE,          //предзаряд
    KKM_STATE_MAIN,               //!< CLOGIC_MAIN Включение главного контактора
    KKM_STATE_PRECHARGE_DISABLE,  //!< CLOGIC_PRELOAD_DISABLE Отключение предзаряда
    KKM_STATE_WORK,               //работа без ключей
    KKM_STATE_CHARGE,             //заряд (накачка)
    KKM_STATE_TEST,               //тестирование сети
    KKM_STATE_STOPPING,           //!< CLOGIC_STOPPING Выключение контакторов
    KKM_STATE_FAULTBLOCK          //ошибка
} KKM_STATUS;

#endif
