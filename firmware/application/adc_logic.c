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
#include "BSP/system.h"
#include "pfc_logic.h"
#include "command_processor.h"
#include "events_process.h"
#include "string.h"
#include "math.h"

/*--------------------------------------------------------------
                       DEFINES
--------------------------------------------------------------*/

#define CENTER 0.0f
#define UMAX_INITIAL_VALUE (-1000000)
#define UMIN_INITIAL_VALUE (1000000)
			
/*--------------------------------------------------------------
                       PRIVATE DATA
--------------------------------------------------------------*/


static float KphaseS = -1.0f;

static float K_filter_x = 0.5f;
static float K_filter_F = 0.8f;
static float K_filter_P = 0.3f;
static float diff = 0;

static const uint8_t needSquare[] = {
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

uint16_t ADC_DMA_buf[ADC_CHANNEL_NUMBER];
float adc_values[ADC_CHANNEL_NUMBER + ADC_MATH_NUMBER];
uint16_t adc_values_raw[ADC_CHANNEL_NUMBER];
uint8_t current_buffer = 0;
uint8_t last_buffer = 0;
uint16_t symbol = 0;
static uint8_t newPeriod = 0;
		
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

static void adc_lock(void)
{
	/* TODO: Add lock functionality */
}

static void adc_unlock(void)
{
	/* TODO: Add unlock functionality */
}

static void adc_half_cplt_callback(void)
{
		gpio_pwm_test_on();
    int i_isr;
    //HAL_ADC_Stop(&hadc1);
		
		adc_stop();
		adc_lock();
    memcpy(&adc_values_raw[8], &ADC_DMA_buf[8], sizeof(ADC_DMA_buf) / 2);
		adc_start((uint32_t*)ADC_DMA_buf, sizeof(ADC_DMA_buf));
	
    for (i_isr = 0; i_isr < ADC_CHANNEL_NUMBER; i_isr++)
    {
        adc_values[i_isr] = adc_values_raw[i_isr];
    }
    //events_was_CT_overload(adc_values);//TODO:
			
		settings_calibrations_t calibrations = settings_get_calibrations();
	
    /* Применяем смещения каналов */
    for (i_isr = 0; i_isr < ADC_CHANNEL_NUMBER; i_isr++)
    {
        /* Каналы переменного тока/напряжения смещаем на UREF */
        adc_values[i_isr] -= calibrations.offset[i_isr];
        adc_values[i_isr] *= calibrations.calibration[i_isr];

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
    if (!pfc_is_pwm_on())
    {
        Ia_e_1 = 0 - adc_values[ADC_I_A];
        Ib_e_1 = 0 - adc_values[ADC_I_B];
        Ic_e_1 = 0 - adc_values[ADC_I_C];
        ia = 0;
    }
    else
    {
				settings_capacitors_t capacitors = settings_get_capacitors();
        float VL = PID(
            capacitors.Ud_nominal - adc_get_cap_voltage(),
            &VLet_1,
            capacitors.ctrlUd_Kp,
            capacitors.ctrlUd_Ki,
            0,
            &VLIt_1);

        float IvlA = VL * PFC.adc.ch[last_buffer][ADC_MATH_A][symbol] / PFC.adc.active[ADC_MATH_A]; /*sin(symbol/128.0*2.0*MATH_PI)*/
        float IvlB = VL * PFC.adc.ch[last_buffer][ADC_MATH_B][symbol] / PFC.adc.active[ADC_MATH_B]; /*sin(symbol/128.0*2.0*MATH_PI+2.0*MATH_PI/3.0)*/
        float IvlC = VL * PFC.adc.ch[last_buffer][ADC_MATH_C][symbol] / PFC.adc.active[ADC_MATH_C]; /*sin(symbol/128.0*2.0*MATH_PI+4.0*MATH_PI/3.0)*/
        
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
		adc_unlock();
    gpio_pwm_test_off();
}

/*--------------------------------------------------------------
                       PUBLIC FUNCTIONS
--------------------------------------------------------------*/

float autoCorrelationFreq(const float *in)
{
    int order = ADC_VAL_NUM;
    float sum, max = -1000000;
    int i, j, maxpos;
    float out[ADC_VAL_NUM];

    for (i = 0; i < order; i++)
    {
        sum = 0;
        for (j = 0; j < order - i; j++)
        {
            sum += in[j] * in[j + i];
        }
        out[i] = sum;
        if (sum > max && i)
        {
            max = sum;
            maxpos = i;
        }
    }
    float dy = 0.5f * (out[maxpos - 1] - out[maxpos + 1]);
    float d2y = 2.0f * max - out[maxpos - 1] - out[maxpos + 1];
    float mp = maxpos + dy / d2y;
    return mp / 128.0f * 20000.0f;
}

void algorithm_work(void)
{
    if (newPeriod)
    {
				adc_lock(); /* TODO: check for overlapping with interrupts */
        //---------------------------------------
				settings_filters_t filters = settings_get_filters();
			
        float K_I = (filters.K_I);
        float K_U = (filters.K_U);
        float K_UD = (filters.K_UD);
        float K_Iinv = (1 - filters.K_I);
        float K_Uinv = (1 - filters.K_U);
        float K_UDinv = (1 - filters.K_UD);

        float umax[3] = {UMAX_INITIAL_VALUE};
        float umin[3] = {UMIN_INITIAL_VALUE};

        for (int i = 0; i < ADC_VAL_NUM; i++)
        {
            //------------- Math channels -------------
            float Uab = PFC.adc.ch[last_buffer][ADC_EMS_B][i] - PFC.adc.ch[last_buffer][ADC_EMS_A][i];
            float Ubc = PFC.adc.ch[last_buffer][ADC_EMS_C][i] - PFC.adc.ch[last_buffer][ADC_EMS_B][i];
            float Uca = PFC.adc.ch[last_buffer][ADC_EMS_A][i] - PFC.adc.ch[last_buffer][ADC_EMS_C][i];

            float Uan = (2 * Uab + Ubc) / 3;
            float Ubn = (-Uab + Ubc) / 3;
            float Ucn = -(Uan + Ubn);

            PFC.adc.ch[last_buffer][ADC_MATH_A][i] = Uan;
            PFC.adc.ch[last_buffer][ADC_MATH_B][i] = Ubn;
            PFC.adc.ch[last_buffer][ADC_MATH_C][i] = Ucn;
            //---------------------------------------
            for (int i_isr = 0; i_isr < ADC_CHANNEL_NUMBER + ADC_MATH_NUMBER; i_isr++)
            {
                if (needSquare[i_isr])
                {
										/* Calculate effective value for voltages and currents */
                    PFC.adc.sum_raw_sqr[last_buffer][i_isr] += SQUARE_F(PFC.adc.ch[last_buffer][i_isr][i]);
                }
                else
                {
										/* Calculate mean values for steady parameters */
                    PFC.adc.sum_raw_sqr[last_buffer][i_isr] += PFC.adc.ch[last_buffer][i_isr][i];
                }
            }
            //------------- Filter U and I ----------
            for (int i_isr = 0; i_isr < PFC_NCHAN; i_isr++)
            {
                IIR_1ORDER(
                    PFC.adc.ch[last_buffer][ADC_EMS_A + i_isr][i],
                    PFC.adc.ch[current_buffer][ADC_EMS_A + i_isr][i],
                    PFC.adc.ch[last_buffer][ADC_EMS_A + i_isr][i],
                    K_U,
                    K_Uinv);

                IIR_1ORDER(
                    PFC.adc.ch[last_buffer][ADC_I_A + i_isr][i],
                    PFC.adc.ch[current_buffer][ADC_I_A + i_isr][i],
                    PFC.adc.ch[last_buffer][ADC_I_A + i_isr][i],
                    K_I,
                    K_Iinv);

                if (umax[i_isr] < PFC.adc.ch[last_buffer][ADC_EMS_A + i_isr][i]) umax[i_isr] = PFC.adc.ch[last_buffer][ADC_EMS_A + i_isr][i];
                if (umin[i_isr] > PFC.adc.ch[last_buffer][ADC_EMS_A + i_isr][i]) umin[i_isr] = PFC.adc.ch[last_buffer][ADC_EMS_A + i_isr][i];
            }
            IIR_1ORDER(
                PFC.adc.ch[last_buffer][ADC_UD][i],
                PFC.adc.ch[current_buffer][ADC_UD][i],
                PFC.adc.ch[last_buffer][ADC_UD][i],
                K_UD,
                K_UDinv);
        }
        //---------- Calculate STD -------------------------
        for (int i_isr = 0; i_isr < ADC_CHANNEL_NUMBER + ADC_MATH_NUMBER; i_isr++)
        {
            if (needSquare[i_isr])
            {
                PFC.adc.active[i_isr] = sqrt(PFC.adc.sum_raw_sqr[last_buffer][i_isr] / ((float)ADC_VAL_NUM));
            }
            else
            {
                PFC.adc.active[i_isr] = PFC.adc.sum_raw_sqr[last_buffer][i_isr] / ((float)ADC_VAL_NUM);
            }
            PFC.adc.sum_raw_sqr[last_buffer][i_isr] = 0;
        }
        //---------------------------------------
        //HAL_GPIO_TogglePin(GPIOD, LED_1_Pin);
        protocol_write_osc_data((float**)PFC.adc.ch[last_buffer]);
        //---------------------------------------
        events_check_overcurrent_rms();
        events_check_voltage_RMS();
        events_check_period(PFC.period_fact);
        //------------FREQ-----------------------
        /*float f=autoCorrelationFreq(PFC.adc.ch[last_buffer][ADC_EMS_A]);
					IIR_1ORDER(
							PFC.period_fact,
							f,
							PFC.period_fact,							
							(1.0f-K_filter_F),
							K_filter_F
						);
			*/
        float x = PFC.adc.ch[last_buffer][ADC_MATH_A][0];
        float x_last = x;

        float cntr = 0;  //(umax[0]-umin[0])/2;

        static float Plast = 0, P;
        for (int j = 0; j < ADC_VAL_NUM; j++)
        {
            IIR_1ORDER(
                x,
                PFC.adc.ch[last_buffer][ADC_MATH_A][j],
                x,
                (1.0f - K_filter_x),
                K_filter_x);

            if (x_last > cntr && x <= cntr)
            {
                P = ((float)j - 1.0f) + ((float)x_last - cntr) / ((float)x_last - (float)x);
                if ((P - Plast) > (128 / 4 * 3))
                {
                    float last_period = PFC.period_fact;

                    IIR_1ORDER(
                        PFC.period_fact,
                        (P - Plast) * (1.0f / 128.0f) * PFC.period_fact,
                        PFC.period_fact,
                        (1.0f - K_filter_F),
                        K_filter_F);

                    Plast = P;

                    IIR_1ORDER(
                        diff,
                        diff,
                        (P - 64.0f),
                        (1.0f - K_filter_P),
                        K_filter_P);
                    if (diff > 10) diff = 10;
                    if (diff < -10) diff = -10;
                    PFC.period_fact -= diff * KphaseS;
                    PFC.period_delta = PFC.period_fact - last_period;
                }
            }
            x_last = x;
        }
        Plast -= ADC_VAL_NUM;
        //---------------------
        if (PFC.period_fact > (20000 + 1000)) PFC.period_fact = (20000 + 1000);
        if (PFC.period_fact < (20000 - 1000)) PFC.period_fact = (20000 - 1000);
				
				uint32_t arr = (200000000.0f / 2.0f / 128.0f * (PFC.period_fact / 1000000.0f));
				timer_correct_period(arr);
        //---------------------
        pfc_process();
        //---------------------
        //---------------------
        static uint32_t lastprint = 0;
        if ((system_get_time() - lastprint) > 500)
        {
            lastprint = system_get_time();
            //printf("\n\nPFC program, %s %s\n",__DATE__,__TIME__);
        }
        //---------------------
        newPeriod = 0;
				adc_unlock();
    }		
}

float adc_get_cap_voltage(void)
{
	adc_lock();
	float ud_ret = PFC.adc.active[ADC_UD];
	adc_unlock();
	return ud_ret;
}

status_t adc_logic_start(void)
{
	adc_register_callbacks(adc_cplt_callback, adc_half_cplt_callback);
	
	adc_start((uint32_t*)ADC_DMA_buf, sizeof(ADC_DMA_buf));

	timer_start_adc_timer();

	return PFC_SUCCESS;
}

void adc_get_complex_phase(ComplexAmpPhase* U_50Hz, float* period_delta)
{
	adc_lock();
	if(U_50Hz)memcpy(U_50Hz, PFC.U_50Hz, sizeof(PFC.U_50Hz));
	if(period_delta)*period_delta = PFC.period_delta;
	adc_unlock();
}

void adc_get_params(float* U_0Hz, float* I_0Hz, float* U_phase, float* thdu, float* period_fact)
{
		adc_lock();
		if(U_0Hz)memcpy(U_0Hz, PFC.U_0Hz, sizeof(PFC.U_0Hz));
		if(I_0Hz)memcpy(I_0Hz, PFC.I_0Hz, sizeof(PFC.I_0Hz));
		if(U_phase)memcpy(U_phase, PFC.U_phase, sizeof(PFC.U_phase));
		if(thdu)memcpy(thdu, PFC.thdu, sizeof(PFC.thdu));
		if(period_fact)*period_fact = PFC.period_fact;
		adc_unlock();
}

float adc_get_temperature(void)
{
	float ret_temp=0;
	adc_lock();
	ret_temp = PFC.temperature;
	adc_unlock();
	return ret_temp;
}

void adc_set_temperature(float temperature)
{
	adc_lock();
	PFC.temperature = temperature;
	adc_unlock();
}

void adc_get_active(float* active)
{
	adc_lock();
	if(active)memcpy(active, PFC.adc.active, sizeof(PFC.adc.active));
	adc_unlock();
}

void adc_get_active_raw(float* active_raw)
{
	adc_lock();
	if(active_raw)memcpy(active_raw, PFC.adc.active_raw, sizeof(PFC.adc.active_raw));
	adc_unlock();
}
