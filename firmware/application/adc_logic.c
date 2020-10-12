/**
 * @file adc_logic.c
 * @author Stanislav Karpikov
 * @brief ADC support and control module
 */
 /** @addtogroup app_adc_logic
 * @{
 */
/*--------------------------------------------------------------
                       INCLUDES
--------------------------------------------------------------*/

#include "adc_logic.h"

#include "BSP/adc.h"
#include "BSP/gpio.h"
#include "BSP/system.h"
#include "BSP/timer.h"
#include "command_processor.h"
#include "events_process.h"
#include "math.h"
#include "pfc_logic.h"
#include "string.h"

/*--------------------------------------------------------------
                       DEFINES
--------------------------------------------------------------*/

#define UMAX_INITIAL_VALUE         (-1000000L) /**< Initial value to calculate the maximum */
#define UMIN_INITIAL_VALUE         (1000000L)  /**< Initial value to calculate the minimum */
#define PID_LEAKAGE_COEFFICIENT    (1.0f)      /**< The leakage coefficient for the PIC controller. Can be 0.999f */
#define PID_USE_DIFFERENTIAL       (0)         /**< Use differential coefficient in the PID controller */
#define GLOBAL_LEAKAGE_COEFFICIENT (0.995f)    /**< The leakage coefficient accumulator variables */

#define PERIOD_REQUIRED     (20000UL) /**< The required period value in [us] */
#define PERIOD_DRIFT        (1000UL)  /**< The acceptable period drift in [us] */
#define MAXIMUM_PERIOD_DIFF (10)      /**< The acceptable period difference */

/*--------------------------------------------------------------
                       PRIVATE DATA
--------------------------------------------------------------*/

static float K_phase_shift = -1.0f; /**< The K coefficient for the frequency and phase shift correction */

static float K_filter_x = 0.5f; /**< The K coefficient for the ADC signal shape smoothing */
static float K_filter_F = 0.8f; /**< The K coefficient for the frequency measurement filter */
static float K_filter_P = 0.3f; /**< The K coefficient for the period measurement filter */
static float diff = 0;          /**< The period difference (between required and measured) */

/** A channel needs the square calculation (e.g. effective value) */
static const uint8_t needSquare[] = {
    0, /*CH10 - ADC_UCAP*/
    1, /*CH11 - ADC_U_A*/
    1, /*CH12 - ADC_U_B*/
    1, /*CH13 - ADC_U_C*/
    0, /*CH0 - ADC_I_A*/
    0, /*CH1 - ADC_I_B*/
    0, /*CH2 - ADC_I_C*/
    0, /*CH3 - ADC_I_ET*/
    0, /*CH5 - ADC_I_TEMP1*/
    0, /*CH6 - ADC_I_TEMP2*/
    1, /*CH14 - ADC_EDC_A*/
    1, /*CH15 - ADC_EDC_B*/
    1, /* - */
    1, /* - */
    1, /* - */
    1, /* - */
    1  /* - */
};

static uint16_t adc_dma_buffer[ADC_CHANNEL_NUMBER];            /**< The buffer for the DMA ADC data */
static float adc_values[ADC_CHANNEL_NUMBER + ADC_MATH_NUMBER]; /**< Temporary storage of the ADC data */
static uint16_t adc_values_raw[ADC_CHANNEL_NUMBER];            /**< Temporary storage of the ADC data (raw values) */
static uint8_t current_buffer = 0;                             /**< Current buffer ID (for double buffering) */
static uint8_t last_buffer = 0;                                /**< Last buffer ID (for double buffering) */
static uint16_t symbol = 0;                                    /**< The current position in the buffer */
static uint8_t new_period = 0;                                 /**< A new period measurement has been starteds */

static float VLet_1 = 0;  /**< Last value for the VL error */
static float VLIt_1 = 0;  /**< Last value for the VL integral part */
static float Ia_e_1 = 0;  /**< Last value for the Ia error */
static float Ia_It_1 = 0; /**< Last value for the Ia integral part */
static float Ib_e_1 = 0;  /**< Last value for the Ib error */
static float Ib_It_1 = 0; /**< Last value for the Ib integral part */
static float Ic_e_1 = 0;  /**< Last value for the Ic error */
static float Ic_It_1 = 0; /**< Last value for the Ic integral part */

