/**
 * @file command_processor.c
 * @author Stanislav Karpikov
 * @brief Analyse commands from the remote panel, handle the interface
 */


#ifndef _COMMAND_PROCESSOR_H
#define _COMMAND_PROCESSOR_H

/*--------------------------------------------------------------
                       INCLUDE
--------------------------------------------------------------*/

#include "defines.h"
#include "protocol.h"
#include "settings.h"

/*--------------------------------------------------------------
                       PUBLIC TYPES
--------------------------------------------------------------*/

typedef enum
{
    PFC_COMMAND_TEST,                //тест соединения
    PFC_COMMAND_SWITCH_ON_OFF,       //включение\выключение фильтра
    PFC_COMMAND_GET_ADC_ACTIVE,      //получение действуюих значений
    PFC_COMMAND_GET_ADC_ACTIVE_RAW,  // получение сырых действующих значений (без калибровки)
    PFC_COMMAND_GET_OSCILLOG,         //получение осцилограммы
    PFC_COMMAND_GET_NET_PARAMS,

    PFC_COMMAND_SET_SETTINGS_CALIBRATIONS,  //Установить настройки
    PFC_COMMAND_SET_SETTINGS_PROTECTION,    //Установить настройки
    PFC_COMMAND_SET_SETTINGS_CAPACITORS,    //Установить настройки
    PFC_COMMAND_SET_SETTINGS_FILTERS,

    PFC_COMMAND_GET_SETTINGS_CALIBRATIONS,  //Установить настройки
    PFC_COMMAND_GET_SETTINGS_PROTECTION,    //Установить настройки
    PFC_COMMAND_GET_SETTINGS_CAPACITORS,    //Установить настройки
    PFC_COMMAND_GET_SETTINGS_FILTERS,

    PFC_COMMAND_GET_WORK_STATE,

    PFC_COMMAND_GET_VERSION_INFO,  // Получить информацию о версии прошивки
    PFC_COMMAND_GET_EVENTS,

    PFC_COMMAND_COUNT
}pfc_interface_commands_t;

/*--------------------------------------------------------------
                       PUBLIC FUNCTIONS
--------------------------------------------------------------*/

void prothandlers_init(SciPort *_port);

#endif  //_COMMAND_PROCESSOR_H
