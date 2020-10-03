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

#define SQUARE_F(X) ((X) * (X))

#define IIR_1ORDER(Y, Xt, Xt_1, K, Kinv) \
    Y = (Xt) * (K) + (Xt_1) * (Kinv)

/*--------------------------------------------------------------
                       PUBLIC TYPES
--------------------------------------------------------------*/

typedef struct ComplexAmpPhase
{
    float amplitude;
    float phase;
} ComplexAmpPhase;

enum
{
    ADC_UD,       //CH10
    ADC_U_A,      //CH11
    ADC_U_B,      //CH12
    ADC_U_C,      //CH13
    ADC_I_A,      //CH0
    ADC_I_B,      //CH1
    ADC_I_C,      //CH2
    ADC_I_ET,     //CH3
    ADC_I_TEMP1,  //CH5
    ADC_I_TEMP2,  //CH6
    ADC_EMS_A,    //CH14
    ADC_EMS_B,    //CH15
    ADC_EMS_C,    //CH8
    ADC_EMS_I,    //CH9
    ADC_MATH_A,
    ADC_MATH_B,
    ADC_MATH_C,
    ADC_MATH_C_A,
    ADC_MATH_C_B,
    ADC_MATH_C_C,
	  ADC_CHANNEL_FULL_COUNT
};
#define ADC_CHANNEL_NUMBER (ADC_EMS_I+1)
#define ADC_MATH_NUMBER    6

/*--------------------------------------------------------------
                       PUBLIC DATA
--------------------------------------------------------------*/

extern float VLet_1;
extern float VLIt_1;
extern float Ia_e_1;
extern float Ia_It_1;
extern float Ib_e_1;
extern float Ib_It_1;
extern float Ic_e_1;
extern float Ic_It_1;


/*--------------------------------------------------------------
                       PUBLIC FUNCTIONS
--------------------------------------------------------------*/

status_t adc_logic_start(void);
float adc_get_cap_voltage(void);
void adc_get_active(float* active);

#endif /* _ADC_LOGIC_H */