//static float Kp = 0.0f; /**< The proportional coefficient */
static float Ki = 0.2f; /**< The integral coefficient, TODO: test with Ki=0.0003; */

/** ADC data structure */
typedef struct
{
    /** Double buffers for the measured data */
    float ch[BUF_NUM][ADC_CHANNEL_FULL_COUNT][ADC_VAL_NUM];

    float active[ADC_CHANNEL_FULL_COUNT];    /**< RMS or mean value with correction, instantenous values */
    uint16_t active_raw[ADC_CHANNEL_NUMBER]; /**< RMS or mean value without correction, instantenous values */

    float sum_raw_sqr[BUF_NUM][ADC_CHANNEL_FULL_COUNT]; /**< Temporary sum for calculate adc active value (squared) */
    float sum_raw[BUF_NUM][ADC_CHANNEL_FULL_COUNT];     /**< Temporary sum for calculate adc active value */
} adc_t;

/** pfc operation data */
typedef struct
{
    adc_t adc; /**< ADC data */

    complex_amp_t U_50Hz[PFC_NCHAN]; /**< ADC data */
    float period_delta;              /**< The instantenous period error */
    float period_fact;               /**< The instantenous period value */
    float U_0Hz[PFC_NCHAN];          /**< The DC part of the U signal waveform */
    float I_0Hz[PFC_NCHAN];          /**< The DC part of the I signal waveform */
    float U_phase[PFC_NCHAN];        /**< The phase the U signal waveform */
    float thdu[PFC_NCHAN];           /**< The total harmonic distortion (on U) */

    float temperature; /**< The temperature of the unit */
} pfc_t;

static pfc_t pfc; /**< PFC operation data instance */

/*--------------------------------------------------------------
                       PRIVATE FUNCTIONS
--------------------------------------------------------------*/

/**
 * @brief  ADC DMA complete callback
 */
void adc_cplt_callback(void)
{
    memcpy(adc_values_raw, adc_dma_buffer, sizeof(adc_dma_buffer) / 2);
}

/**
 * @brief PID controller
 *
 * @param et The curent value of the error
 * @param et_1 The last value of the error
 * @param Kp The protortional coefficient
 * @param Ki The integral coefficient
 * @param Kd The differential coefficient
 * @param It_1 Integral value accumulator
 *
 * @return Regulated value, return 0 in case of an error
 */
static float PID(float et, float* et_1, float Kp, float Ki, float Kd, float* It_1)
{
    if (!et_1 || !It_1) return 0;

    float Pt = Kp * et;
    float It = *It_1 + Ki * et;
    *It_1 = It * PID_LEAKAGE_COEFFICIENT;
#if PID_USE_DIFFERENTIAL == 1
    float Dt = Kd * (et - *et_1);
    float Ut = Pt + It + Dt;
#else
    float Ut = Pt + It;
#endif
    *et_1 = et;
    return Ut;
};

/**
 * @brief Lock the ADC module (mutex)
 */
static void adc_lock(void)
{
    /* TODO: Add lock functionality */
}

/**
 * @brief Unlock the ADC module (mutex)
 */
static void adc_unlock(void)
{
    /* TODO: Add unlock functionality */
}

/**
 * @brief ADC DMA half complete callback
 */
