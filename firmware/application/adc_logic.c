#include "main.h"
#include "device.h"
#include "clogic.h"
#include "events_process.h"

extern ADC_HandleTypeDef hadc1;
extern DMA_HandleTypeDef hdma_adc1;
extern TIM_HandleTypeDef htim1;

uint16_t ADC_DMA_buf[ADC_CHANNEL_NUMBER];
float adc_values[ADC_CHANNEL_NUMBER+ADC_MATH_NUMBER];
uint16_t adc_values_raw[ADC_CHANNEL_NUMBER];
uint8_t current_buffer=0;
uint8_t last_buffer=0;
uint16_t symbol=0;
uint8_t newPeriod=0;
uint8_t needSquare[]={
	0,			//CH10
	1,		//CH11
	1,		//CH12
	1,		//CH13
	0,		//CH0
	0,		//CH1
	0,		//CH2
	0,		//CH3
	0,	//CH5
	0,	//CH6
	1,	//CH14
	1,	//CH15
	1,	//CH8
	1,	//CH9
	1,
	1,
	1
};
//ADC_UD,			//CH10
//ADC_U_A,		//CH11
//ADC_U_B,		//CH12
//ADC_U_C,		//CH13
//ADC_I_A,		//CH0
//ADC_I_B,		//CH1
//ADC_I_C,		//CH2
//ADC_I_ET,		//CH3
//ADC_I_TEMP1,//CH5
//ADC_I_TEMP2,//CH6
//ADC_EMS_A,	//CH14
//ADC_EMS_B,	//CH15
//ADC_EMS_C,	//CH8
//ADC_EMS_I,	//CH9
//ADC_MATH_A,
//ADC_MATH_B,
//ADC_MATH_C,
//ADC_MATH_C_A,
//ADC_MATH_C_B,
//ADC_MATH_C_C
void HAL_ADC_ConvHalfCpltCallback(ADC_HandleTypeDef* hadc){
	memcpy(adc_values_raw,ADC_DMA_buf,sizeof(ADC_DMA_buf)/2);
}
//Расчёт ПИД регулятора
float PID(float et,float *et_1,float Kp,float Ki,float Kd,float *It_1){
	float Pt=Kp*et;
	float It=*It_1+Ki*et;
	*It_1=It/**0.999*/;//утечка
	//float Dt=Kd*(et-*et_1);
	float Ut=Pt+It/*+Dt*/;
	*et_1=et;
	return Ut;
};

