/**
 * @file adc_logic.c
 * @author Stanislav Karpikov
 * @brief ADC support and control module
 */
/*--------------------------------------------------------------
                       INCLUDES
--------------------------------------------------------------*/

#include "adc_logic.h"

#include "BSP/timer.h"
#include "BSP/adc.h"
#include "BSP/gpio.h"
#include "pfc_logic.h"
#include "command_processor.h"
#include "events_process.h"
#include "string.h"

/*--------------------------------------------------------------
                       PRIVATE DATA
--------------------------------------------------------------*/

uint16_t ADC_DMA_buf[ADC_CHANNEL_NUMBER];
float adc_values[ADC_CHANNEL_NUMBER + ADC_MATH_NUMBER];
uint16_t adc_values_raw[ADC_CHANNEL_NUMBER];
uint8_t current_buffer = 0;
uint8_t last_buffer = 0;
uint16_t symbol = 0;
uint8_t newPeriod = 0;
uint8_t needSquare[] = {
    0,  //CH10 - ADC_UD
    1,  //CH11 - ADC_U_A
    1,  //CH12 - ADC_U_B
    1,  //CH13 - ADC_U_C
    0,  //CH0 - ADC_I_A
    0,  //CH1 - ADC_I_B
    0,  //CH2 - ADC_I_C
    0,  //CH3 - ADC_I_ET
    0,  //CH5 - ADC_I_TEMP1
    0,  //CH6 - ADC_I_TEMP2
    1,  //CH14 - ADC_EMS_A
    1,  //CH15 - ADC_EMS_B
    1,  //CH8 - ADC_EMS_C
    1,  //CH9 - ADC_EMS_I
    1,
    1,
    1
};

//ADC_MATH_A,
//ADC_MATH_B,
//ADC_MATH_C,
//ADC_MATH_C_A,
//ADC_MATH_C_B,
//ADC_MATH_C_C
		
float VLet_1 = 0;  //прошлое значение ошибок
float VLIt_1 = 0;  //прошлое значение интегральной составляющей
float Ia_e_1 = 0;        //прошлое значение ошибок
float Ia_It_1 = 0;       //прошлое значение интегральной составляющей
float Ib_e_1 = 0;        //прошлое значение ошибок
float Ib_It_1 = 0;       //прошлое значение интегральной составляющей
float Ic_e_1 = 0;        //прошлое значение ошибок
float Ic_It_1 = 0;       //прошлое значение интегральной составляющей
float Kp = 0, Ki = 0.2;  //Ki=0.0003;
float ia = 0;


typedef struct
{
    float ch[BUF_NUM][ADC_CHANNEL_FULL_COUNT][ADC_VAL_NUM];

    float active[ADC_CHANNEL_FULL_COUNT];  //RMS or mean value with correction
    uint16_t active_raw[ADC_CHANNEL_NUMBER];             //RMS or mean value without correction

    //Temp sum for calculate adc active value
    float sum_raw_sqr[BUF_NUM][ADC_CHANNEL_FULL_COUNT];
    float sum_raw[BUF_NUM][ADC_CHANNEL_FULL_COUNT];
}adc_t;

typedef struct
{
    PROTOCOL_CONTEXT protocol;
    SETTINGS settings;
    adc_t adc;

    ComplexAmpPhase U_50Hz[PFC_NCHAN];
    float period_delta;

    float period_fact;
    float U_0Hz[PFC_NCHAN];
    float I_0Hz[PFC_NCHAN];
    float U_phase[PFC_NCHAN];
    float thdu[PFC_NCHAN];

    float temperature;
}pfc_t;

static pfc_t PFC;

/*--------------------------------------------------------------
                       PRIVATE FUNCTIONS
--------------------------------------------------------------*/

void adc_cplt_callback(void)
{
    memcpy(adc_values_raw, ADC_DMA_buf, sizeof(ADC_DMA_buf) / 2);
}

//Расчёт ПИД регулятора
float PID(float et, float* et_1, float Kp, float Ki, float Kd, float* It_1)
{
    float Pt = Kp * et;
    float It = *It_1 + Ki * et;
    *It_1 = It /**0.999*/;  //утечка
    //float Dt=Kd*(et-*et_1);
    float Ut = Pt + It /*+Dt*/;
    *et_1 = et;
    return Ut;
};