static void adc_half_cplt_callback(void)
{
    gpio_pwm_test_on();
    int i_isr;
    //HAL_ADC_Stop(&hadc1);

    adc_stop();
    adc_lock();
    memcpy(&adc_values_raw[ADC_CHANNEL_NUMBER / 2], &adc_dma_buffer[ADC_CHANNEL_NUMBER / 2], sizeof(adc_dma_buffer) / 2);
    adc_start((uint32_t*)adc_dma_buffer, sizeof(adc_dma_buffer));

    for (i_isr = 0; i_isr < ADC_CHANNEL_NUMBER; i_isr++)
    {
        adc_values[i_isr] = adc_values_raw[i_isr];
    }
    //events_check_adc_overload(adc_values);//TODO: Add event protection

    settings_calibrations_t calibrations = settings_get_calibrations();

    /* Apply calibrations */
    for (i_isr = 0; i_isr < ADC_CHANNEL_NUMBER; i_isr++)
    {
        adc_values[i_isr] -= calibrations.offset[i_isr];
        adc_values[i_isr] *= calibrations.calibration[i_isr];

        pfc.adc.ch[current_buffer][i_isr][symbol] = adc_values[i_isr];
    }
    //events_check_overcurrent(&adc_values[ADC_I_A]);
    //events_check_overvoltage(&adc_values[ADC_U_A]);
    if (pfc_get_state() >= PFC_STATE_CHARGE && pfc_get_state() < PFC_STATE_FAULTBLOCK)
    {
        events_check_ud(adc_values[ADC_UCAP]);
    }
    //restart adc
    //HAL_ADC_Start(&hadc1);

    if (!pfc_is_pwm_on())
    {
        Ia_e_1 = 0 - adc_values[ADC_I_A];
        Ib_e_1 = 0 - adc_values[ADC_I_B];
        Ic_e_1 = 0 - adc_values[ADC_I_C];
    }
    else
    {
        settings_capacitors_t capacitors = settings_get_capacitors();
        float VL = PID(
            capacitors.Ucap_nominal - adc_get_cap_voltage(),
            &VLet_1,
            capacitors.ctrl_Ucap_Kp,
            capacitors.ctrl_Ucap_Ki,
            0,
            &VLIt_1);

        float IvlA = VL * pfc.adc.ch[last_buffer][ADC_MATH_A][symbol] / pfc.adc.active[ADC_MATH_A]; /*sin(symbol/128.0*2.0*MATH_PI)*/
        float IvlB = VL * pfc.adc.ch[last_buffer][ADC_MATH_B][symbol] / pfc.adc.active[ADC_MATH_B]; /*sin(symbol/128.0*2.0*MATH_PI+2.0*MATH_PI/3.0)*/
        float IvlC = VL * pfc.adc.ch[last_buffer][ADC_MATH_C][symbol] / pfc.adc.active[ADC_MATH_C]; /*sin(symbol/128.0*2.0*MATH_PI+4.0*MATH_PI/3.0)*/

        /*
				TODO: Check the IIR nesessity
				IIR_1ORDER(
						ia,
						adc_values[ADC_I_A],
						ia,							
						(1.0f-Kp),
						Kp
					);
				*/

        float va = PID(
            IvlA - adc_values[ADC_I_A] + pfc.adc.active[ADC_I_A],
            &Ia_e_1,
            0,
            Ki,
            0,
            &Ia_It_1);
        float vb = PID(
            IvlB - adc_values[ADC_I_B] + pfc.adc.active[ADC_I_B],
            &Ib_e_1,
            0,
            Ki,
            0,
            &Ib_It_1);
        float vc = PID(
            IvlC - adc_values[ADC_I_C] + pfc.adc.active[ADC_I_C],
            &Ic_e_1,
            0,
            Ki,
            0,
            &Ic_It_1);

        Ia_It_1 *= GLOBAL_LEAKAGE_COEFFICIENT;
        Ib_It_1 *= GLOBAL_LEAKAGE_COEFFICIENT;
        Ic_It_1 *= GLOBAL_LEAKAGE_COEFFICIENT;

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

        pfc.adc.ch[current_buffer][ADC_MATH_C_A][symbol] = va;
        pfc.adc.ch[current_buffer][ADC_MATH_C_B][symbol] = vb;
        pfc.adc.ch[current_buffer][ADC_MATH_C_C][symbol] = vc;
    }

    symbol++;
    if (symbol >= ADC_VAL_NUM)
    {
        symbol = 0;
        current_buffer ^= 1;
        last_buffer = !current_buffer;
        new_period = 1;
    }
    adc_unlock();
    gpio_pwm_test_off();
}

