#include "defines.h"
#include "settings.h"
#include "device.h"
#include "clogic.h"
#include "math.h"
#include "main.h"
#include "events.h"

extern TIM_HandleTypeDef htim1;
uint8_t PWMon=0;
/* Сохраняет состояние каждого канала PWM и отключает его. */
void clogic_disable_pwm(void) {
	int i;
	for (i = 0; i < KKM_NCHAN; i++) {
		//epwm_disable(i);
	}
	
	__HAL_TIM_MOE_DISABLE(&htim1);
	__HAL_TIM_DISABLE(&htim1);
	
	TIM1->CCER &=~ (TIM_CCER_CC1E);
	TIM1->CCER &=~ (TIM_CCER_CC1NE);
	TIM1->CCER &=~ (TIM_CCER_CC2E);
	TIM1->CCER &=~ (TIM_CCER_CC2NE);
	TIM1->CCER &=~ (TIM_CCER_CC3E);
	TIM1->CCER &=~ (TIM_CCER_CC3NE);
	PWMon=0;
}

/* Восстанавливает состояние каналов Ш� М. */
void clogic_restore_pwm(void) {
	int i;
	//et_1=0;//прошлое значение ошибок по Ud
	//It_1=0;//прошлое значение интегральной составляющей по Ud

	for (i = 0; i < KKM_NCHAN; i++) {
			//epwm_enable(i);
	}	

	if(!PWMon){
		VLet_1=0;//прошлое значение ошибок
		VLIt_1=0;//прошлое значение интегральной составляющей
	//	Idet_1=0;//прошлое значение ошибок
	//	IdIt_1=0;//прошлое значение интегральной составляющей
	//	Iqet_1=0;//прошлое значение ошибок
	//	IqIt_1=0;
		Ia_e_1=0;//прошлое значение ошибок
		Ib_e_1=0;//прошлое значение ошибок
		Ic_e_1=0;//прошлое значение ошибок
		Ia_It_1=0;//прошлое значение интегральной составляющей
		Ib_It_1=0;//прошлое значение интегральной составляющей
		Ic_It_1=0;//прошлое значение интегральной составляющей
			
	
		TIM1->CCER |= (TIM_CCER_CC1E);
		TIM1->CCER |= (TIM_CCER_CC1NE);
		TIM1->CCER |= (TIM_CCER_CC2E);
		TIM1->CCER |= (TIM_CCER_CC2NE);
		TIM1->CCER |= (TIM_CCER_CC3E);
		TIM1->CCER |= (TIM_CCER_CC3NE);
		
		__HAL_TIM_MOE_ENABLE(&htim1);
		__HAL_TIM_ENABLE(&htim1);
		
		PWMon=1;
	}
}

static unsigned char is_voltage_ready(void) {
	if ( UD > KKM.settings.CAPACITORS.Ud_precharge){
		return 1;
	}else{
		return 0;
	}
}
void Relay_Main_Off(void) {
  HAL_GPIO_WritePin(GPIOD, RELE_2_Pin, GPIO_PIN_RESET);
}
void Relay_Main_On(void) {
  HAL_GPIO_WritePin(GPIOD, RELE_2_Pin, GPIO_PIN_SET);
}
void Relay_Preload_Off(void) {
  HAL_GPIO_WritePin(GPIOD, RELE_1_Pin, GPIO_PIN_RESET);
}
void Relay_Preload_On(void) {
	HAL_GPIO_WritePin(GPIOD, RELE_1_Pin, GPIO_PIN_SET);
}
void ventilators_on(void) {
	//TODO:
}

void ventilators_off(void) {
	//TODO:
}

/**
 * функция работы логики контакторов.
 */

