#include "events_process.h"
#include "pfc_logic.h"
#include "math.h"

#define ADC_MARGIN                  (100)
#define IS_ADC_VALUE_NEAR_BOUNDS(x) ((x) > (4095 - ADC_MARGIN) || (x) < ADC_MARGIN)

#ifdef ONLY_A_CHANNEL
uint16_t passingADCChannels[] = {
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
    1,  // ADC_UD_1;
    1   // ADC_UD_2;
};

inline uint16_t isPassADCChannel(uint16_t channel)
{
    return !(passingADCChannels[channel]);
}
#endif

uint16_t CT_overloadTicks[ADC_EMS_I + 1];
char events_was_CT_overload(float *adc_values)
{
    char i;
    char was_ovrld = 0;
    //char gain;

    for (i = 0; i < ADC_EMS_I + 1; i++)
    {
#ifdef ONLY_A_CHANNEL
        if (isPassADCChannel(i)) continue;
#endif
        if (IS_ADC_VALUE_NEAR_BOUNDS(adc_values[i]))
        {
            CT_overloadTicks[i]++;
            if (CT_overloadTicks[i] > 3)
            {
                events_new_event(EVENT_TYPE_PROTECTION,
                         SUB_EVENT_TYPE_PROTECTION_ADC_OVERLOAD,
                         i,
                         adc_values[i]);
            }
        }
        else
        {
            CT_overloadTicks[i] = 0;
        }
    }

    return was_ovrld;
}

uint16_t overUDTicks;
int events_check_Ud(float Ud)
{
    if (pfc_get_state() >= PFC_STATE_STOPPING || pfc_get_state() <= PFC_STATE_STOP) return 0;
		settings_protection_t protection = settings_get_protection();
    if (Ud > protection.Ud_max)
    {
        overUDTicks++;
        if (overUDTicks > 3)
        {
            events_new_event(
                EVENT_TYPE_PROTECTION,
                SUB_EVENT_TYPE_PROTECTION_UD_MAX,
                0,
                Ud);
        }
    }
    else
    {
        overUDTicks = 0;
    }
    if (pfc_get_state() >= PFC_STATE_CHARGE && Ud < protection.Ud_min)
    {
        events_new_event(
            EVENT_TYPE_PROTECTION,
            SUB_EVENT_TYPE_PROTECTION_UD_MIN,
            0,
            Ud);
    }

    return EVENT_OK;
}

void events_check_temperature(void)
{
    if (pfc_get_state() >= PFC_STATE_STOPPING || pfc_get_state() <= PFC_STATE_STOP) return;
		float temperature = adc_get_temperature();
		settings_protection_t protection = settings_get_protection();
    if (temperature > protection.temperature /* TEMPERATURE_HW_MAX */)
    {
        events_new_event(
            EVENT_TYPE_PROTECTION,
            SUB_EVENT_TYPE_PROTECTION_TEMPERATURE,
            0,
            temperature);
    }
}

void events_check_voltage_RMS(void)
{
    int i;

    if (pfc_get_state() >= PFC_STATE_STOPPING || pfc_get_state() <= PFC_STATE_STOP) return;
		float active[ADC_CHANNEL_FULL_COUNT]={0}; 
		adc_get_active(active);
		settings_protection_t protection = settings_get_protection();
    for (i = 0; i < PFC_NCHAN; i++)
    {
#ifdef ONLY_A_CHANNEL
        if (i > 0) return;
#endif
        if (active[ADC_U_A + i] < protection.U_min)
        {
            events_new_event(
                EVENT_TYPE_PROTECTION,
                SUB_EVENT_TYPE_PROTECTION_U_MIN,
                i,
                active[ADC_U_A + i]);
        }
    }
}

#define SQRT_OF_2             (1 / 1.4142135623730950488016887242097)
#define RMS_TO_INSTANT_SIN(r) ((r)*SQRT_OF_2)

uint16_t overVoltageTicks[PFC_NCHAN] = {0, 0, 0};
void events_check_overvoltage_transient(float *U)
{
    int channel;

    if (pfc_get_state() >= PFC_STATE_STOPPING || pfc_get_state() <= PFC_STATE_STOP) return;
		settings_protection_t protection = settings_get_protection();
    for (channel = 0; channel < PFC_NCHAN; channel++)
    {
#ifdef ONLY_A_CHANNEL
        if (channel > 0) return;
#endif
        if (fabs(RMS_TO_INSTANT_SIN(U[channel])) > protection.U_max)
        {
            overVoltageTicks[channel]++;
            if (overVoltageTicks[channel] > 3)
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
            overVoltageTicks[channel] = 0;
        }
    }
}

int events_check_overcurrent_rms(void)
{
    char i;

    if (pfc_get_state() >= PFC_STATE_STOPPING || pfc_get_state() <= PFC_STATE_STOP) return EVENT_OK;
		float active[ADC_CHANNEL_FULL_COUNT]={0}; 
		adc_get_active(active);
		settings_protection_t protection = settings_get_protection();
    for (i = 0; i < PFC_NCHAN; i++)
    {
#ifdef ONLY_A_CHANNEL
        if (i > 0) return 0;
#endif
        if (active[ADC_I_A + i] > protection.I_max_rms)
        {
            events_new_event(
                EVENT_TYPE_PROTECTION,
                SUB_EVENT_TYPE_PROTECTION_IAFG_MAX_RMS,
                i,
                active[ADC_I_A + i]);
        }
    }
    return EVENT_OK;
}

int events_check_overcurrent_peak(float *IPFC)
{
    int channel;

    if (pfc_get_state() >= PFC_STATE_STOPPING || pfc_get_state() <= PFC_STATE_STOP) return EVENT_OK;
	
		settings_protection_t protection = settings_get_protection();
    for (channel = 0; channel < PFC_NCHAN; channel++)
    {
#ifdef ONLY_A_CHANNEL
        if (channel > 0) return 0;
#endif
        if (fabs(IPFC[channel]) > protection.I_max_peak)
        {
            events_new_event(
                EVENT_TYPE_PROTECTION,
                SUB_EVENT_TYPE_PROTECTION_IAFG_MAX_PEAK,
                channel,
                IPFC[channel]);
        }
    }

    return EVENT_OK;
}

void events_check_period(unsigned int period_length)
{
    if (period_length == 0) return;
    float freq = 1.0f / ((float)period_length / 1000000.0f);
    if (pfc_get_state() >= PFC_STATE_STOPPING || pfc_get_state() <= PFC_STATE_STOP) return;
	
		settings_protection_t protection = settings_get_protection();
	
    if (freq > protection.Fnet_max)
    {
        events_new_event(
            EVENT_TYPE_PROTECTION,
            SUB_EVENT_TYPE_PROTECTION_F_MAX,
            0,
            freq);
    }
    if (freq < protection.Fnet_min)
    {
        events_new_event(
            EVENT_TYPE_PROTECTION,
            SUB_EVENT_TYPE_PROTECTION_F_MIN,
            0,
            freq);
    } 
		/*TODO: Check the nesessity of the U_50Hz verification
	if(pfc_get_state()>=PFC_STATE_PRECHARGE_PREPARE && fabs(PFC.U_50Hz[0].phase-MATH_PI/2)>0.1){
		events_new_event(
					EVENT_TYPE_PROTECTION,
					SUB_EVENT_TYPE_PROTECTION_BAD_SYNC,
					0,
					PFC.U_50Hz[0].phase
				);
	}*/
}