/**
 * @brief Calculate autocorellation
 * 
 * @param in The input buffer
 *
 * @return Autocorellation coefficient
 */
static __attribute((unused)) float auto_correlation_frequency(const float* in)
{
    int order = ADC_VAL_NUM;
    float sum, max = UMAX_INITIAL_VALUE;
    int i = 0, j = 0, maxpos = 0;
    float out[ADC_VAL_NUM] = {0};

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
    return mp / (float)ADC_VAL_NUM * (float)PERIOD_REQUIRED;
}
/*--------------------------------------------------------------
                       PUBLIC FUNCTIONS
--------------------------------------------------------------*/

/*
 * @brief Run the algorithm
 */
void algorithm_process(void)
{
    if (new_period)
    {
        adc_lock(); /* TODO: check for overlapping with interrupts */

        settings_filters_t filters = settings_get_filters();

        float K_I = (filters.K_I);
        float K_U = (filters.K_U);
        float K_Ucap = (filters.K_Ucap);
        float K_Iinv = (1 - filters.K_I);
        float K_Uinv = (1 - filters.K_U);
        float K_Ucapinv = (1 - filters.K_Ucap);

        float umax[3] = {UMAX_INITIAL_VALUE};
        float umin[3] = {UMIN_INITIAL_VALUE};

        for (int i = 0; i < ADC_VAL_NUM; i++)
        {
            /* Calculate mathematical channels */
            float Uab = pfc.adc.ch[last_buffer][ADC_EDC_B][i] - pfc.adc.ch[last_buffer][ADC_EDC_A][i];
            float Ubc = pfc.adc.ch[last_buffer][ADC_EDC_C][i] - pfc.adc.ch[last_buffer][ADC_EDC_B][i];
            float Uca = pfc.adc.ch[last_buffer][ADC_EDC_A][i] - pfc.adc.ch[last_buffer][ADC_EDC_C][i];

            float Uan = (2 * Uab + Ubc) / 3;
            float Ubn = (-Uab + Ubc) / 3;
            float Ucn = -(Uan + Ubn);

            pfc.adc.ch[last_buffer][ADC_MATH_A][i] = Uan;
            pfc.adc.ch[last_buffer][ADC_MATH_B][i] = Ubn;
            pfc.adc.ch[last_buffer][ADC_MATH_C][i] = Ucn;

            for (int i_isr = 0; i_isr < ADC_CHANNEL_NUMBER + ADC_MATH_NUMBER; i_isr++)
            {
                if (needSquare[i_isr])
                {
                    /* Calculate effective value for voltages and currents */
                    pfc.adc.sum_raw_sqr[last_buffer][i_isr] += SQUARE_F(pfc.adc.ch[last_buffer][i_isr][i]);
                }
                else
                {
                    /* Calculate mean values for steady parameters */
                    pfc.adc.sum_raw_sqr[last_buffer][i_isr] += pfc.adc.ch[last_buffer][i_isr][i];
                }
            }

            /* Apply the filtration fo U and I parameters */
            for (int i_isr = 0; i_isr < PFC_NCHAN; i_isr++)
            {
                IIR_1ORDER(
                    pfc.adc.ch[last_buffer][ADC_EDC_A + i_isr][i],
                    pfc.adc.ch[current_buffer][ADC_EDC_A + i_isr][i],
                    pfc.adc.ch[last_buffer][ADC_EDC_A + i_isr][i],
                    K_U,
                    K_Uinv);

                IIR_1ORDER(
                    pfc.adc.ch[last_buffer][ADC_I_A + i_isr][i],
                    pfc.adc.ch[current_buffer][ADC_I_A + i_isr][i],
                    pfc.adc.ch[last_buffer][ADC_I_A + i_isr][i],
                    K_I,
                    K_Iinv);

                if (umax[i_isr] < pfc.adc.ch[last_buffer][ADC_EDC_A + i_isr][i]) umax[i_isr] = pfc.adc.ch[last_buffer][ADC_EDC_A + i_isr][i];
                if (umin[i_isr] > pfc.adc.ch[last_buffer][ADC_EDC_A + i_isr][i]) umin[i_isr] = pfc.adc.ch[last_buffer][ADC_EDC_A + i_isr][i];
            }
            IIR_1ORDER(
                pfc.adc.ch[last_buffer][ADC_UCAP][i],
                pfc.adc.ch[current_buffer][ADC_UCAP][i],
                pfc.adc.ch[last_buffer][ADC_UCAP][i],
                K_Ucap,
                K_Ucapinv);
        }

        /* Calculate grid parameters */
        for (int i_isr = 0; i_isr < ADC_CHANNEL_NUMBER + ADC_MATH_NUMBER; i_isr++)
        {
            if (needSquare[i_isr])
            {
                pfc.adc.active[i_isr] = sqrt(pfc.adc.sum_raw_sqr[last_buffer][i_isr] / ((float)ADC_VAL_NUM));
            }
            else
            {
                pfc.adc.active[i_isr] = pfc.adc.sum_raw_sqr[last_buffer][i_isr] / ((float)ADC_VAL_NUM);
            }
            pfc.adc.sum_raw_sqr[last_buffer][i_isr] = 0;
        }

        /* Process oscillog data */
        //HAL_GPIO_TogglePin(GPIOD, LED_1_Pin);
        protocol_write_osc_data((float**)pfc.adc.ch[last_buffer]);

        /* Process events */
        events_check_rms_overcurrent();
        events_check_rms_voltage();
        events_check_period(pfc.period_fact);

        /* Adjust the frequency */
        /*float f=auto_correlation_frequency(pfc.adc.ch[last_buffer][ADC_EDC_A]);
					IIR_1ORDER(
							pfc.period_fact,
							f,
							pfc.period_fact,							
							(1.0f-K_filter_F),
							K_filter_F
						);
			  */
        float x = pfc.adc.ch[last_buffer][ADC_MATH_A][PFC_ACHAN];
        float x_last = x;

        float cntr = 0;  //(umax[PFC_ACHAN]-umin[PFC_ACHAN])/2;

        static float P_last = 0, P;
        for (int j = 0; j < ADC_VAL_NUM; j++)
        {
            IIR_1ORDER(
                x,
                pfc.adc.ch[last_buffer][ADC_MATH_A][j],
                x,
                (1.0f - K_filter_x),
                K_filter_x);

            if (x_last > cntr && x <= cntr)
            {
                P = ((float)j - 1.0f) + ((float)x_last - cntr) / ((float)x_last - (float)x);
                if ((P - P_last) > ((float)ADC_VAL_NUM / 4 * 3))
                {
                    float last_period = pfc.period_fact;

                    IIR_1ORDER(
                        pfc.period_fact,
                        (P - P_last) * (1.0f / (float)ADC_VAL_NUM) * pfc.period_fact,
                        pfc.period_fact,
                        (1.0f - K_filter_F),
                        K_filter_F);

                    P_last = P;

                    IIR_1ORDER(
                        diff,
                        diff,
                        (P - (float)ADC_VAL_NUM / 2),
                        (1.0f - K_filter_P),
                        K_filter_P);
                    if (diff > MAXIMUM_PERIOD_DIFF) diff = MAXIMUM_PERIOD_DIFF;
                    if (diff < -MAXIMUM_PERIOD_DIFF) diff = -MAXIMUM_PERIOD_DIFF;
                    pfc.period_fact -= diff * K_phase_shift;
                    pfc.period_delta = pfc.period_fact - last_period;
                }
            }
            x_last = x;
        }
        P_last -= ADC_VAL_NUM;

        if (pfc.period_fact > (PERIOD_REQUIRED + PERIOD_DRIFT)) pfc.period_fact = (PERIOD_REQUIRED + PERIOD_DRIFT);
        if (pfc.period_fact < (PERIOD_REQUIRED - PERIOD_DRIFT)) pfc.period_fact = (PERIOD_REQUIRED - PERIOD_DRIFT);

        /* TODO: Move coefficients to the defines */
        uint32_t arr = (200000000.0f / 2.0f / (float)ADC_VAL_NUM * (pfc.period_fact / 1000000.0f));
        timer_correct_period(arr);

        pfc_process();

        new_period = 0;
        adc_unlock();
    }
}

