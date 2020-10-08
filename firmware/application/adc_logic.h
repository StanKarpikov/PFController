/**
 * @file adc_logic.h
 * @author Stanislav Karpikov
 * @brief ADC support and control module (header)
 */

#ifndef _ADC_LOGIC_H
#define _ADC_LOGIC_H

/*--------------------------------------------------------------
                       INCLUDES
--------------------------------------------------------------*/

#include "BSP/debug.h"
#include "stdint.h"

/*--------------------------------------------------------------
                       PUBLIC MACRO
--------------------------------------------------------------*/

/** 
 * @brief Calculate the square in place  
 *
 * @param X The value
 * 
 * @return X^2
 */
#define SQUARE_F(X) ((X) * (X))

/** 
 * @brief Apply the first order digital filter (leaky integrator)
 *
 * @param Y[out] The filtered value
 * @param Xt[in] The value at a current step
 * @param Xt_1[in] The accmulated value
 * @param K[in] The filter coefficient
 * @param Kinv[in] The inverted coefficient (1-K)
 */
#define IIR_1ORDER(Y, Xt, Xt_1, K, Kinv) \
    Y = (Xt) * (K) + (Xt_1) * (Kinv)

/*--------------------------------------------------------------
                       PUBLIC TYPES
--------------------------------------------------------------*/

/** Complex apmlitude */
typedef struct
{
    float amplitude;
    float phase;
} complex_amp_t;

/** 
 * @brief ADC channels (include math clannels) 
 *
 * @note Because no neutral connection is used, voltages should be processed before using. 
 * The mathematical channels are used for this purpose.
 */
enum
{
    ADC_UD,       /**< CH10 Voltage at the capacitors */
    ADC_U_A,      /**< CH11 Voltage U, phase A */
    ADC_U_B,      /**< CH12 Voltage U, phase B */
    ADC_U_C,      /**< CH13 Voltage U, phase C */
    ADC_I_A,      /**< CH0 Current I, phase A */
    ADC_I_B,      /**< CH1 Current I, phase B */
    ADC_I_C,      /**< CH2 Current I, phase C */
    ADC_I_ET,     /**< CH3 Current I, load */
    ADC_I_TEMP1,  /**< CH5 */
    ADC_I_TEMP2,  /**< CH6 */
    ADC_EMS_A,    /**< CH14 EMS A */
    ADC_EMS_B,    /**< CH15 EMS B */
    ADC_EMS_C,    /**< CH8 EMS C */
    ADC_EMS_I,    /**< CH9 EMS I */
    ADC_MATH_A,    /**< MATH channel A */
    ADC_MATH_B,    /**< MATH channel B */
    ADC_MATH_C,    /**< MATH channel C */
    ADC_MATH_C_A,    /**< MATH channel CA */
    ADC_MATH_C_B,    /**< MATH channel CB */
    ADC_MATH_C_C,    /**< MATH channel CC */
	  ADC_CHANNEL_FULL_COUNT /**< ADC channels count */
};

#define ADC_CHANNEL_NUMBER (ADC_EMS_I+1) /**< The number of ADC physical channels */
#define ADC_MATH_NUMBER    (6) /**< The number of ADC mathematical channels */

/*--------------------------------------------------------------
                       PUBLIC FUNCTIONS
--------------------------------------------------------------*/

/**
 * @brief Start ADC processing
 * 
 * @return The status of the operation
 */
status_t adc_logic_start(void);

/**
 * @brief Get capacitors voltage
 * 
 * @return Capacitors voltage [V]
 */
float adc_get_cap_voltage(void);

/**
 * @brief Get instantenous (active) ADC values
 * @note NULL pointers can be passed to omit a variable
 * 
 * @active[out] active Instantenous ADC values
 */
void adc_get_active(float* active);

/**
 * @brief Get instantenous (active) ADC values in a raw form
 * @note NULL pointers can be passed to omit a variable
 * 
 * @active[out] active_raw Instantenous ADC values in a raw form
 */
void adc_get_active_raw(float* active_raw);

/**
 * @brief Get the complex amplitude and phase
 * @note NULL pointers can be passed to omit a variable
 * 
 * @param[out] U_50Hz The complex amplitude and phase 
 * @param[out] period_delta The period error
 */
void adc_get_complex_phase(complex_amp_t* U_50Hz, float* period_delta);

/**
 * @brief Get grid parameters
 * @note NULL pointers can be passed to omit a variable
 * 
 * @param[out] U_0Hz The DC part of the U signal waveform
 * @param[out] I_0Hz The DC part of the I signal waveform
 * @param[out] U_phase The phase the U signal waveform
 * @param[out] thdu The total harmonic distortion (on U)
 * @param[out] period_fact The instantenous period value
 */
void adc_get_params(float* U_0Hz, float* I_0Hz, float* U_phase, float* thdu, float* period_fact);

/**
 * @brief Get temperature
 * 
 * @return THe temperature
 */
float adc_get_temperature(void);

/**
 * @brief Set temperature
 * 
 * @param temperature The temperature
 */
void adc_set_temperature(float temperature);

/**
 * @brief Run the algorithm
 */
void algorithm_process(void);

/**
 * @brief Clear accumulator values
 */
void adc_clear_accumulators(void);

#endif /* _ADC_LOGIC_H */
