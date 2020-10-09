/**
 * @file events_process.c
 * @author Stanislav Karpikov
 * @brief Process events (status, warnings, errors, faults)
 */

/*--------------------------------------------------------------
                       INCLUDE
--------------------------------------------------------------*/

#include "events_process.h"
#include "pfc_logic.h"
#include "math.h"

/*--------------------------------------------------------------
                       DEFINES
--------------------------------------------------------------*/

#undef ONLY_A_CHANNEL /**< Define to test work only on one channel */

/** A margin is used to check ADC value to be close to the boundaries (possible short-circiut or open load) */
#define ADC_RANGE_MARGIN                  (50U)

/**
 * @brief Check an ADC value to be in the working range
 *
 * @param X The value to check
 
 * @retval 0 The value is in the range
 * @retval 1 The value is outside the range
 */
#define CHECK_ADC_VALUE_RANGE(X) (((X) < ADC_RANGE_MARGIN) || ((X) > (4095U - ADC_RANGE_MARGIN)))

/*--------------------------------------------------------------
                       PRIVATE FUNCTIONS
--------------------------------------------------------------*/

#ifdef ONLY_A_CHANNEL
static inline uint16_t is_pass_adc_channel(uint16_t channel)
{
		static const uint16_t passing_adc_channels[] = {
				1,  // ADC_IL_A;
				0,  // ADC_IL_B;
				0,  // ADC_IL_C;
				1,  // ADC_IAFG_A;
				0,  // ADC_IAFG_B;
				0,  // ADC_IAFG_C;
				1,  // ADC_U_A;
				0,  // ADC_U_B;
				0,  // ADC_U_C;
				1,  // ADC_TEMP;
				1,  // ADC_UREF;
				1,  // ADC_UCAP_1;
				1   // ADC_UCAP_2;
		};
    return !(passing_adc_channels[channel]);
}
#endif

/*--------------------------------------------------------------
                       PUBLIC FUNCTIONS
--------------------------------------------------------------*/

/*
 * @brief Check ADC active values for overloading
 * 
 * @param adc_values ADC active values
 *
 * @return The status of the operation
 */
status_t events_check_adc_overload(float *adc_values)
{
		static uint16_t adc_ov_ticks[ADC_EMS_I + 1]={0};

    for (int i = 0; i < ADC_EMS_I + 1; i++)
    {
#ifdef ONLY_A_CHANNEL
        if (is_pass_adc_channel(i)) continue;
#endif
        if (CHECK_ADC_VALUE_RANGE(adc_values[i]))
        {
            adc_ov_ticks[i]++;
            if (adc_ov_ticks[i] > 3)
            {
                events_new_event(EVENT_TYPE_PROTECTION,
                         SUB_EVENT_TYPE_PROTECTION_ADC_OVERLOAD,
                         i,
                         adc_values[i]);
            }
        }
        else
        {
            adc_ov_ticks[i] = 0;
        }
    }

    return PFC_SUCCESS;
}

/*
 * @brief Check capacitors voltage
 * 
 * @param Ucap The capacitors voltage
 *
 * @return The status of the operation
 */
status_t events_check_ud(float Ucap)
{
	  static uint16_t ud_ov_ticks = 0;
    if (pfc_get_state() >= PFC_STATE_STOPPING || pfc_get_state() <= PFC_STATE_STOP) return PFC_SUCCESS;
		settings_protection_t protection = settings_get_protection();
    if (Ucap > protection.Ucap_max)
    {
        ud_ov_ticks++;
        if (ud_ov_ticks > 3)
        {
            events_new_event(
                EVENT_TYPE_PROTECTION,
                SUB_EVENT_TYPE_PROTECTION_UCAP_MAX,
                0,
                Ucap);
        }
    }
    else
    {
        ud_ov_ticks = 0;
    }
    if (pfc_get_state() >= PFC_STATE_CHARGE && Ucap < protection.Ucap_min)
    {
        events_new_event(
            EVENT_TYPE_PROTECTION,
            SUB_EVENT_TYPE_PROTECTION_UCAP_MIN,
            0,
            Ucap);
    }

    return PFC_SUCCESS;
}

/*
 * @brief Check the temperature
 *
 * @return The status of the operation
 */
status_t events_check_temperature(void)
{
    if (pfc_get_state() >= PFC_STATE_STOPPING || pfc_get_state() <= PFC_STATE_STOP) return PFC_SUCCESS;
		float temperature = adc_get_temperature();
		settings_protection_t protection = settings_get_protection();
    if (temperature > protection.temperature)
    {
        events_new_event(
            EVENT_TYPE_PROTECTION,
            SUB_EVENT_TYPE_PROTECTION_TEMPERATURE,
            0,
            temperature);
    }
		return PFC_SUCCESS;
}

/*
 * @brief Check the RMS voltage
 *
 * @return The status of the operation
 */
status_t events_check_rms_voltage(void)
{
    if (pfc_get_state() >= PFC_STATE_STOPPING || pfc_get_state() <= PFC_STATE_STOP) return PFC_SUCCESS;
		float active[ADC_CHANNEL_FULL_COUNT]={0}; 
		adc_get_active(active);
		settings_protection_t protection = settings_get_protection();
    for (int channel = 0; channel < PFC_NCHAN; channel++)
    {
#ifdef ONLY_A_CHANNEL
        if (channel > 0) return PFC_SUCCESS;
#endif
        if (active[ADC_U_A + channel] < protection.U_min)
        {
            events_new_event(
                EVENT_TYPE_PROTECTION,
                SUB_EVENT_TYPE_PROTECTION_U_MIN,
                channel,
                active[ADC_U_A + channel]);
        }
    }
		return PFC_SUCCESS;
}

