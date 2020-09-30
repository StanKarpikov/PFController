#include "device.h"
#include "settings.h"
#include "clogic.h"
#include "main.h"
#include "events_process.h"
#include "algorithm.h"
float KphaseS=-1.0f;
#define CENTER 0.0f
float K_filter_x=0.5f;
float K_filter_F=0.8f;
float K_filter_P=0.3f;
float phase=(float)ADC_VAL_NUM/2.0f;
float diff=0;
int addPhase=0;

float autoCorrelationFreq(const float *in){
		int order=ADC_VAL_NUM;
    float sum,max=-1000000;
    int i,j,maxpos;
		float out[ADC_VAL_NUM];

    for (i=0;i<order;i++) {
        sum=0;
        for (j=0;j<order-i;j++) {
            sum+=in[j]*in[j+i];
        }
        out[i]=sum;
				if(sum>max && i){
					max=sum;
					maxpos=i;
				}
    }
		float dy=0.5*(out[maxpos-1]-out[maxpos+1]);
		float d2y=2.0*max-out[maxpos-1]-out[maxpos+1];
		float mp=maxpos+dy/d2y;
		return mp/128.0*20000.0;
}

void algorithm_work(void){
	if(newPeriod){
		//---------------------------------------
		float K_I	 			= (KKM.settings.FILTERS.K_I);
		float K_U	 			= (KKM.settings.FILTERS.K_U);
		float K_UD	 		= (KKM.settings.FILTERS.K_UD);
		float K_Iinv	 	= (1-KKM.settings.FILTERS.K_I);
		float K_Uinv	 	= (1-KKM.settings.FILTERS.K_U);
		float K_UDinv 	= (1-KKM.settings.FILTERS.K_UD);
								
		float umax[3]={-1000000};	
		float umin[3]={1000000};
		
		for(int i=0;i<ADC_VAL_NUM;i++){
			//------------- Math channels -------------
			float Uab = KKM.adc.ch[last_buffer][ADC_EMS_B][i]-KKM.adc.ch[last_buffer][ADC_EMS_A][i];
			float Ubc = KKM.adc.ch[last_buffer][ADC_EMS_C][i]-KKM.adc.ch[last_buffer][ADC_EMS_B][i];
			float Uca = KKM.adc.ch[last_buffer][ADC_EMS_A][i]-KKM.adc.ch[last_buffer][ADC_EMS_C][i];
			
			float Uan = (2*Uab+Ubc)/3;
			float Ubn = (-Uab+Ubc)/3;
			float Ucn = -(Uan+Ubn);
			
			KKM.adc.ch[last_buffer][ADC_MATH_A][i]=Uan;
			KKM.adc.ch[last_buffer][ADC_MATH_B][i]=Ubn;
			KKM.adc.ch[last_buffer][ADC_MATH_C][i]=Ucn;
			//---------------------------------------
			for (int i_isr = 0; i_isr < ADC_CHANNEL_NUMBER+ADC_MATH_NUMBER; i_isr++) {
				if (needSquare[i_isr]){
					KKM.adc.sum_raw_sqr[last_buffer][i_isr] += SQUARE_F(KKM.adc.ch[last_buffer][i_isr][i]); // для переменных токов/напряжений считаем действующее 
				}else{
					KKM.adc.sum_raw_sqr[last_buffer][i_isr] += KKM.adc.ch[last_buffer][i_isr][i]; // для постоянных величин - считаем среднее 
				}
			}
			//------------- Filter U and I ----------	
			for(int i_isr=0;i_isr<KKM_NCHAN;i_isr++){
				IIR_1ORDER(
						KKM.adc.ch[last_buffer][ADC_EMS_A+i_isr][i],
						KKM.adc.ch[current_buffer][ADC_EMS_A+i_isr][i],
						KKM.adc.ch[last_buffer][ADC_EMS_A+i_isr][i],
						K_U,
						K_Uinv);
				
				IIR_1ORDER(
						KKM.adc.ch[last_buffer][ADC_I_A+i_isr][i],
						KKM.adc.ch[current_buffer][ADC_I_A+i_isr][i],
						KKM.adc.ch[last_buffer][ADC_I_A+i_isr][i],
						K_I,
						K_Iinv);
				
				
				if(umax[i_isr]<KKM.adc.ch[last_buffer][ADC_EMS_A+i_isr][i])umax[i_isr]=KKM.adc.ch[last_buffer][ADC_EMS_A+i_isr][i];
				if(umin[i_isr]>KKM.adc.ch[last_buffer][ADC_EMS_A+i_isr][i])umin[i_isr]=KKM.adc.ch[last_buffer][ADC_EMS_A+i_isr][i];
			}
			IIR_1ORDER(
				KKM.adc.ch[last_buffer][ADC_UD][i],
				KKM.adc.ch[current_buffer][ADC_UD][i],
				KKM.adc.ch[last_buffer][ADC_UD][i],
				K_UD,
				K_UDinv);
		}
		//---------- Calculate STD -------------------------
			for (int i_isr = 0; i_isr < ADC_CHANNEL_NUMBER+ADC_MATH_NUMBER; i_isr++) {
				if (needSquare[i_isr]){
					KKM.adc.active[i_isr]=sqrt(KKM.adc.sum_raw_sqr[last_buffer][i_isr]/((float)ADC_VAL_NUM));	
				}else{
					KKM.adc.active[i_isr]=KKM.adc.sum_raw_sqr[last_buffer][i_isr]/((float)ADC_VAL_NUM);
				}
				KKM.adc.sum_raw_sqr[last_buffer][i_isr]=0;
			}
		//------------------------------------
		for(int i=0;i<ADC_VAL_NUM;i++){
			/*int j=(i+addPhase)&127;
			#define L (250e-6)
			#define R (0.07f)
			#define w (2.0f*MATH_PI*50.0f)
			float Th=(float)j/128.0f*(2.0f*MATH_PI);
			float Ud=UD;
			float Ua=//sin(Th)*sqrt(2.0f)*KKM.adc.active[ADC_EMS_A];
				KKM.adc.ch[last_buffer][ADC_MATH_A][j];
			float Ub=//sin(Th+2.0f*MATH_PI/3.0f)*sqrt(2.0f)*KKM.adc.active[ADC_EMS_B];
				KKM.adc.ch[last_buffer][ADC_MATH_B][j];
			if(Ud==0)Ud=1e-6;
			
			float VL=(KKM.settings.CAPACITORS.Ud_nominal-Ud)*KKM.settings.CAPACITORS.ctrlUd_Kp;
			if(VL>50)VL=50;
			if(VL<-50)VL=-50;
			
			float va=2.0f/Ud*(Ua-VL*(R/w/L*sin(Th)+cos(Th)));
			float vb=2.0f/Ud*(Ub-VL*(R/w/L*sin(Th+2.0f*MATH_PI/3.0f)+cos(Th+2.0f*MATH_PI/3.0f)));
			float vc=-va-vb;*/
			/*
			#define EPS 0.01f
			
			if(va>(1.0f-EPS))va=1.0f-EPS;
			if(vb>(1.0f-EPS))vb=1.0f-EPS;
			if(vc>(1.0f-EPS))vc=1.0f-EPS;
			
			if(va<(-1.0f+EPS))va=-1.0f+EPS;
			if(vb<(-1.0f+EPS))vb=-1.0f+EPS;
			if(vc<(-1.0f+EPS))vc=-1.0f+EPS;*/
			
			//KKM.adc.ch[current_buffer][ADC_MATH_C_A][i]=-va;
			//KKM.adc.ch[current_buffer][ADC_MATH_C_B][i]=-vb;
			//KKM.adc.ch[current_buffer][ADC_MATH_C_C][i]=-vc;
		}
		//---------------------------------------
		  //HAL_GPIO_TogglePin(GPIOD, LED_1_Pin);
			for(int i=0;i<KKM_NCHAN;i++){
				memcpy(OSC_DATA[OSC_U_A+i],KKM.adc.ch[last_buffer][ADC_MATH_A+i],sizeof(OSC_DATA[OSC_U_A+i]));
				memcpy(OSC_DATA[OSC_I_A+i],KKM.adc.ch[last_buffer][ADC_I_A+i],sizeof(OSC_DATA[OSC_I_A+i]));
				memcpy(OSC_DATA[OSC_COMP_A+i],KKM.adc.ch[last_buffer][ADC_MATH_C_A+i],sizeof(OSC_DATA[OSC_COMP_A+i]));
			}
			memcpy(OSC_DATA[OSC_UD],KKM.adc.ch[last_buffer][ADC_UD],sizeof(OSC_DATA[OSC_UD]));
			//---------------------------------------
			events_check_overcurrent_rms();
			events_check_voltage_RMS();
			events_check_period(KKM.period_fact);
			//------------FREQ-----------------------
			/*float f=autoCorrelationFreq(KKM.adc.ch[last_buffer][ADC_EMS_A]);
					IIR_1ORDER(
							KKM.period_fact,
							f,
							KKM.period_fact,							
							(1.0f-K_filter_F),
							K_filter_F
						);
			*/
			float x=KKM.adc.ch[last_buffer][ADC_MATH_A][0];
			float x_last=x;
			
			
			float cntr=0;//(umax[0]-umin[0])/2;
			
			static float Plast=0,P;
			for(int j=0;j<ADC_VAL_NUM;j++){
				
				IIR_1ORDER(
							x,
							KKM.adc.ch[last_buffer][ADC_MATH_A][j],
							x,
							(1.0f-K_filter_x),
							K_filter_x
						);
				
				if(x_last>cntr && x<=cntr){
					P=((float)j-1.0f)+((float)x_last-cntr)/((float)x_last-(float)x);
					if((P-Plast)>(128/4*3)){
						
						float last_period=KKM.period_fact;
						
						IIR_1ORDER(
							KKM.period_fact,
							(P-Plast)*(1.0f/128.0f)*KKM.period_fact,
							KKM.period_fact,							
							(1.0f-K_filter_F),
							K_filter_F
						);
						
						Plast=P;

							IIR_1ORDER(
								diff,
								diff,
								(P-64.0f),							
								(1.0f-K_filter_P),
								K_filter_P
							);						
						if(diff>10)diff=10;
						if(diff<-10)diff=-10;
						KKM.period_fact-=diff*KphaseS;						
						KKM.period_delta=KKM.period_fact-last_period;
					}
				}
				x_last=x;
			}
			Plast-=ADC_VAL_NUM;
			//---------------------
			if(KKM.period_fact>(20000+1000))KKM.period_fact=(20000+1000);
			if(KKM.period_fact<(20000-1000))KKM.period_fact=(20000-1000);
			__disable_irq();
			TIM2->ARR=(200000000.0f/2.0f/128.0f*(KKM.period_fact/1000000.0f));
			TIM2->EGR = TIM_EGR_UG;
			__enable_irq();
			//---------------------
			clogic_do();
			//---------------------
			//---------------------
			static uint32_t lastprint=0;
			if((HAL_GetTick()-lastprint)>500){
				lastprint=HAL_GetTick();
				//HAL_GPIO_TogglePin(GPIOD, LED_3_Pin);
				//HAL_GPIO_TogglePin(RE_485_GPIO_Port, RE_485_Pin);
				//printf("\n\nKKM program, %s %s\n",__DATE__,__TIME__);
			}
			//---------------------
			newPeriod=0;
		}
}