float VLet_1=0;//прошлое значение ошибок
float VLIt_1=0;//прошлое значение интегральной составляющей
//float Idet_1=0;//прошлое значение ошибок
//float IdIt_1=0;//прошлое значение интегральной составляющей
//float Iqet_1=0;//прошлое значение ошибок
//float IqIt_1=0;//прошлое значение интегральной составляющей
float Ia_e_1=0;//прошлое значение ошибок
float Ia_It_1=0;//прошлое значение интегральной составляющей
float Ib_e_1=0;//прошлое значение ошибок
float Ib_It_1=0;//прошлое значение интегральной составляющей
float Ic_e_1=0;//прошлое значение ошибок
float Ic_It_1=0;//прошлое значение интегральной составляющей
float Kp=0,Ki=0.2;//Ki=0.0003;
float ia=0;
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc){
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13,GPIO_PIN_SET);
	int i_isr;
	//HAL_ADC_Stop(&hadc1);
	HAL_ADC_Stop_DMA(&hadc1);
	memcpy(&adc_values_raw[8],&ADC_DMA_buf[8],sizeof(ADC_DMA_buf)/2);
	HAL_ADC_Start_DMA(&hadc1,(uint32_t*)ADC_DMA_buf,14);
	
	for (i_isr = 0; i_isr < ADC_CHANNEL_NUMBER; i_isr++) {
		adc_values[i_isr]=adc_values_raw[i_isr];
	}
	//events_was_CT_overload(adc_values);//TODO:
	//=======================================================
	//=======================================================
	/* Применяем смещения каналов */
	for (i_isr = 0; i_isr < ADC_CHANNEL_NUMBER; i_isr++){
		/* Каналы переменного тока/напряжения смещаем на UREF */
		adc_values[i_isr] -= KKM.settings.CALIBRATIONS.offset[i_isr];
		adc_values[i_isr] *= KKM.settings.CALIBRATIONS.calibration[i_isr];
				//=======================================================
		KKM.adc.ch[current_buffer][i_isr][symbol]=adc_values[i_isr];
	}
	//=======================================================
		//events_check_overcurrent_peak(&adc_values[ADC_I_A]);
		//events_check_overvoltage_transient(&adc_values[ADC_U_A]);
		if(KKM.status>=KKM_STATE_CHARGE && KKM.status<KKM_STATE_FAULTBLOCK){
			events_check_Ud(adc_values[ADC_UD]);
		}
			//restart adc
	//HAL_ADC_Start(&hadc1);

	//=======================================================
	//=======================================================
	if(!PWMon){
		Ia_e_1=0-adc_values[ADC_I_A];
		Ib_e_1=0-adc_values[ADC_I_B];
		Ic_e_1=0-adc_values[ADC_I_C];
		ia=0;
	}else{
		float VL= PID(
			KKM.settings.CAPACITORS.Ud_nominal-UD,
			&VLet_1,
			KKM.settings.CAPACITORS.ctrlUd_Kp,
			KKM.settings.CAPACITORS.ctrlUd_Ki,
			0,
			&VLIt_1
			);

		float IvlA=VL*KKM.adc.ch[last_buffer][ADC_MATH_A][symbol]/KKM.adc.active[ADC_MATH_A];/*sin(symbol/128.0*2.0*MATH_PI)*/;
		float IvlB=VL*KKM.adc.ch[last_buffer][ADC_MATH_B][symbol]/KKM.adc.active[ADC_MATH_B];/*sin(symbol/128.0*2.0*MATH_PI+2.0*MATH_PI/3.0)*/;
		float IvlC=VL*KKM.adc.ch[last_buffer][ADC_MATH_C][symbol]/KKM.adc.active[ADC_MATH_C];/*sin(symbol/128.0*2.0*MATH_PI+4.0*MATH_PI/3.0)*/;
		/*
		IIR_1ORDER(
				ia,
				adc_values[ADC_I_A],
				ia,							
				(1.0f-Kp),
				Kp
			);*/
		
		float va = PID(
			IvlA-adc_values[ADC_I_A]+KKM.adc.active[ADC_I_A],
			&Ia_e_1,
			0,
			Ki,
			0,
			&Ia_It_1
			);
		float vb = PID(
			IvlB-adc_values[ADC_I_B]+KKM.adc.active[ADC_I_B],
			&Ib_e_1,
			0,
			Ki,
			0,
			&Ib_It_1
			);
		float vc = PID(
			IvlC-adc_values[ADC_I_C]+KKM.adc.active[ADC_I_C],
			&Ic_e_1,
			0,
			Ki,
			0,
			&Ic_It_1
			);
		Ia_It_1*=0.995;
		Ib_It_1*=0.995;
		Ic_It_1*=0.995;
		
		if(Ia_It_1>(1.0f))Ia_It_1=1.0f;
		if(Ib_It_1>(1.0f))Ib_It_1=1.0f;
		if(Ic_It_1>(1.0f))Ic_It_1=1.0f;			
		if(Ia_It_1<(-1.0f))Ia_It_1=-1.0f;
		if(Ib_It_1<(-1.0f))Ib_It_1=-1.0f;
		if(Ic_It_1<(-1.0f))Ic_It_1=-1.0f;
		
		#define EPS 0
		if(va>(1.0f-EPS))va=1.0f-EPS;
		if(vb>(1.0f-EPS))vb=1.0f-EPS;
		if(vc>(1.0f-EPS))vc=1.0f-EPS;			
		if(va<(-1.0f+EPS))va=-1.0f+EPS;
		if(vb<(-1.0f+EPS))vb=-1.0f+EPS;
		if(vc<(-1.0f+EPS))vc=-1.0f+EPS;
		
		TIM1->CCR1 = (float)PWM_PERIOD*(-va*0.5f+0.5f);
		TIM1->CCR2 = (float)PWM_PERIOD*(-vb*0.5f+0.5f);
		TIM1->CCR3 = (float)PWM_PERIOD*(-vc*0.5f+0.5f);
		
		KKM.adc.ch[current_buffer][ADC_MATH_C_A][symbol]=va;
		KKM.adc.ch[current_buffer][ADC_MATH_C_B][symbol]=vb;
		KKM.adc.ch[current_buffer][ADC_MATH_C_C][symbol]=vc;
	}
	//=======================================================
	symbol++;
	static int pulse=0;
	if(symbol>=ADC_VAL_NUM){
		symbol=0;
		current_buffer^=1;
		last_buffer=!current_buffer;
		newPeriod=1;
		
		if(KKM.status==KKM_STATE_CHARGE){
			pulse++;
			if(pulse>=4){
				pulse=0;
				KKM.status=KKM_STATE_WORK;
			}
		}
	}
	//=======================================================
	//=======================================================
	//=======================================================
HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13,GPIO_PIN_RESET);
}