static void adc_half_cplt_callback(void)
{
		gpio_pwm_test_on();
    int i_isr;
    //HAL_ADC_Stop(&hadc1);
		
		adc_stop();
    memcpy(&adc_values_raw[8], &ADC_DMA_buf[8], sizeof(ADC_DMA_buf) / 2);
		adc_start((uint32_t*)ADC_DMA_buf, sizeof(ADC_DMA_buf));
	
    for (i_isr = 0; i_isr < ADC_CHANNEL_NUMBER; i_isr++)
    {
        adc_values[i_isr] = adc_values_raw[i_isr];
    }
    //events_was_CT_overload(adc_values);//TODO:
		
    /* Применяем смещения каналов */
    for (i_isr = 0; i_isr < ADC_CHANNEL_NUMBER; i_isr++)
    {
        /* Каналы переменного тока/напряжения смещаем на UREF */
        adc_values[i_isr] -= PFC.settings.CALIBRATIONS.offset[i_isr];
        adc_values[i_isr] *= PFC.settings.CALIBRATIONS.calibration[i_isr];

        PFC.adc.ch[current_buffer][i_isr][symbol] = adc_values[i_isr];
    }
    //events_check_overcurrent_peak(&adc_values[ADC_I_A]);
    //events_check_overvoltage_transient(&adc_values[ADC_U_A]);
    if (pfc_get_state() >= PFC_STATE_CHARGE && pfc_get_state() < PFC_STATE_FAULTBLOCK)
    {
        events_check_Ud(adc_values[ADC_UD]);
    }
    //restart adc
    //HAL_ADC_Start(&hadc1);

    //=======================================================
    //=======================================================
    if (!PWMon)
    {
        Ia_e_1 = 0 - adc_values[ADC_I_A];
        Ib_e_1 = 0 - adc_values[ADC_I_B];
        Ic_e_1 = 0 - adc_values[ADC_I_C];
        ia = 0;
    }
    else
    {
        float VL = PID(
            PFC.settings.CAPACITORS.Ud_nominal - adc_get_cap_voltage(),
            &VLet_1,
            PFC.settings.CAPACITORS.ctrlUd_Kp,
            PFC.settings.CAPACITORS.ctrlUd_Ki,
            0,
            &VLIt_1);

        float IvlA = VL * PFC.adc.ch[last_buffer][ADC_MATH_A][symbol] / PFC.adc.active[ADC_MATH_A]; /*sin(symbol/128.0*2.0*MATH_PI)*/
        ;
        float IvlB = VL * PFC.adc.ch[last_buffer][ADC_MATH_B][symbol] / PFC.adc.active[ADC_MATH_B]; /*sin(symbol/128.0*2.0*MATH_PI+2.0*MATH_PI/3.0)*/
        ;
        float IvlC = VL * PFC.adc.ch[last_buffer][ADC_MATH_C][symbol] / PFC.adc.active[ADC_MATH_C]; /*sin(symbol/128.0*2.0*MATH_PI+4.0*MATH_PI/3.0)*/
        ;
        /*
		IIR_1ORDER(
				ia,
				adc_values[ADC_I_A],
				ia,							
				(1.0f-Kp),
				Kp
			);*/

        float va = PID(
            IvlA - adc_values[ADC_I_A] + PFC.adc.active[ADC_I_A],
            &Ia_e_1,
            0,
            Ki,
            0,
            &Ia_It_1);
        float vb = PID(
            IvlB - adc_values[ADC_I_B] + PFC.adc.active[ADC_I_B],
            &Ib_e_1,
            0,
            Ki,
            0,
            &Ib_It_1);
        float vc = PID(
            IvlC - adc_values[ADC_I_C] + PFC.adc.active[ADC_I_C],
            &Ic_e_1,
            0,
            Ki,
            0,
            &Ic_It_1);
        Ia_It_1 *= 0.995f;
        Ib_It_1 *= 0.995f;
        Ic_It_1 *= 0.995f;

        if (Ia_It_1 > (1.0f)) Ia_It_1 = 1.0f;
        if (Ib_It_1 > (1.0f)) Ib_It_1 = 1.0f;
        if (Ic_It_1 > (1.0f)) Ic_It_1 = 1.0f;
        if (Ia_It_1 < (-1.0f)) Ia_It_1 = -1.0f;
        if (Ib_It_1 < (-1.0f)) Ib_It_1 = -1.0f;
        if (Ic_It_1 < (-1.0f)) Ic_It_1 = -1.0f;

        if (va > (1.0f - EPS)) va = 1.0f - EPS;
        if (vb > (1.0f - EPS)) vb = 1.0f - EPS;
        if (vc > (1.0f - EPS)) vc = 1.0f - EPS;
        if (va < (-1.0f + EPS)) va = -1.0f + EPS;
        if (vb < (-1.0f + EPS)) vb = -1.0f + EPS;
        if (vc < (-1.0f + EPS)) vc = -1.0f + EPS;

				uint32_t ccr1 = (float)PWM_PERIOD * (-va * 0.5f + 0.5f);
        uint32_t ccr2 = (float)PWM_PERIOD * (-vb * 0.5f + 0.5f);
        uint32_t ccr3 = (float)PWM_PERIOD * (-vc * 0.5f + 0.5f);
				timer_write_pwm(ccr1, ccr2, ccr3);

        PFC.adc.ch[current_buffer][ADC_MATH_C_A][symbol] = va;
        PFC.adc.ch[current_buffer][ADC_MATH_C_B][symbol] = vb;
        PFC.adc.ch[current_buffer][ADC_MATH_C_C][symbol] = vc;
    }
    symbol++;
    if (symbol >= ADC_VAL_NUM)
    {
        symbol = 0;
        current_buffer ^= 1;
        last_buffer = !current_buffer;
        newPeriod = 1;
    }
    gpio_pwm_test_off();
}

/*--------------------------------------------------------------
                       PUBLIC FUNCTIONS
--------------------------------------------------------------*/

float adc_get_cap_voltage(void)
{
	return PFC.adc.active[ADC_UD];
}

status_t adc_logic_start(void)
{
	adc_register_callbacks(adc_cplt_callback, adc_half_cplt_callback);
	
	adc_start((uint32_t*)ADC_DMA_buf, sizeof(ADC_DMA_buf));

	timer_start_adc_timer();

	return PFC_SUCCESS;
}

void adc_get_active(float* U_0Hz, float* I_0Hz, float* U_phase, float* thdu)
{
		memcpy(U_0Hz, PFC.U_0Hz, sizeof(PFC.U_0Hz));
		memcpy(I_0Hz, PFC.I_0Hz, sizeof(PFC.I_0Hz));
		memcpy(U_phase, PFC.U_phase, sizeof(PFC.U_phase));
		memcpy(thdu, PFC.thdu, sizeof(PFC.thdu));
}


void adc_get_active(float* active)
{
	if(active)memcpy(active, PFC.adc.active, sizeof(PFC.adc.active));
}
