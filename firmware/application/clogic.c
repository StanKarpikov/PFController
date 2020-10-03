#include "clogic.h"

#include "BSP/bsp.h"
#include "defines.h"
#include "device.h"
#include "events.h"
#include "math.h"
#include "settings.h"
#include "BSP/timer.h"
#include "BSP/gpio.h"

uint8_t PWMon = 0;
/* Сохраняет состояние каждого канала PWM и отключает его. */
void clogic_disable_pwm(void)
{
    int i;
    for (i = 0; i < PFC_NCHAN; i++)
    {
        //epwm_disable(i);
    }

		timer_disable_pwm();
		
    PWMon = 0;
}

/* Восстанавливает состояние каналов Ш� М. */
void clogic_restore_pwm(void)
{
    int i;
    //et_1=0;//прошлое значение ошибок по Ud
    //It_1=0;//прошлое значение интегральной составляющей по Ud

    for (i = 0; i < PFC_NCHAN; i++)
    {
        //epwm_enable(i);
    }

    if (!PWMon)
    {
        VLet_1 = 0;   //прошлое значение ошибок
        VLIt_1 = 0;   //прошлое значение интегральной составляющей
                      //	Idet_1=0;//прошлое значение ошибок
                      //	IdIt_1=0;//прошлое значение интегральной составляющей
                      //	Iqet_1=0;//прошлое значение ошибок
                      //	IqIt_1=0;
        Ia_e_1 = 0;   //прошлое значение ошибок
        Ib_e_1 = 0;   //прошлое значение ошибок
        Ic_e_1 = 0;   //прошлое значение ошибок
        Ia_It_1 = 0;  //прошлое значение интегральной составляющей
        Ib_It_1 = 0;  //прошлое значение интегральной составляющей
        Ic_It_1 = 0;  //прошлое значение интегральной составляющей

				timer_restore_pwm();
        

        PWMon = 1;
    }
}

static unsigned char is_voltage_ready(void)
{
    if (UD > PFC.settings.CAPACITORS.Ud_precharge)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}


/**
 * функция работы логики контакторов.
 */

void clogic_do(void)
{
    static uint16_t last_status = 0;
    static uint32_t period_counter = 0;
    static uint8_t main_started = 0;
    static uint8_t preload_started = 0;
    static uint32_t main_start_period = 0;
    static uint32_t period_delta = 0;

    switch (PFC.status)
    {                         //действия в зависимости от текущего состояния
        case PFC_STATE_INIT:  //!< CLOGIC_INIT Первоначальное состояние. Сброс контакторов
            Relay_Main_Off();
            Relay_Preload_Off();
            ventilators_off();

            main_started = 0;
            preload_started = 0;

            clogic_disable_pwm();

            // первые несколько периодов отводим на стабилизацию системы после включения
            if (period_counter < 50)
            {
                //TODO: what shall we do in here?
            }
            else
            {
                clogic_set_state(PFC_STATE_STOP);
            }
            break;
        case PFC_STATE_STOP:  //не работает
            Relay_Main_Off();
            Relay_Preload_Off();
            ventilators_off();

            main_started = 0;
            preload_started = 0;

            clogic_disable_pwm();
            //TODO: автоматически запускать
            break;
        case PFC_STATE_SYNC:  //синхронизация с сетью
            clogic_disable_pwm();
            if (PFC.period_delta == 0 && fabs(PFC.U_50Hz[0].phase) > 0.03)
            {   //TODO: число из интерфейса
                //wait
            }
            else
            {
                clogic_set_state(PFC_STATE_PRECHARGE_PREPARE);
            }
            break;
        case PFC_STATE_PRECHARGE_PREPARE:  //!< CLOGIC_PRELOAD_PREPARE Подготовка к включению предзаряда. Выключает Ш� М
            //Relay_Preload_Off();//?
            clogic_disable_pwm();
            preload_started = 0;
            clogic_set_state(PFC_STATE_PRECHARGE);
            break;
        case PFC_STATE_PRECHARGE:  //предзаряд
            clogic_disable_pwm();
            if (is_voltage_ready())
            {
                clogic_set_state(PFC_STATE_MAIN);
                break;
            }
            if (!preload_started)
            {  //выполняется один раз
                Relay_Preload_On();
                //events_preload_start();
                preload_started = 1;
            }
            //wait
            break;
        case PFC_STATE_MAIN:  //!< CLOGIC_MAIN Включение главного контактора
            clogic_disable_pwm();
            if (!main_started)
            {  //выполняется один раз
                Relay_Main_On();
                ventilators_on();
                ventilators_on();
                main_started = 1;
                main_start_period = period_counter;
            }
            period_delta = period_counter - main_start_period;
            if (period_delta >= PRELOAD_DELAY)
            {  //Ожидание скачка напряжения при подключении основного контактора, TODO: из интерфейса
                clogic_set_state(PFC_STATE_PRECHARGE_DISABLE);
            }
            break;
        case PFC_STATE_PRECHARGE_DISABLE:  //!< CLOGIC_PRELOAD_DISABLE Отключение предзаряда
            Relay_Preload_Off();
            preload_started = 0;
            clogic_set_state(PFC_STATE_WORK);
            break;
        case PFC_STATE_WORK:  //работа без ключей
            clogic_disable_pwm();

            break;
        case PFC_STATE_CHARGE:  //заряд (накачка)
            clogic_restore_pwm();
            //wait
            //Проверка, что заряжается в нужную сторону
            /*if (UD < (PFC.settings.CAPACITORS.Ud_precharge*0.8)){//TODO:
				NEWEVENT(
						EVENT_TYPE_PROTECTION,
						SUB_EVENT_TYPE_PROTECTION_UD_MIN,
						0,
						UD
					);
			}*/
            break;
        case PFC_STATE_TEST:  //тестирование сети
            break;
        case PFC_STATE_STOPPING:  //!< CLOGIC_STOPPING Выключение контакторов
            Relay_Main_Off();
            Relay_Preload_Off();
            ventilators_off();
            //events_preload_stop();
            clogic_set_state(PFC_STATE_STOP);
            break;
        case PFC_STATE_FAULTBLOCK:  //ошибка
            Relay_Main_Off();
            Relay_Preload_Off();
            ventilators_off();
            clogic_disable_pwm();
            //wait
            break;
    }
    if (PFC.status != last_status)
    {
        NEWEVENT(EVENT_TYPE_CHANGESTATE, PFC.status, 0, 0);
    }
    last_status = PFC.status;
    period_counter++;
}

/** Возвращает текущее состояние */
uint32_t clogic_state(void)
{
    return PFC.status;
}

/** Устанавливает состояние */
void clogic_set_state(PFC_STATUS state)
{
    PFC.status = state;
}