/*
 * @brief Get capacitors voltage
 * 
 * @return Capacitors voltage [V]
 */
float adc_get_cap_voltage(void)
{
    adc_lock();
    float ud_ret = pfc.adc.active[ADC_UCAP];
    adc_unlock();
    return ud_ret;
}

/*
 * @brief Start ADC processing
 * 
 * @return The status of the operation
 */
status_t adc_logic_start(void)
{
    adc_register_callbacks(adc_cplt_callback, adc_half_cplt_callback);

    adc_start((uint32_t*)adc_dma_buffer, sizeof(adc_dma_buffer));

    timer_start_adc_timer();

    return PFC_SUCCESS;
}

/*
 * @brief Get the complex amplitude and phase
 * @note NULL pointers can be passed to omit a variable
 * 
 * @param[out] U_50Hz The complex amplitude and phase 
 * @param[out] period_delta The period error
 */
void adc_get_complex_phase(complex_amp_t* U_50Hz, float* period_delta)
{
    adc_lock();
    if (U_50Hz) memcpy(U_50Hz, pfc.U_50Hz, sizeof(pfc.U_50Hz));
    if (period_delta) *period_delta = pfc.period_delta;
    adc_unlock();
}

/*
 * @brief Get grid parameters
 * @note NULL pointers can be passed to omit a variable
 * 
 * @param[out] U_0Hz The DC part of the U signal waveform
 * @param[out] I_0Hz The DC part of the I signal waveform
 * @param[out] U_phase The phase the U signal waveform
 * @param[out] thdu The total harmonic distortion (on U)
 * @param[out] period_fact The instantenous period value
 */
