/**
 * @file pfc_logic.h
 * @author Stanislav Karpikov
 * @brief State management and PFC control functions
 */

/*--------------------------------------------------------------
                       INCLUDES
--------------------------------------------------------------*/

#include "pfc_logic.h"
#include "BSP/bsp.h"
#include "defines.h"
#include "command_processor.h"
#include "events.h"
#include "math.h"
#include "settings.h"
#include "BSP/timer.h"
#include "BSP/gpio.h"
#include "adc_logic.h"

/*--------------------------------------------------------------
                       PRIVATE TYPES
--------------------------------------------------------------*/


/*--------------------------------------------------------------
                       PRIVATE FUNCTIONS PROTOTYPES
--------------------------------------------------------------*/

static void pfc_set_state(pfc_state_t state);

static void pfc_init_process(void);
static void pfc_stop_process(void);
static void pfc_sync_process(void);
static void pfc_precharge_prepare_process(void);
static void pfc_precharge_process(void);
static void pfc_main_process(void);
static void pfc_precharge_disable_process(void);
static void pfc_work_process(void);
static void pfc_charge_process(void);
static void pfc_test_process(void);
static void pfc_stopping_process(void);
static void pfc_faultblock_process(void);

/*--------------------------------------------------------------
                       PRIVATE DATA
--------------------------------------------------------------*/

static uint16_t last_status = 0;
static uint32_t period_counter = 0;
static uint8_t main_started = 0;
static uint8_t preload_started = 0;
static uint32_t main_start_period = 0;
static uint32_t period_delta = 0;

static uint8_t PWMon = 0;

static pfc_state_t current_state = PFC_STATE_INIT;

/*--------------------------------------------------------------
                       PRIVATE TYPES
--------------------------------------------------------------*/

typedef void (*FSM_PROCESS_CALLBACK)(void); /**<  */

/** State table array */
static FSM_PROCESS_CALLBACK state_table[PFC_STATE_COUNT] = {
	pfc_init_process,               /**< Initial state */
  pfc_stop_process,               /**< Stop state (power hardware is switched off) */
  pfc_sync_process,               /**< Syncronisation with the network */
  pfc_precharge_prepare_process,  /**< Prepare precharge */
  pfc_precharge_process,          /**< Precharge (connector is in on state) */
  pfc_main_process,               /**< Main state. Precharge is finished */
  pfc_precharge_disable_process,  /**< Precharge is switching off */
  pfc_work_process,               /**< Ready state */
  pfc_charge_process,             /**< Main capacitors charge is ongoing */
  pfc_test_process,               /**< Test state */
	pfc_stopping_process,           /**< Stoping work: disable sensitive and power peripheral */
  pfc_faultblock_process,         /**< Fault state */
};

/*--------------------------------------------------------------
                       PRIVATE FUNCTIONS
--------------------------------------------------------------*/
		
static void pfc_switch_on(void)
{
    if (pfc_get_state() == PFC_STATE_STOP)
    {
        pfc_set_state(PFC_STATE_SYNC);
    }
    else if (pfc_get_state() == PFC_STATE_FAULTBLOCK)
    {
        pfc_set_state(PFC_STATE_SYNC);
    }
}

static void pfc_switch_off(void)
{
    pfc_set_state(PFC_STATE_STOPPING);
}

/* Сохраняет состояние каждого канала PWM и отключает его. */
static void pfc_disable_pwm(void)
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
static void pfc_restore_pwm(void)
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
				adc_clear_accumulators();
				timer_restore_pwm();
        

        PWMon = 1;
    }
}

static bool is_voltage_ready(void)
{
		settings_capacitors_t capacitors = settings_get_capacitors();
    if (adc_get_cap_voltage() > capacitors.Ud_precharge)
    {
        return true;
    }
    else
    {
        return false;
    }
}


static void pfc_init_process(void)
{
		Relay_Main_Off();
		Relay_Preload_Off();
		ventilators_off();

		main_started = 0;
		preload_started = 0;

		pfc_disable_pwm();

	  /* Wait for transition processes to end */
		if (period_counter > STARTUP_STABILISATION_TIME)
		{
				pfc_set_state(PFC_STATE_STOP);
		}
}

static void pfc_stop_process(void)
{
		Relay_Main_Off();
		Relay_Preload_Off();
		ventilators_off();

		main_started = 0;
		preload_started = 0;

		pfc_disable_pwm();
	
	/* TODO: Run PFC autotically. Currently the device wait for a command from the panel */
}

static void pfc_sync_process(void)
{	
	pfc_disable_pwm();
	complex_amp_t U_50Hz[PFC_NCHAN]={0};
	float period_delta = 0;
  adc_get_complex_phase(U_50Hz, &period_delta);
	/* Wait for a phase stabilisation */
	/* TODO: Check for float comparison */
	if (period_delta == 0 && fabs(U_50Hz[PFC_ACHAN].phase) < SYNC_MINIMUM_PHASE)
	{
			pfc_set_state(PFC_STATE_PRECHARGE_PREPARE);
	}
}
static void pfc_precharge_prepare_process(void)
{
	pfc_disable_pwm();
	preload_started = 0;
	pfc_set_state(PFC_STATE_PRECHARGE);
}

