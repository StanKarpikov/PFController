#include "events_process.h"
#include "math.h"

#define ADC_MARGIN                  100
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
                NEWEVENT(EVENT_TYPE_PROTECTION,
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
    if (PFC.status >= PFC_STATE_STOPPING || PFC.status <= PFC_STATE_STOP) return 0;
    if (Ud > PFC.settings.PROTECTION.Ud_max)
    {
        overUDTicks++;
        if (overUDTicks > 3)
        {
            NEWEVENT(
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
    if (PFC.status >= PFC_STATE_CHARGE && Ud < PFC.settings.PROTECTION.Ud_min)
    {
        NEWEVENT(
            EVENT_TYPE_PROTECTION,
            SUB_EVENT_TYPE_PROTECTION_UD_MIN,
            0,
            Ud);
    }

    return EVENT_OK;
}

void events_check_temperature(void)
{
    if (PFC.status >= PFC_STATE_STOPPING || PFC.status <= PFC_STATE_STOP) return;
    if (PFC.temperature > PFC.settings.PROTECTION.temperature /* TEMPERATURE_HW_MAX */)
    {
        NEWEVENT(
            EVENT_TYPE_PROTECTION,
            SUB_EVENT_TYPE_PROTECTION_TEMPERATURE,
            0,
            PFC.temperature);
    }
}

void events_check_voltage_RMS(void)
{
    int i;

    if (PFC.status >= PFC_STATE_STOPPING || PFC.status <= PFC_STATE_STOP) return;

    for (i = 0; i < PFC_NCHAN; i++)
    {
#ifdef ONLY_A_CHANNEL
        if (i > 0) return;
#endif
        if (PFC.adc.active[ADC_U_A + i] < PFC.settings.PROTECTION.U_min)
        {
            NEWEVENT(
                EVENT_TYPE_PROTECTION,
                SUB_EVENT_TYPE_PROTECTION_U_MIN,
                i,
                PFC.adc.active[ADC_U_A + i]);
        }
    }
}

#define SQRT_OF_2             (1 / 1.4142135623730950488016887242097)
#define RMS_TO_INSTANT_SIN(r) ((r)*SQRT_OF_2)

uint16_t overVoltageTicks[PFC_NCHAN] = {0, 0, 0};
void events_check_overvoltage_transient(float *U)
{
    int channel;

    if (PFC.status >= PFC_STATE_STOPPING || PFC.status <= PFC_STATE_STOP) return;
    for (channel = 0; channel < PFC_NCHAN; channel++)
    {
#ifdef ONLY_A_CHANNEL
        if (channel > 0) return;
#endif
        if (fabs(RMS_TO_INSTANT_SIN(U[channel])) > PFC.settings.PROTECTION.U_max)
        {
            overVoltageTicks[channel]++;
            if (overVoltageTicks[channel] > 3)
            {
                NEWEVENT(
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

    if (PFC.status >= PFC_STATE_STOPPING || PFC.status <= PFC_STATE_STOP) return EVENT_OK;
    for (i = 0; i < PFC_NCHAN; i++)
    {
#ifdef ONLY_A_CHANNEL
        if (i > 0) return 0;
#endif
        if (PFC.adc.active[ADC_I_A + i] > PFC.settings.PROTECTION.I_max_rms)
        {
            NEWEVENT(
                EVENT_TYPE_PROTECTION,
                SUB_EVENT_TYPE_PROTECTION_IAFG_MAX_RMS,
                i,
                PFC.adc.active[ADC_I_A + i]);
        }
    }
    return EVENT_OK;
}

int events_check_overcurrent_peak(float *IPFC)
{
    int channel;

    if (PFC.status >= PFC_STATE_STOPPING || PFC.status <= PFC_STATE_STOP) return EVENT_OK;
    for (channel = 0; channel < PFC_NCHAN; channel++)
    {
#ifdef ONLY_A_CHANNEL
        if (channel > 0) return 0;
#endif
        if (fabs(IPFC[channel]) > PFC.settings.PROTECTION.I_max_peak)
        {
            NEWEVENT(
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

    if (PFC.status >= PFC_STATE_STOPPING || PFC.status <= PFC_STATE_STOP) return;
    if (freq > PFC.settings.PROTECTION.Fnet_max)
    {
        NEWEVENT(
            EVENT_TYPE_PROTECTION,
            SUB_EVENT_TYPE_PROTECTION_F_MAX,
            0,
            freq);
    }
    if (freq < PFC.settings.PROTECTION.Fnet_min)
    {
        NEWEVENT(
            EVENT_TYPE_PROTECTION,
            SUB_EVENT_TYPE_PROTECTION_F_MIN,
            0,
            freq);
    } /*TODO:
	if(PFC.status>=PFC_STATE_PRECHARGE_PREPARE && fabs(PFC.U_50Hz[0].phase-MATH_PI/2)>0.1){
		NEWEVENT(
					EVENT_TYPE_PROTECTION,
					SUB_EVENT_TYPE_PROTECTION_BAD_SYNC,
					0,
					PFC.U_50Hz[0].phase
				);
	}*/
}