void adc_get_params(float* U_0Hz, float* I_0Hz, float* U_phase, float* thdu, float* period_fact)
{
    adc_lock();
    if (U_0Hz) memcpy(U_0Hz, pfc.U_0Hz, sizeof(pfc.U_0Hz));
    if (I_0Hz) memcpy(I_0Hz, pfc.I_0Hz, sizeof(pfc.I_0Hz));
    if (U_phase) memcpy(U_phase, pfc.U_phase, sizeof(pfc.U_phase));
    if (thdu) memcpy(thdu, pfc.thdu, sizeof(pfc.thdu));
    if (period_fact) *period_fact = pfc.period_fact;
    adc_unlock();
}

/*
 * @brief Get temperature
 * 
 * @return The temperature
 */
float adc_get_temperature(void)
{
    float ret_temp = 0;
    adc_lock();
    ret_temp = pfc.temperature;
    adc_unlock();
    return ret_temp;
}

/*
 * @brief Set temperature
 * 
 * @param temperature The temperature
 */
void adc_set_temperature(float temperature)
{
    adc_lock();
    pfc.temperature = temperature;
    adc_unlock();
}

/*
 * @brief Get instantenous (active) ADC values
 * @note NULL pointers can be passed to omit a variable
 * 
 * @active[out] active Instantenous ADC values
 */
void adc_get_active(float* active)
{
    adc_lock();
    if (active) memcpy(active, pfc.adc.active, sizeof(pfc.adc.active));
    adc_unlock();
}

/*
 * @brief Get instantenous (active) ADC values in a raw form
 * @note NULL pointers can be passed to omit a variable
 * 
 * @active[out] active_raw Instantenous ADC values in a raw form
 */
void adc_get_active_raw(float* active_raw)
{
    adc_lock();
    if (active_raw) memcpy(active_raw, pfc.adc.active_raw, sizeof(pfc.adc.active_raw));
    adc_unlock();
}

/*
 * @brief Clear accumulator values
 */
void adc_clear_accumulators(void)
{
    VLet_1 = 0;
    VLIt_1 = 0;
    Ia_e_1 = 0;
    Ib_e_1 = 0;
    Ic_e_1 = 0;
    Ia_It_1 = 0;
    Ib_It_1 = 0;
    Ic_It_1 = 0;
}
/** @} */
