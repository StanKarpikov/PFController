/**
 * @file pfc_logic.h
 * @author Stanislav Karpikov
 * @brief State management and PFC control functions
 */

/** @addtogroup app_pfc_logic
 * @{
 */

/*--------------------------------------------------------------
                       INCLUDES
--------------------------------------------------------------*/

#include "pfc_logic.h"

#include "BSP/bsp.h"
#include "BSP/gpio.h"
#include "BSP/timer.h"
#include "adc_logic.h"
#include "command_processor.h"
#include "defines.h"
#include "events.h"
#include "math.h"
#include "settings.h"

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

static pfc_state_t last_status = PFC_STATE_INIT; /**< The last state of the PFC */
static uint32_t period_counter = 0;              /**< The number of periods since start */
static uint8_t main_started = 0;                 /**< The main connector is switched on */
static uint8_t preload_started = 0;              /**< The preload connector is switched on */
static uint32_t main_start_period = 0;           /**< The period count when the main connector was switched on */

static uint8_t pwm_on = 0; /**< PWM is switched on */

static pfc_state_t current_state = PFC_STATE_INIT; /**< The current state of the PFC */

/*--------------------------------------------------------------
                       PRIVATE TYPES
--------------------------------------------------------------*/

typedef void (*FSM_PROCESS_CALLBACK)(void); /**< State machine state callback */

/** State table array */
static FSM_PROCESS_CALLBACK state_table[PFC_STATE_COUNT] = {
    pfc_init_process,              /**< Initial state */
    pfc_stop_process,              /**< Stop state (power hardware is switched off) */
    pfc_sync_process,              /**< Syncronisation with the network */
    pfc_precharge_prepare_process, /**< Prepare precharge */
    pfc_precharge_process,         /**< Precharge (connector is in on state) */
    pfc_main_process,              /**< Main state. Precharge is finished */
    pfc_precharge_disable_process, /**< Precharge is switching off */
    pfc_work_process,              /**< Ready state */
    pfc_charge_process,            /**< Main capacitors charge is ongoing */
    pfc_test_process,              /**< Test state */
    pfc_stopping_process,          /**< Stoping work: disable sensitive and power peripheral */
    pfc_faultblock_process,        /**< Fault state */
};

/*--------------------------------------------------------------
                       PRIVATE FUNCTIONS
--------------------------------------------------------------*/

/**
 * @brief Switch on the PFC
 */
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

/**
 * @brief Switch off the PFC
 */
static void pfc_switch_off(void)
{
    pfc_set_state(PFC_STATE_STOPPING);
}

/**
 * @brief Disable PWM channels
 */
static void pfc_disable_pwm(void)
{
    int i;
    for (i = 0; i < PFC_NCHAN; i++)
    {
        //epwm_disable(i);
    }

    timer_disable_pwm();

    pwm_on = 0;
}

/**
 * @brief Re-enable PWM channels after switching off
 */
static void pfc_restore_pwm(void)
{
    //et_1=0;//Last Ucap error value
    //It_1=0;//Last Ucap integral accumulator value

    if (!pwm_on)
    {
        adc_clear_accumulators();
        timer_restore_pwm();

        pwm_on = 1;
    }
}

/**
 * @brief Returns the readiness of the voltage on capacitors
 *
 * @return true if the voltage is ready, false if not ready
 */
static bool is_voltage_ready(void)
{
    settings_capacitors_t capacitors = settings_get_capacitors();
    if (adc_get_cap_voltage() > capacitors.Ucap_precharge)
    {
        return true;
    }
    else
    {
        return false;
    }
}

/**
 * @brief Process the state callback: init
 */
static void pfc_init_process(void)
{
    gpio_main_relay_switch_off();
    gpio_preload_relay_switch_off();
    gpio_ventilators_switch_off();

    main_started = 0;
    preload_started = 0;

    pfc_disable_pwm();

    /* Wait for transition processes to end */
    if (period_counter > STARTUP_STABILISATION_TIME)
    {
        pfc_set_state(PFC_STATE_STOP);
    }
}

/**
 * @brief Process the state callback: stop
 */
static void pfc_stop_process(void)
{
    gpio_main_relay_switch_off();
    gpio_preload_relay_switch_off();
    gpio_ventilators_switch_off();

    main_started = 0;
    preload_started = 0;

    pfc_disable_pwm();

    /* TODO: Run PFC autotically. Currently the device wait for a command from the panel */
}

/**
 * @brief Process the state callback: synd
 */
static void pfc_sync_process(void)
{
    pfc_disable_pwm();
    complex_amp_t U_50Hz[PFC_NCHAN] = {0};
    float period_delta = 0;
    adc_get_complex_phase(U_50Hz, &period_delta);
    /* Wait for a phase stabilisation */
    /* TODO: Check for float comparison */
    if (period_delta == 0 && fabs(U_50Hz[PFC_ACHAN].phase) < SYNC_MINIMUM_PHASE)
    {
        pfc_set_state(PFC_STATE_PRECHARGE_PREPARE);
    }
}