/*
 * @brief Check overvoltage (amplitude)
 *
 * @param U A pointer to the channel voltages
 *
 * @return The status of the operation
 */
status_t events_check_overvoltage(float *U)
{
		static int ov_ticks[PFC_NCHAN] = {0, 0, 0};

    if (pfc_get_state() >= PFC_STATE_STOPPING || pfc_get_state() <= PFC_STATE_STOP) return PFC_SUCCESS;
		settings_protection_t protection = settings_get_protection();
    for (int channel = 0; channel < PFC_NCHAN; channel++)
    {
#ifdef ONLY_A_CHANNEL
        if (channel > 0) return PFC_SUCCESS;
#endif
        if (fabs(U[channel]/MATH_SQRT2) > protection.U_max)
        {
            ov_ticks[channel]++;
            if (ov_ticks[channel] > 3)
            {
                events_new_event(
                    EVENT_TYPE_PROTECTION,
                    SUB_EVENT_TYPE_PROTECTION_U_MAX,
                    channel,
                    U[channel]);
            }
        }
        else
        {
            ov_ticks[channel] = 0;
        }
    }
		return PFC_SUCCESS;
}

/*
 * @brief Check RMS overcurrent
 *
 * @return The status of the operation
 */
status_t events_check_rms_overcurrent(void)
{
    if (pfc_get_state() >= PFC_STATE_STOPPING || pfc_get_state() <= PFC_STATE_STOP) return PFC_SUCCESS;
		float active[ADC_CHANNEL_FULL_COUNT]={0}; 
		adc_get_active(active);
		settings_protection_t protection = settings_get_protection();
    for (int channel = 0; channel < PFC_NCHAN; channel++)
    {
#ifdef ONLY_A_CHANNEL
        if (i > 0) return PFC_SUCCESS;
#endif
        if (active[ADC_I_A + channel] > protection.I_max_rms)
        {
            events_new_event(
                EVENT_TYPE_PROTECTION,
                SUB_EVENT_TYPE_PROTECTION_IAFG_MAX_RMS,
                channel,
                active[ADC_I_A + channel]);
        }
    }
    return PFC_SUCCESS;
}

/*
 * @brief Check overcurrent (amplitude)
 *
 * @param I A pointer to the currents array
 *
 * @return The status of the operation
 */
status_t events_check_overcurrent(float *I)
{
    if (pfc_get_state() >= PFC_STATE_STOPPING || pfc_get_state() <= PFC_STATE_STOP) return PFC_SUCCESS;
	
		settings_protection_t protection = settings_get_protection();
    for (int channel = 0; channel < PFC_NCHAN; channel++)
    {
#ifdef ONLY_A_CHANNEL
        if (channel > 0) return PFC_SUCCESS;
#endif
        if (fabs(I[channel]) > protection.I_max_peak)
        {
            events_new_event(
                EVENT_TYPE_PROTECTION,
                SUB_EVENT_TYPE_PROTECTION_IAFG_MAX_PEAK,
                channel,
                I[channel]);
        }
    }

    return PFC_SUCCESS;
}

/*
 * @brief Check the period and frequency
 *
 * @param period The value of the period
 *
 * @return The status of the operation
 */
status_t events_check_period(uint32_t period)
{
    if (period == 0) return PFC_SUCCESS;
    float freq = 1.0f / ((float)period / 1e6f);
    if (pfc_get_state() >= PFC_STATE_STOPPING || pfc_get_state() <= PFC_STATE_STOP) return PFC_SUCCESS;
	
		settings_protection_t protection = settings_get_protection();
	
    if (freq > protection.F_max)
    {
        events_new_event(
            EVENT_TYPE_PROTECTION,
            SUB_EVENT_TYPE_PROTECTION_F_MAX,
            0,
            freq);
    }
    if (freq < protection.F_min)
    {
        events_new_event(
            EVENT_TYPE_PROTECTION,
            SUB_EVENT_TYPE_PROTECTION_F_MIN,
            0,
            freq);
    }
		/*
		TODO: Check the nesessity of the U_50Hz verification
		
		if(pfc_get_state()>=PFC_STATE_PRECHARGE_PREPARE && fabs(PFC.U_50Hz[PFC_ACHAN].phase-MATH_PI/2)>0.1){
			events_new_event(
						EVENT_TYPE_PROTECTION,
						SUB_EVENT_TYPE_PROTECTION_BAD_SYNC,
						0,
						PFC.U_50Hz[PFC_ACHAN].phase
					);
		}
		*/
		return PFC_SUCCESS;
}

/*
 * @brief Check the input channel phases
 *
 * @return The status of the operation
 */
status_t events_check_voltage_phase_rotation(void)
{
	/* TODO: Implement the check */
		return PFC_SUCCESS;
}

/*
 * @brief Check the current on the load
 *
 * @return The status of the operation
 */
status_t events_check_loading_current(void)
{
	/* TODO: Implement the check */
		return PFC_SUCCESS;
}

/*
 * @brief Check preloading operation (preloading has been started and is ongoing in the right direction)
 *
 * @return The status of the operation
 */
status_t events_preload_start(void)
{
	/* TODO: Implement the check */
		return PFC_SUCCESS;
}

/*
 * @brief Check preloading operation (preloading was stopped)
 *
 * @return The status of the operation
 */
status_t events_preload_stop(void)
{
	/* TODO: Implement the check */
		return PFC_SUCCESS;
}