////arm_park_f32(Iabc,wt)
//			float Ia=adc_values[ADC_I_A];
//			float Ib=adc_values[ADC_I_B];
//			float Ic=adc_values[ADC_I_C];
//			float wt=(float)symbol/128.0*2.0*MATH_PI;

//      float Ialpha =  Ia*(1*2/3)+ 
//											Ib*(-0.5*2/3)+ 
//											Ic*(-0.5*2/3);
//      float Ibeta  =  Ia*0+
//											Ib*(sqrt(3)/2*2/3)+
//											Ic*(-sqrt(3)/2*2/3);
//      
//      float cosVal=cos(wt-MATH_PI/2);
//      float sinVal=sin(wt-MATH_PI/2);
//      
//      // Calculate pId using the equation, pId = Ialpha * cosVal + Ibeta * sinVal
//      float Id = Ialpha * cosVal + Ibeta * sinVal;

//      // Calculate pIq using the equation, pIq = - Ialpha * sinVal + Ibeta * cosVal
//      float Iq = -Ialpha * sinVal + Ibeta * cosVal;
////=============*************==========================================
//			float VL=PID(
//				KKM.settings.CAPACITORS.Ud_nominal-adc_values[ADC_UD],//текущая ошибка
//				&VLet_1,//прошлая ошибка
//				0,//Kp
//				0,//Ki
//				0,//Kd
//				&VLIt_1//прошлая интегральная составляющая
//			);
//			Id=PID(
//				VL-Iq,//текущая ошибка
//				&Idet_1,//прошлая ошибка
//				0.1,//Kp
//				0.01,//Ki
//				0,//Kd
//				&IdIt_1//прошлая интегральная составляющая
//			)*(-2.0);
//			Iq=PID(
//				0-Iq,//текущая ошибка
//				&Iqet_1,//прошлая ошибка
//				0.1,//Kp
//				0.01,//Ki
//				0,//Kd
//				&IqIt_1//прошлая интегральная составляющая
//			)*(-2.0);
////=============*************==========================================
////arm_inv_park_f32(Id,Iq,wt)

////Calculate pIalpha using the equation, pIalpha = Id * cosVal - Iq * sinVal 
//    Ialpha = Id * cosVal - Iq * sinVal;
//  
//// Calculate pIbeta using the equation, pIbeta = Id * sinVal + Iq * cosVal 
//    Ibeta  = Id * sinVal + Iq * cosVal;
//    
//    Ia=Ialpha*(1)      +   Ibeta*(0);
//    Ib=Ialpha*(-0.5)   +   Ibeta*(sqrt(3)/2);
//    Ic=Ialpha*(-0.5)   +   Ibeta*(-sqrt(3)/2);
//		
//		float va=Ia;
//		float vb=Ib;
//		float vc=Ic;
//		
//			#define EPS 0.01f
//			
//			if(va>(1.0f-EPS))va=1.0f-EPS;
//			if(vb>(1.0f-EPS))vb=1.0f-EPS;
//			if(vc>(1.0f-EPS))vc=1.0f-EPS;
//			
//			if(va<(-1.0f+EPS))va=-1.0f+EPS;
//			if(vb<(-1.0f+EPS))vb=-1.0f+EPS;
//			if(vc<(-1.0f+EPS))vc=-1.0f+EPS;
//			
//		/*
//		float va=KKM.adc.ch[last_buffer][ADC_MATH_C_A][symbol];
//		float vb=KKM.adc.ch[last_buffer][ADC_MATH_C_B][symbol];
//		float vc=KKM.adc.ch[last_buffer][ADC_MATH_C_C][symbol];
//		if(va>1 || va<-1){
//			TIM1->CCER &=~ (TIM_CCER_CC1E);
//			TIM1->CCER &=~ (TIM_CCER_CC1NE);
//		}
//		else{
//			TIM1->CCER |= (TIM_CCER_CC1E);
//			TIM1->CCER |= (TIM_CCER_CC1NE);
//		}
//		if(vb>1 || vb<-1){
//			TIM1->CCER &=~ (TIM_CCER_CC2E);
//			TIM1->CCER &=~ (TIM_CCER_CC2NE);
//		}
//		else{
//			TIM1->CCER |= (TIM_CCER_CC2E);
//			TIM1->CCER |= (TIM_CCER_CC2NE);
//		}
//		if(vc>1 || vc<-1){
//			TIM1->CCER &=~ (TIM_CCER_CC3E);
//			TIM1->CCER &=~ (TIM_CCER_CC3NE);
//		}
//		else{
//			TIM1->CCER |= (TIM_CCER_CC3E);
//			TIM1->CCER |= (TIM_CCER_CC3NE);
//		}*/
//		