static void pfc_precharge_process(void)
{
	pfc_disable_pwm();
	if (is_voltage_ready())
	{
			pfc_set_state(PFC_STATE_MAIN);
			return;
	}
	if (!preload_started)
	{
			Relay_Preload_On();
			//events_preload_start();
			preload_started = 1;
	}
}
static void pfc_main_process(void)
{
	pfc_disable_pwm();
	if (!main_started)
	{
			Relay_Main_On();
			ventilators_on();
			main_started = 1;
			main_start_period = period_counter;
	}
	period_delta = period_counter - main_start_period;
	
	/* Wait for transition processes to end */
	if (period_delta > PRELOAD_STABILISATION_TIME)
	{
			pfc_set_state(PFC_STATE_PRECHARGE_DISABLE);
	}
}
static void pfc_precharge_disable_process(void)
{
	Relay_Preload_Off();
	preload_started = 0;
	pfc_set_state(PFC_STATE_WORK);
}
static void pfc_work_process(void)
{
	pfc_disable_pwm();
}
static void pfc_charge_process(void)
{
	static uint8_t pulse=0;
	pfc_restore_pwm();

	pulse++;
	if (pulse >= 4)
	{
			pulse = 0;
			pfc_set_state(PFC_STATE_WORK);
	}

	/*
	TODO: Check the charge
	if (adc_get_cap_voltage() < (PFC.settings.CAPACITORS.Ud_precharge*0.8)){//TODO:
		events_new_event(
				EVENT_TYPE_PROTECTION,
				SUB_EVENT_TYPE_PROTECTION_UD_MIN,
				0,
				adc_get_cap_voltage()
			);
	}
	*/
	
}
static void pfc_test_process(void)
{
	/* NOTE: Test state can be added */
}
static void pfc_stopping_process(void)
{
	Relay_Main_Off();
	Relay_Preload_Off();
	ventilators_off();
	//events_preload_stop();
	pfc_set_state(PFC_STATE_STOP);
}
static void pfc_faultblock_process(void)
{
	Relay_Main_Off();
	Relay_Preload_Off();
	ventilators_off();
	pfc_disable_pwm();
}

static void pfc_set_state(pfc_state_t state)
{
    current_state = state;
}
/*--------------------------------------------------------------
                       PUBLIC FUNCTIONS
--------------------------------------------------------------*/

void pfc_faultblock(void)
{
	pfc_set_state(PFC_STATE_FAULTBLOCK);
}

status_t pfc_apply_command(pfc_commands_t command, uint32_t data)
{
	settings_pwm_t pwm_settings = settings_get_pwm();
	switch (command)
	{
			case COMMAND_WORK_ON:
					if (pfc_get_state() == PFC_STATE_STOP)
					{
							pfc_switch_on();
					}
					break;
			case COMMAND_WORK_OFF:
					pfc_switch_off();
					break;
			case COMMAND_CHARGE_ON:
					if (pfc_get_state() == PFC_STATE_WORK)
					{
							pfc_set_state(PFC_STATE_CHARGE);
					}
					break;
			case COMMAND_CHARGE_OFF:
					if (pfc_get_state() == PFC_STATE_CHARGE)
					{
							pfc_set_state(PFC_STATE_WORK);
					}
					break;
			case COMMAND_SETTINGS_SAVE:
					if (pfc_get_state() == PFC_STATE_STOP)
					{
							SaveSettings();
					}
					break;
			case COMMAND_CHANNEL0_DATA:
					pwm_settings.activeChannels[PFC_ACHAN] = data;
					settings_set_pwm(pwm_settings);
					break;
			case COMMAND_CHANNEL1_DATA:
					pwm_settings.activeChannels[PFC_BCHAN] = data;
					settings_set_pwm(pwm_settings);
					break;
			case COMMAND_CHANNEL2_DATA:
					pwm_settings.activeChannels[PFC_CCHAN] = data;
					settings_set_pwm(pwm_settings);
					break;
			default:
				  return PFC_ERROR_DATA;
	}
	return PFC_SUCCESS;
}

/**
 * @brief Process the current PFC state (called in a cycle)
 */
void pfc_process(void)
{
	  FSM_PROCESS_CALLBACK handler = state_table[pfc_get_state()];
    if(handler)
    {
			handler();
		}
       
    if (pfc_get_state() != last_status)
    {
        events_new_event(EVENT_TYPE_CHANGESTATE, pfc_get_state(), 0, 0);
    }
    last_status = pfc_get_state();
    period_counter++;
}

uint8_t pfc_is_pwm_on(void)
{
	return PWMon;
}

pfc_state_t pfc_get_state(void)
{
    return current_state;
}
