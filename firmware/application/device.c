#include "device.h"
#include "settings.h"
#include "clogic.h"
#include "main.h"
//================================================================================
//================================================================================
//================================================================================
uint64_t currentTime=0;
struct _kkm KKM;
float OSC_DATA[OSC_CHANNEL_NUMBER][OSCILLOG_TRANSFER_SIZE];

void kkm_switch_on(void) {
	if(KKM.status == KKM_STATE_STOP){
		clogic_set_state(KKM_STATE_SYNC);
	}else if(KKM.status == KKM_STATE_FAULTBLOCK){
		clogic_set_state(KKM_STATE_SYNC);
	}
}

void kkm_switch_off(void) {
	clogic_set_state(KKM_STATE_STOPPING);
}
//================================================================================
//================================================================================
//================================================================================
static prot_handler pr_handlers[lenP_AFG_COMMANDS];

#define HANDLER(COMM) \
		void protocol_command_ ## COMM(PROTOCOL_CONTEXT *pc){ \
			struct sCOMMAND_ ## COMM *req; \
			struct sANSWER_ ##  COMM *pack; \
			UNUSED(req); \
			UNUSED(pack); \
			req  =(struct sCOMMAND_ ## COMM *)(pc->receivedPackage.data+MIN_PACKET_LEN); \
			pack =(struct sANSWER_  ## COMM *)(pc->packageToSend.data+MIN_PACKET_LEN); \
			pc->packageToSend.fields.len=sizeof(struct sANSWER_ ## COMM); \
			pc->packageToSend.fields.status.raw=0; \
			pc->packageToSend.fields.command=P_AFG_COMMAND_ ## COMM; 

#define END_HANDLER(COMM) \
			/*memcpy(pc->packageToSend.data,&pack,sizeof(pack)); */\
			package_set_data_len(&pc->packageToSend,sizeof(struct sANSWER_ ## COMM)); \
			protocol_send_package(pc);\
		}
//================================================================================
enum{
	COMMAND_WORK_ON=1,
	COMMAND_WORK_OFF,
	COMMAND_CHARGE_ON,
	COMMAND_CHARGE_OFF,
	COMMAND_CHANNEL0_DATA,
	COMMAND_CHANNEL1_DATA,
	COMMAND_CHANNEL2_DATA,
	COMMAND_SETTINGS_SAVE
};

HANDLER(SWITCH_ON_OFF)
	pack->result=1;
	switch(req->command){
		case COMMAND_WORK_ON:
			if (KKM.status == KKM_STATE_STOP){
				kkm_switch_on();
			}
			break;
		case COMMAND_WORK_OFF:
			kkm_switch_off();
		break;
		case COMMAND_CHARGE_ON:
			if (KKM.status == KKM_STATE_WORK){
				clogic_set_state(KKM_STATE_CHARGE);
			}
			break;
		case COMMAND_CHARGE_OFF:
			if (KKM.status == KKM_STATE_CHARGE){
				clogic_set_state(KKM_STATE_WORK);
			}
		break;
		case COMMAND_SETTINGS_SAVE:
			if(KKM.status==KKM_STATE_STOP){
				SaveSettings(&KKM.settings);
			}
			break;
		case COMMAND_CHANNEL0_DATA:
			KKM.settings.PWM.activeChannels[0]=req->data;
			break;
		case COMMAND_CHANNEL1_DATA:
			KKM.settings.PWM.activeChannels[1]=req->data;
			break;
		case COMMAND_CHANNEL2_DATA:
			KKM.settings.PWM.activeChannels[2]=req->data;
			break;
		default:
			pack->result=0;
			break;
	}
END_HANDLER(SWITCH_ON_OFF)
//================================================================================
HANDLER(GET_ADC_ACTIVE)

	pack->ADC_UD			=KKM.adc.active[ADC_UD];
	pack->ADC_U_A			=KKM.adc.active[ADC_EMS_A];//TODO:
	pack->ADC_U_B			=KKM.adc.active[ADC_EMS_B];
	pack->ADC_U_C			=KKM.adc.active[ADC_EMS_C];
	pack->ADC_I_A			=KKM.adc.active[ADC_I_A];
	pack->ADC_I_B			=KKM.adc.active[ADC_I_B];
	pack->ADC_I_C			=KKM.adc.active[ADC_I_C];
	pack->ADC_I_ET		=KKM.adc.active[ADC_I_ET];
	pack->ADC_I_TEMP1	=KKM.adc.active[ADC_I_TEMP1];
	pack->ADC_I_TEMP2	=KKM.adc.active[ADC_I_TEMP2];
	pack->ADC_EMS_A		=KKM.adc.active[ADC_EMS_A];
	pack->ADC_EMS_B		=KKM.adc.active[ADC_EMS_B];
	pack->ADC_EMS_C		=KKM.adc.active[ADC_EMS_C];
	pack->ADC_EMS_I		=KKM.adc.active[ADC_EMS_I];
	pack->ADC_MATH_A	=KKM.adc.active[ADC_EMS_A];//TODO:
	pack->ADC_MATH_B	=KKM.adc.active[ADC_EMS_B];
	pack->ADC_MATH_C	=KKM.adc.active[ADC_EMS_C];	
END_HANDLER(GET_ADC_ACTIVE)

//================================================================================
HANDLER(GET_NET_PARAMS)
	pack->period_fact=KKM.period_fact;
	pack->U0Hz_A=KKM.U_0Hz[0];//Постоянная составляющая
	pack->U0Hz_B=KKM.U_0Hz[1];
	pack->U0Hz_C=KKM.U_0Hz[2];
	pack->I0Hz_A=KKM.I_0Hz[0];//Постоянная составляющая
	pack->I0Hz_B=KKM.I_0Hz[1];
	pack->I0Hz_C=KKM.I_0Hz[2];
	
	pack->thdu_A=KKM.thdu[0];
	pack->thdu_B=KKM.thdu[1];
	pack->thdu_C=KKM.thdu[2];

	pack->U_phase_A=KKM.U_phase[0];
	pack->U_phase_B=KKM.U_phase[1];
	pack->U_phase_C=KKM.U_phase[2];
END_HANDLER(GET_NET_PARAMS)
//================================================================================
HANDLER(GET_ADC_ACTIVE_RAW)
	pack->ADC_UD		=KKM.adc.active_raw[ADC_UD];
	pack->ADC_U_A		=KKM.adc.active_raw[ADC_U_A];
	pack->ADC_U_B		=KKM.adc.active_raw[ADC_U_B];
	pack->ADC_U_C		=KKM.adc.active_raw[ADC_U_C];
	pack->ADC_I_A		=KKM.adc.active_raw[ADC_I_A];
	pack->ADC_I_B		=KKM.adc.active_raw[ADC_I_B];
	pack->ADC_I_C		=KKM.adc.active_raw[ADC_I_C];
	pack->ADC_I_ET	=KKM.adc.active_raw[ADC_I_ET];
	pack->ADC_I_TEMP1	=KKM.adc.active_raw[ADC_I_TEMP1];
	pack->ADC_I_TEMP2	=KKM.adc.active_raw[ADC_I_TEMP2];
	pack->ADC_EMS_A	=KKM.adc.active_raw[ADC_EMS_A];
	pack->ADC_EMS_B	=KKM.adc.active_raw[ADC_EMS_B];
	pack->ADC_EMS_C	=KKM.adc.active_raw[ADC_EMS_C];
	pack->ADC_EMS_I	=KKM.adc.active_raw[ADC_EMS_I];	
END_HANDLER(GET_ADC_ACTIVE_RAW)
//================================================================================
HANDLER(GET_WORK_STATE)
	DINT;
	currentTime=req->currentTime;
	if(currentTime>4133894400000){
		currentTime=0;
	}
	EINT;
	pack->state=KKM.status;
	pack->activeChannels[0]=KKM.settings.PWM.activeChannels[0];
	pack->activeChannels[1]=KKM.settings.PWM.activeChannels[1];
	pack->activeChannels[2]=KKM.settings.PWM.activeChannels[2];
END_HANDLER(GET_WORK_STATE)
//================================================================================
#include "fw_ver.h"
HANDLER(GET_VERSION_INFO)
	pack->major=FW_VERSION_MAJOR;
	pack->minor=FW_VERSION_MINOR;
	pack->micro=FW_VERSION_MICRO;
	pack->build=FW_VERSION_BUILD;

	uint16_t DD=0;
	if(__DATE__[4]>='0')DD=(__DATE__[4]-'0');
	DD=DD*10+(__DATE__[5]-'0');
	uint16_t MM=BUILD_MONTH_CH1;
	uint16_t YYYY=(__DATE__[7]-'0')*1000+(__DATE__[8]-'0')*100+(__DATE__[9]-'0')*10+(__DATE__[10]-'0');
	uint16_t HH=(__TIME__[0]-'0')*10+(__TIME__[1]-'0');
	uint16_t MN=(__TIME__[3]-'0')*10+(__TIME__[4]-'0');
	uint16_t SS=(__TIME__[6]-'0')*10+(__TIME__[7]-'0');

	pack->day=DD;
	pack->month=MM;
	pack->year=YYYY;
	pack->hour=HH;
	pack->minute=MN;
	pack->second=SS;

END_HANDLER(GET_VERSION_INFO)
//================================================================================
HANDLER(GET_OSCILOG)	
	if(req->num>OSC_CHANNEL_NUMBER){
		protocol_error_handle(pc, package_get_command(&pc->receivedPackage));
		return;
	}
	pack->ch	= req->num;
	pack->len	= OSCILLOG_TRANSFER_SIZE;

	float oscillog_max=-1e10;
	float oscillog_min=1e10;
	for(int i=0;i<OSCILLOG_TRANSFER_SIZE;i++){
		if(OSC_DATA[req->num][i]>oscillog_max)oscillog_max=OSC_DATA[req->num][i];
		if(OSC_DATA[req->num][i]<oscillog_min)oscillog_min=OSC_DATA[req->num][i];
	}
	float astep=0.0;
	if((oscillog_max-oscillog_min)==0){
		astep=255.0;
	}else{
		astep=255.0/(oscillog_max-oscillog_min);
	}
	for(int i=0;i<OSCILLOG_TRANSFER_SIZE;i++){
		pack->data[i]=(OSC_DATA[req->num][i]-oscillog_min)*astep;
	}
	pack->max	= oscillog_max;
	pack->min	= oscillog_min;

END_HANDLER(GET_OSCILOG)
//================================================================================
HANDLER(GET_SETTINGS_CALIBRATIONS)
	int i;
	for(i=0;i<ADC_CHANNEL_NUMBER;i++){
		pack->calibration[i] = KKM.settings.CALIBRATIONS.calibration[i]; //!< Калибровки для каналов
		pack->offset[i]			 = KKM.settings.CALIBRATIONS.offset[i]; //!< Смещения для каналов
	}
END_HANDLER(GET_SETTINGS_CALIBRATIONS)
//================================================================================
HANDLER(SET_SETTINGS_CALIBRATIONS)
	int i;
	for(i=0;i<ADC_CHANNEL_NUMBER;i++){
		KKM.settings.CALIBRATIONS.calibration[i]=req->calibration[i]; //!< Калибровки для каналов
		KKM.settings.CALIBRATIONS.offset[i]=req->offset[i]; //!< Смещения для каналов
	}
END_HANDLER(SET_SETTINGS_CALIBRATIONS)
//================================================================================
HANDLER(GET_SETTINGS_PROTECTION)
	pack->Ud_min				=KKM.settings.PROTECTION.Ud_min; //!< Граничные значения для Ud
	pack->Ud_max				=KKM.settings.PROTECTION.Ud_max; //!< Граничные значения для Ud
	pack->temperature		=KKM.settings.PROTECTION.temperature; //!< Граничные значения для Температуры
	pack->U_min					=KKM.settings.PROTECTION.U_min; //!< Граничные значения для напряжения
	pack->U_max					=KKM.settings.PROTECTION.U_max;
	pack->Fnet_min			=KKM.settings.PROTECTION.Fnet_min; //!< минимальная частота сети
	pack->Fnet_max			=KKM.settings.PROTECTION.Fnet_max; //!< максимальная частота сети
	pack->I_max_rms	=KKM.settings.PROTECTION.I_max_rms; //!< Максимальное граничное значение тока фильтра по RMS
	pack->I_max_peak	=KKM.settings.PROTECTION.I_max_peak; //!< Максимальное граничное мгновенное значение тока фильтра
END_HANDLER(GET_SETTINGS_CALIBRATIONS)
//================================================================================
HANDLER(SET_SETTINGS_PROTECTION)
	KKM.settings.PROTECTION.Ud_min			=req->Ud_min; //!< Граничные значения для Ud
	KKM.settings.PROTECTION.Ud_max			=req->Ud_max; //!< Граничные значения для Ud
	KKM.settings.PROTECTION.temperature	=req->temperature; //!< Граничные значения для Температуры
	KKM.settings.PROTECTION.U_min				=req->U_min; //!< Граничные значения для напряжения
	KKM.settings.PROTECTION.U_max				=req->U_max;
	KKM.settings.PROTECTION.Fnet_min		=req->Fnet_min; //!< минимальная частота сети
	KKM.settings.PROTECTION.Fnet_max		=req->Fnet_max; //!< максимальная частота сети
	KKM.settings.PROTECTION.I_max_rms		=req->I_max_rms; //!< Максимальное граничное значение тока фильтра по RMS
	KKM.settings.PROTECTION.I_max_peak	=req->I_max_peak; //!< Максимальное граничное мгновенное значение тока фильтра
END_HANDLER(SET_SETTINGS_CALIBRATIONS)
//================================================================================
HANDLER(GET_SETTINGS_CAPACITORS)
	pack->ctrlUd_Kp					=KKM.settings.CAPACITORS.ctrlUd_Kp;
	pack->ctrlUd_Ki					=KKM.settings.CAPACITORS.ctrlUd_Ki;
	pack->ctrlUd_Kd					=KKM.settings.CAPACITORS.ctrlUd_Kd;
	pack->Ud_nominal				=KKM.settings.CAPACITORS.Ud_nominal;
	pack->Ud_precharge			=KKM.settings.CAPACITORS.Ud_precharge;
END_HANDLER(GET_SETTINGS_CALIBRATIONS)
//================================================================================
HANDLER(SET_SETTINGS_CAPACITORS)
	KKM.settings.CAPACITORS.ctrlUd_Kp			=req->ctrlUd_Kp;
	KKM.settings.CAPACITORS.ctrlUd_Ki			=req->ctrlUd_Ki;
	KKM.settings.CAPACITORS.ctrlUd_Kd			=req->ctrlUd_Kd;
	KKM.settings.CAPACITORS.Ud_nominal		=req->Ud_nominal;
	KKM.settings.CAPACITORS.Ud_precharge	=req->Ud_precharge;
END_HANDLER(SET_SETTINGS_CALIBRATIONS)
//================================================================================
HANDLER(GET_SETTINGS_FILTERS)
	pack->K_I			=KKM.settings.FILTERS.K_I;
	pack->K_U			=KKM.settings.FILTERS.K_U;
	pack->K_UD		=KKM.settings.FILTERS.K_UD;
END_HANDLER(GET_SETTINGS_FILTERS)
//================================================================================
HANDLER(SET_SETTINGS_FILTERS)
	KKM.settings.FILTERS.K_I		=req->K_I;
	KKM.settings.FILTERS.K_U		=req->K_U;
	KKM.settings.FILTERS.K_UD		=req->K_UD;
END_HANDLER(SET_SETTINGS_FILTERS)
//================================================================================
#include "events.h"
HANDLER(GET_EVENTS)
	pack->num=EventsGet(req->afterIndex,MAX_NUM_TRANSFERED_EVENTS,pack->events);
END_HANDLER(GET_EVENTS)
//================================================================================

void prothandlers_init(SciPort *_port) {
	int i;
	for (i = 0; i < lenP_AFG_COMMANDS; i++){
		pr_handlers[i] = (prot_handler) protocol_unknown_command_handle;
	}

	pr_handlers[P_AFG_COMMAND_SWITCH_ON_OFF] =
			(prot_handler) protocol_command_SWITCH_ON_OFF;
	pr_handlers[P_AFG_COMMAND_GET_ADC_ACTIVE] =
			(prot_handler) protocol_command_GET_ADC_ACTIVE;
	pr_handlers[P_AFG_COMMAND_GET_ADC_ACTIVE_RAW] =
			(prot_handler) protocol_command_GET_ADC_ACTIVE_RAW;
	pr_handlers[P_AFG_COMMAND_GET_OSCILOG] =
			(prot_handler) protocol_command_GET_OSCILOG;
	pr_handlers[P_AFG_COMMAND_GET_NET_PARAMS] =
			(prot_handler) protocol_command_GET_NET_PARAMS;

	pr_handlers[P_AFG_COMMAND_SET_SETTINGS_CALIBRATIONS] =
			(prot_handler) protocol_command_SET_SETTINGS_CALIBRATIONS;
	pr_handlers[P_AFG_COMMAND_SET_SETTINGS_PROTECTION] =
			(prot_handler) protocol_command_SET_SETTINGS_PROTECTION;
	pr_handlers[P_AFG_COMMAND_SET_SETTINGS_CAPACITORS] =
			(prot_handler) protocol_command_SET_SETTINGS_CAPACITORS;
	pr_handlers[P_AFG_COMMAND_SET_SETTINGS_FILTERS] =
			(prot_handler) protocol_command_SET_SETTINGS_FILTERS;

	pr_handlers[P_AFG_COMMAND_GET_SETTINGS_CALIBRATIONS] =
			(prot_handler) protocol_command_GET_SETTINGS_CALIBRATIONS;
	pr_handlers[P_AFG_COMMAND_GET_SETTINGS_PROTECTION] =
			(prot_handler) protocol_command_GET_SETTINGS_PROTECTION;
	pr_handlers[P_AFG_COMMAND_GET_SETTINGS_CAPACITORS] =
			(prot_handler) protocol_command_GET_SETTINGS_CAPACITORS;
	pr_handlers[P_AFG_COMMAND_GET_SETTINGS_FILTERS] =
			(prot_handler) protocol_command_GET_SETTINGS_FILTERS;

	pr_handlers[P_AFG_COMMAND_GET_WORK_STATE] =
			(prot_handler) protocol_command_GET_WORK_STATE;

	pr_handlers[P_AFG_COMMAND_GET_VERSION_INFO] =
			(prot_handler) protocol_command_GET_VERSION_INFO;
	pr_handlers[P_AFG_COMMAND_GET_EVENTS] =
			(prot_handler) protocol_command_GET_EVENTS;

	protocol_init(&KKM.protocol,p_mode_client, pr_handlers, lenP_AFG_COMMANDS,_port);
}