/**
 * @brief Process the state callback: precharge prepare
 */
static void pfc_precharge_prepare_process(void)
{
    pfc_disable_pwm();
    preload_started = 0;
    pfc_set_state(PFC_STATE_PRECHARGE);
}

/**
 * @brief Process the state callback: precharge process
 */
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
        gpio_preload_relay_switch_on();
        //events_preload_start();
        preload_started = 1;
    }
}

/**
 * @brief Process the state callback: main
 */
static void pfc_main_process(void)
{
    pfc_disable_pwm();
    if (!main_started)
    {
        gpio_main_relay_switch_on();
        gpio_ventilators_switch_on();
        main_started = 1;
        main_start_period = period_counter;
    }
    uint32_t period_delta = period_counter - main_start_period;

    /* Wait for transition processes to end */
    if (period_delta > PRELOAD_STABILISATION_TIME)
    {
        pfc_set_state(PFC_STATE_PRECHARGE_DISABLE);
    }
}

/**
 * @brief Process the state callback: precharge disable
 */
static void pfc_precharge_disable_process(void)
{
    gpio_preload_relay_switch_off();
    preload_started = 0;
    pfc_set_state(PFC_STATE_WORK);
}

/**
 * @brief Process the state callback: work
 */
static void pfc_work_process(void)
{
    pfc_disable_pwm();
}

/**
 * @brief Process the state callback: charge
 */
static void pfc_charge_process(void)
{
    static uint8_t pulse = 0;
    pfc_restore_pwm();

    pulse++;
    if (pulse >= 4)
    {
        pulse = 0;
        pfc_set_state(PFC_STATE_WORK);
    }

    /*
	TODO: Check the charge
	if (adc_get_cap_voltage() < (PFC.settings.capacitors.Ucap_precharge*0.8)){//TODO:
		events_new_event(
				EVENT_TYPE_PROTECTION,
				SUB_EVENT_TYPE_PROTECTION_UCAP_MIN,
				0,
				adc_get_cap_voltage()
			);
	}
	*/
}

/**
 * @brief Process the state callback: test
 */
static void pfc_test_process(void)
{
    /* NOTE: Test state can be added */
}

/**
 * @brief Process the state callback: stopping
 */
static void pfc_stopping_process(void)
{
    gpio_main_relay_switch_off();
    gpio_preload_relay_switch_off();
    gpio_ventilators_switch_off();
    //events_preload_stop();
    pfc_set_state(PFC_STATE_STOP);
}

/**
 * @brief Process the state callback: faultblock
 */
static void pfc_faultblock_process(void)
{
    gpio_main_relay_switch_off();
    gpio_preload_relay_switch_off();
    gpio_ventilators_switch_off();
    pfc_disable_pwm();
}

/**
 * @brief Set state of the PFC
 *
 * @param state A new state of the PFC
 */
static void pfc_set_state(pfc_state_t state)
{
    current_state = state;
}

/*--------------------------------------------------------------
                       PUBLIC FUNCTIONS
--------------------------------------------------------------*/

/*
 * @brief Block the PFC in case of a fault event
 */
void pfc_faultblock(void)
{
    pfc_set_state(PFC_STATE_FAULTBLOCK);
}

/*
 * @brief Execute a command from the panel
 *
 * @param command The command to execute
 * @param data Command data
 * 
 * @return Status of the operation
 */
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
                settings_save();
            }
            break;
        case COMMAND_CHANNEL0_DATA:
            pwm_settings.active_channels[PFC_ACHAN] = data;
            settings_set_pwm(pwm_settings);
            break;
        case COMMAND_CHANNEL1_DATA:
            pwm_settings.active_channels[PFC_BCHAN] = data;
            settings_set_pwm(pwm_settings);
            break;
        case COMMAND_CHANNEL2_DATA:
            pwm_settings.active_channels[PFC_CCHAN] = data;
            settings_set_pwm(pwm_settings);
            break;
        default:
            return PFC_ERROR_DATA;
    }
    return PFC_SUCCESS;
}

/*
 * @brief Process the current PFC state (called in a cycle)
 */
void pfc_process(void)
{
    FSM_PROCESS_CALLBACK handler = state_table[pfc_get_state()];
    if (handler)
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

/*
 * @brief Get the current PFC state
 *
 * @return The PFC state
 */
pfc_state_t pfc_get_state(void)
{
    return current_state;
}

/*
 * @brief Get the current PWM state
 *
 * @return The PWM state
 */
uint8_t pfc_is_pwm_on(void)
{
    return pwm_on;
}
/** @} */