void clogic_do(void) {
	static uint16_t last_status=0;
	static uint32_t period_counter=0;
	static uint8_t  main_started = 0;
	static uint8_t  preload_started = 0;
	static uint32_t main_start_period=0;
	static uint32_t period_delta=0;
	
	switch(KKM.status){//действия в зависимости от текущего состояния
		case KKM_STATE_INIT: //!< CLOGIC_INIT Первоначальное состояние. Сброс контакторов
			Relay_Main_Off();
			Relay_Preload_Off();
			ventilators_off();

			main_started = 0;
			preload_started = 0;

			clogic_disable_pwm();

		// первые несколько периодов отводим на стабилизацию системы после включения
			if (period_counter < 50) {
				//TODO: what shall we do in here?
			}else{
				clogic_set_state(KKM_STATE_STOP);
			}
			break;
		case KKM_STATE_STOP: //не работает
			Relay_Main_Off();
			Relay_Preload_Off();
			ventilators_off();

			main_started = 0;
			preload_started = 0;

			clogic_disable_pwm();
			//TODO: автоматически запускать
			break;
		case KKM_STATE_SYNC: //синхронизация с сетью
			clogic_disable_pwm();
			if (KKM.period_delta==0 && fabs(KKM.U_50Hz[0].phase)>0.03) {//TODO: число из интерфейса
				//wait
			}else{
				clogic_set_state(KKM_STATE_PRECHARGE_PREPARE);
			}
			break;
		case KKM_STATE_PRECHARGE_PREPARE: //!< CLOGIC_PRELOAD_PREPARE Подготовка к включению предзаряда. Выключает Ш� М
			//Relay_Preload_Off();//?
			clogic_disable_pwm();
			preload_started=0;
			clogic_set_state(KKM_STATE_PRECHARGE);
			break;
		case KKM_STATE_PRECHARGE: //предзаряд
			clogic_disable_pwm();
			if (is_voltage_ready()){
				clogic_set_state(KKM_STATE_MAIN);
				break;
			}
			if (!preload_started) {//выполняется один раз
				Relay_Preload_On();
				//events_preload_start();
				preload_started = 1;
			}
			//wait
			break;
		case KKM_STATE_MAIN: //!< CLOGIC_MAIN Включение главного контактора
			clogic_disable_pwm();
			if (!main_started) {//выполняется один раз
				Relay_Main_On();
				ventilators_on();
				ventilators_on();
				main_started = 1;
				main_start_period = period_counter;
			}
			period_delta = period_counter - main_start_period;
			if (period_delta >= PRELOAD_DELAY){//Ожидание скачка напряжения при подключении основного контактора, TODO: из интерфейса
				clogic_set_state(KKM_STATE_PRECHARGE_DISABLE);
			}
			break;
		case KKM_STATE_PRECHARGE_DISABLE: //!< CLOGIC_PRELOAD_DISABLE Отключение предзаряда
			Relay_Preload_Off();
			preload_started = 0;
			clogic_set_state(KKM_STATE_WORK);
			break;
		case KKM_STATE_WORK:	//работа без ключей
			clogic_disable_pwm();

			break;
		case KKM_STATE_CHARGE: //заряд (накачка)			
			clogic_restore_pwm();
			//wait
			//Проверка, что заряжается в нужную сторону
			/*if (UD < (KKM.settings.CAPACITORS.Ud_precharge*0.8)){//TODO:
				NEWEVENT(
						EVENT_TYPE_PROTECTION,
						SUB_EVENT_TYPE_PROTECTION_UD_MIN,
						0,
						UD
					);
			}*/
			break;
		case KKM_STATE_TEST: //тестирование сети
			break;
		case KKM_STATE_STOPPING: //!< CLOGIC_STOPPING Выключение контакторов
			Relay_Main_Off();
			Relay_Preload_Off();
			ventilators_off();
			//events_preload_stop();
			clogic_set_state(KKM_STATE_STOP);
			break;
		case KKM_STATE_FAULTBLOCK: //ошибка
			Relay_Main_Off();
			Relay_Preload_Off();
			ventilators_off();
			clogic_disable_pwm();
			//wait
			break;
	}
	if(KKM.status!=last_status){
		NEWEVENT(EVENT_TYPE_CHANGESTATE,KKM.status,0,0);
	}
	last_status=KKM.status;
	period_counter++;
}

/** Возвращает текущее состояние */
uint32_t clogic_state(void) {
	return KKM.status;
}

/** Устанавливает состояние */
void clogic_set_state(KKM_STATUS state) {
		KKM.status = state;
}

