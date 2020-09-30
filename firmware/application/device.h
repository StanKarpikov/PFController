#ifndef __DEVICE_H
#define __DEVICE_H

#include "defines.h"
#include "protocol.h"
#include "settings.h"

#define UD (KKM.adc.active[ADC_UD])

extern uint8_t last_buffer;
extern uint8_t current_buffer;
extern uint16_t symbol;
extern uint8_t newPeriod;
extern float sinusoid[2000];
extern float OSC_DATA[OSC_CHANNEL_NUMBER][OSCILLOG_TRANSFER_SIZE];
extern uint16_t ADC_DMA_buf[ADC_CHANNEL_NUMBER];
extern float VLet_1;//прошлое значение ошибок
extern float VLIt_1;//прошлое значение интегральной составляющей
//extern float Idet_1;//прошлое значение ошибок
//extern float IdIt_1;//прошлое значение интегральной составляющей
//extern float Iqet_1;//прошлое значение ошибок
//extern float IqIt_1;//прошлое значение интегральной составляющей
extern float Ia_e_1;//прошлое значение ошибок
extern float Ia_It_1;//прошлое значение интегральной составляющей
extern float Ib_e_1;//прошлое значение ошибок
extern float Ib_It_1;//прошлое значение интегральной составляющей
extern float Ic_e_1;//прошлое значение ошибок
extern float Ic_It_1;//прошлое значение интегральной составляющей

struct _adc {
	float ch[BUF_NUM][ADC_CHANNEL_NUMBER+ADC_MATH_NUMBER][ADC_VAL_NUM];
	/*
	float I[BUF_NUM][KKM_NCHAN][ADC_VAL_NUM];
	float U[BUF_NUM][KKM_NCHAN][ADC_VAL_NUM];
	float Ud[BUF_NUM][ADC_VAL_NUM];
*/
	float active[ADC_CHANNEL_NUMBER+ADC_MATH_NUMBER];			//RMS or mean value with correction
	uint16_t active_raw[ADC_CHANNEL_NUMBER]; //RMS or mean value without correction

	//Temp sum for calculate adc active value
	float sum_raw_sqr[BUF_NUM][ADC_CHANNEL_NUMBER+ADC_MATH_NUMBER]; 
	float sum_raw[BUF_NUM][ADC_CHANNEL_NUMBER+ADC_MATH_NUMBER];
};
typedef struct _adc kADC;

struct _kkm {
	PROTOCOL_CONTEXT protocol; //!< Контекст работы протокола по serial порту (Связь с графическим интерфейсом).
	SETTINGS settings; //!< Указатель на настройки	
	KKM_STATUS status; //!< Текущее состояние работы	
	kADC adc;
	
	ComplexAmpPhase U_50Hz[KKM_NCHAN]; //!< Амплитуда и фаза гармоники для 50Гц напряжения (для каждой фазы)
	float period_delta;
	
	float period_fact;		
	float U_0Hz[KKM_NCHAN]; //!< Величина постоянной составляющей в напряжении (для каждой фазы)
	float I_0Hz[KKM_NCHAN]; //!< Величина постоянной составляющей тока (для каждой фазы)
	float U_phase[KKM_NCHAN];	
	float thdu[KKM_NCHAN];
	
	float temperature;
};
extern struct _kkm KKM;
//========================================================
//================== INTERFACE ===========================
//========================================================
enum P_AFG_COMMANDS {
	P_AFG_COMMAND_TEST, //тест соединения
	P_AFG_COMMAND_SWITCH_ON_OFF, //включение\выключение фильтра
	P_AFG_COMMAND_GET_ADC_ACTIVE, //получение действуюих значений
	P_AFG_COMMAND_GET_ADC_ACTIVE_RAW, // получение сырых действующих значений (без калибровки)
	P_AFG_COMMAND_GET_OSCILOG, //получение осцилограммы
  P_AFG_COMMAND_GET_NET_PARAMS,

	P_AFG_COMMAND_SET_SETTINGS_CALIBRATIONS, //Установить настройки
	P_AFG_COMMAND_SET_SETTINGS_PROTECTION, //Установить настройки
	P_AFG_COMMAND_SET_SETTINGS_CAPACITORS, //Установить настройки
	P_AFG_COMMAND_SET_SETTINGS_FILTERS,

	P_AFG_COMMAND_GET_SETTINGS_CALIBRATIONS, //Установить настройки
	P_AFG_COMMAND_GET_SETTINGS_PROTECTION, //Установить настройки
	P_AFG_COMMAND_GET_SETTINGS_CAPACITORS, //Установить настройки
	P_AFG_COMMAND_GET_SETTINGS_FILTERS,

	P_AFG_COMMAND_GET_WORK_STATE,

	P_AFG_COMMAND_GET_VERSION_INFO, // Получить информацию о версии прошивки
	P_AFG_COMMAND_GET_EVENTS,

	lenP_AFG_COMMANDS
};
void prothandlers_init(SciPort *_port);
#endif //__DEVICE_H
