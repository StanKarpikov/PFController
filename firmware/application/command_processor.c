/**
 * @file command_processor.c
 * @author Stanislav Karpikov
 * @brief Analyse commands from the remote panel, handle the interface
 */

/*--------------------------------------------------------------
                       INCLUDE
--------------------------------------------------------------*/

#include "command_processor.h"
#include "pfc_logic.h"
#include "settings.h"
#include "BSP/debug.h"
#include "BSP/system.h"
#include "adc_logic.h"
#include "events.h"
#include "fw_ver.h"
#include "string.h"

/*--------------------------------------------------------------
                       PRIVATE FUNCTIONS PROTOTYPES
--------------------------------------------------------------*/

static void	protocol_command_test(void *pc);
static void	protocol_command_switch_on_off(void *pc);
static void	protocol_command_get_adc_active(void *pc);
static void	protocol_command_get_adc_active_raw(void *pc);
static void	protocol_command_get_oscillog(void *pc);
static void	protocol_command_get_net_params(void *pc);
static void	protocol_command_set_settings_calibrations(void *pc);
static void	protocol_command_set_settings_protection(void *pc);
static void	protocol_command_set_settings_capacitors(void *pc);
static void	protocol_command_set_settings_filters(void *pc);
static void	protocol_command_get_settings_calibrations(void *pc);
static void	protocol_command_get_settings_protection(void *pc);
static void	protocol_command_get_settings_capacitors(void *pc);
static void	protocol_command_get_settings_filters(void *pc);
static void	protocol_command_get_work_state(void *pc);
static void	protocol_command_get_version_info(void *pc);
static void	protocol_command_get_events(void *pc);

/*--------------------------------------------------------------
                       PRIVATE TYPES
--------------------------------------------------------------*/

static PFC_COMMAND_CALLBACK handlers_array[]=
{
	protocol_command_test,
	protocol_command_switch_on_off,
	protocol_command_get_adc_active,
	protocol_command_get_adc_active_raw,
	protocol_command_get_oscillog,
	protocol_command_get_net_params,

	protocol_command_set_settings_calibrations,
	protocol_command_set_settings_protection,
	protocol_command_set_settings_capacitors,
	protocol_command_set_settings_filters,

	protocol_command_get_settings_calibrations,
	protocol_command_get_settings_protection,
	protocol_command_get_settings_capacitors,
	protocol_command_get_settings_filters,

	protocol_command_get_work_state,

	protocol_command_get_version_info,
	protocol_command_get_events
};

enum
{
    OSC_UD,
    OSC_U_A,
    OSC_U_B,
    OSC_U_C,
    OSC_I_A,
    OSC_I_B,
    OSC_I_C,
    OSC_COMP_A,
    OSC_COMP_B,
    OSC_COMP_C,
    OSC_CHANNEL_NUMBER
};

/*--------------------------------------------------------------
                       PRIVATE DATA
--------------------------------------------------------------*/

static float OSC_DATA[OSC_CHANNEL_NUMBER][OSCILLOG_TRANSFER_SIZE]={0};

/*--------------------------------------------------------------
                       PRIVATE FUNCTIONS
--------------------------------------------------------------*/

static void preprocess_answer(
	void **req, void **pack, 
	protocol_context_t *pc, uint32_t len, pfc_interface_commands_t command
	)
{
	*req  = (pc->receivedPackage.data + MIN_PACKET_LEN);
	*pack = (pc->packageToSend.data + MIN_PACKET_LEN);
	
	pc->packageToSend.fields.len = len;
	pc->packageToSend.fields.status.raw = 0;
	pc->packageToSend.fields.command = command;
}

static void	protocol_command_switch_on_off(void *pc)
{
	struct s_command_switch_on_off *req=0;
	struct s_answer_switch_on_off *pack=0;

	preprocess_answer((void**)&req, (void**)&pack, pc, sizeof(struct s_answer_switch_on_off), PFC_COMMAND_SWITCH_ON_OFF);
	
	pack->result = 1;
	if(pfc_apply_command((pfc_commands_t)req->command, req->data) != PFC_SUCCESS)
	{
		pack->result=0;
	}

	package_set_data_len(&(((protocol_context_t*)pc)->packageToSend), sizeof(struct s_answer_switch_on_off));
	protocol_send_package(pc); 
}

static void	protocol_command_get_adc_active(void *pc)
{
	struct s_command_get_adc_active *req=0;
	struct s_answer_get_adc_active *pack=0;

	preprocess_answer((void**)&req, (void**)&pack, pc, sizeof(struct s_answer_get_adc_active), PFC_COMMAND_GET_ADC_ACTIVE);
	
	float active[ADC_CHANNEL_FULL_COUNT]={0}; 
	adc_get_active(active);
	
	pack->ADC_UD = active[ADC_UD];
	pack->ADC_U_A = active[ADC_EMS_A];  //TODO:
	pack->ADC_U_B = active[ADC_EMS_B];
	pack->ADC_U_C = active[ADC_EMS_C];
	pack->ADC_I_A = active[ADC_I_A];
	pack->ADC_I_B = active[ADC_I_B];
	pack->ADC_I_C = active[ADC_I_C];
	pack->ADC_I_ET = active[ADC_I_ET];
	pack->ADC_I_TEMP1 = active[ADC_I_TEMP1];
	pack->ADC_I_TEMP2 = active[ADC_I_TEMP2];
	pack->ADC_EMS_A = active[ADC_EMS_A];
	pack->ADC_EMS_B = active[ADC_EMS_B];
	pack->ADC_EMS_C = active[ADC_EMS_C];
	pack->ADC_EMS_I = active[ADC_EMS_I];
	pack->ADC_MATH_A = active[ADC_EMS_A];  //TODO:
	pack->ADC_MATH_B = active[ADC_EMS_B];
	pack->ADC_MATH_C = active[ADC_EMS_C];

	package_set_data_len(&(((protocol_context_t*)pc)->packageToSend), sizeof(struct s_answer_get_adc_active));
	protocol_send_package(pc); 	
}

static void	protocol_command_get_net_params(void *pc)
{
	struct s_command_get_net_params *req=0;
	struct s_answer_get_net_params *pack=0;

	preprocess_answer((void**)&req, (void**)&pack, pc, sizeof(struct s_answer_get_net_params), PFC_COMMAND_GET_NET_PARAMS);
	
	float U_0Hz[PFC_NCHAN]={0};
  float I_0Hz[PFC_NCHAN]={0};
  float U_phase[PFC_NCHAN]={0};
  float thdu[PFC_NCHAN]={0};
	float period_fact=0;
	adc_get_params(U_0Hz, I_0Hz, U_phase, thdu, &period_fact);
	
	pack->period_fact = period_fact;
	pack->U0Hz_A = U_0Hz[0];
	pack->U0Hz_B = U_0Hz[1];
	pack->U0Hz_C = U_0Hz[2];
	pack->I0Hz_A = I_0Hz[0];
	pack->I0Hz_B = I_0Hz[1];
	pack->I0Hz_C = I_0Hz[2];

	pack->thdu_A = thdu[0];
	pack->thdu_B = thdu[1];
	pack->thdu_C = thdu[2];

	pack->U_phase_A = U_phase[0];
	pack->U_phase_B = U_phase[1];
	pack->U_phase_C = U_phase[2];

	package_set_data_len(&(((protocol_context_t*)pc)->packageToSend), sizeof(struct s_answer_get_net_params));
	protocol_send_package(pc); 
}



static void	protocol_command_get_adc_active_raw(void *pc)
{
	struct s_command_get_adc_active_raw *req=0;
	struct s_answer_get_adc_active_raw *pack=0;

	preprocess_answer((void**)&req, (void**)&pack, pc, sizeof(struct s_answer_get_adc_active_raw), PFC_COMMAND_GET_ADC_ACTIVE_RAW);
		
	float active_raw[ADC_CHANNEL_NUMBER]={0}; 
	adc_get_active_raw(active_raw);
		
	pack->ADC_UD = active_raw[ADC_UD];
	pack->ADC_U_A = active_raw[ADC_U_A];
	pack->ADC_U_B = active_raw[ADC_U_B];
	pack->ADC_U_C = active_raw[ADC_U_C];
	pack->ADC_I_A = active_raw[ADC_I_A];
	pack->ADC_I_B = active_raw[ADC_I_B];
	pack->ADC_I_C = active_raw[ADC_I_C];
	pack->ADC_I_ET = active_raw[ADC_I_ET];
	pack->ADC_I_TEMP1 = active_raw[ADC_I_TEMP1];
	pack->ADC_I_TEMP2 = active_raw[ADC_I_TEMP2];
	pack->ADC_EMS_A = active_raw[ADC_EMS_A];
	pack->ADC_EMS_B = active_raw[ADC_EMS_B];
	pack->ADC_EMS_C = active_raw[ADC_EMS_C];
	pack->ADC_EMS_I = active_raw[ADC_EMS_I];

	package_set_data_len(&(((protocol_context_t*)pc)->packageToSend), sizeof(struct s_answer_get_adc_active_raw));
	protocol_send_package(pc); 
}

static void	protocol_command_get_work_state(void *pc)
{
	struct s_command_get_work_state *req=0;
	struct s_answer_get_work_state *pack=0;

	preprocess_answer((void**)&req, (void**)&pack, pc, sizeof(struct s_answer_get_work_state), PFC_COMMAND_GET_WORK_STATE);

	system_set_time(req->currentTime);

	pack->state = pfc_get_state();
	
	settings_pwm_t pwm_settings = settings_get_pwm();
	pack->activeChannels[0] = pwm_settings.activeChannels[0];
	pack->activeChannels[1] = pwm_settings.activeChannels[1];
	pack->activeChannels[2] = pwm_settings.activeChannels[2];

	package_set_data_len(&(((protocol_context_t*)pc)->packageToSend), sizeof(struct s_answer_get_work_state));
	protocol_send_package(pc); 
}

static status_t get_date_from_compiler_string(
		char* date, char* time,
		uint16_t* DD, uint16_t* MM, uint16_t* YYYY, 
		uint16_t* HH, uint16_t* MN, uint16_t* SS	
	)
{
	ARGUMENT_ASSERT(date);
	ARGUMENT_ASSERT(time);
	ARGUMENT_ASSERT(DD);
	ARGUMENT_ASSERT(MM);
	ARGUMENT_ASSERT(YYYY);
	ARGUMENT_ASSERT(HH);
	ARGUMENT_ASSERT(MN);
	ARGUMENT_ASSERT(SS);
	
	*DD = 0;
	if (date[4] >= '0') *DD = (date[4] - '0');
	*DD = *DD * 10 + (date[5] - '0');
	*MM = BUILD_MONTH_CH1;
	*YYYY = (date[7] - '0') * 1000 + (date[8] - '0') * 100 + (date[9] - '0') * 10 + (date[10] - '0');
	*HH = (time[0] - '0') * 10 + (time[1] - '0');
	*MN = (time[3] - '0') * 10 + (time[4] - '0');
	*SS = (time[6] - '0') * 10 + (time[7] - '0');
	return PFC_SUCCESS;
}

static void	protocol_command_get_version_info(void *pc)
{
	struct s_command_get_version_info *req=0;
	struct s_answer_get_version_info *pack=0;

	preprocess_answer((void**)&req, (void**)&pack, pc, sizeof(struct s_answer_get_version_info), PFC_COMMAND_GET_VERSION_INFO);
	
	pack->major = FW_VERSION_MAJOR;
	pack->minor = FW_VERSION_MINOR;
	pack->micro = FW_VERSION_MICRO;
	pack->build = FW_VERSION_BUILD;

	uint16_t DD=0, MM=0, YYYY=0, HH=0, MN=0, SS=0;
	get_date_from_compiler_string(__DATE__, __TIME__, &DD, &MM, &YYYY, &HH, &MN, &SS);

	pack->day = DD;
	pack->month = MM;
	pack->year = YYYY;
	pack->hour = HH;
	pack->minute = MN;
	pack->second = SS;

	package_set_data_len(&(((protocol_context_t*)pc)->packageToSend), sizeof(struct s_answer_get_version_info));
	protocol_send_package(pc); 
}



static void	protocol_command_get_oscillog(void *pc)
{
	struct s_command_get_oscillog *req=0;
	struct s_answer_get_oscillog *pack=0;

	preprocess_answer((void**)&req, (void**)&pack, pc, sizeof(struct s_answer_get_oscillog), PFC_COMMAND_GET_OSCILLOG);
	
	if (req->num > OSC_CHANNEL_NUMBER)
	{
			protocol_error_handle(pc, package_get_command(&(((protocol_context_t*)pc)->receivedPackage)));
			return;
	}
	pack->ch = req->num;
	pack->len = OSCILLOG_TRANSFER_SIZE;

	float oscillog_max = -1e10;
	float oscillog_min = 1e10;
	for (int i = 0; i < OSCILLOG_TRANSFER_SIZE; i++)
	{
			if (OSC_DATA[req->num][i] > oscillog_max) oscillog_max = OSC_DATA[req->num][i];
			if (OSC_DATA[req->num][i] < oscillog_min) oscillog_min = OSC_DATA[req->num][i];
	}
	float astep = 0.0f;
	if ((oscillog_max - oscillog_min) == 0)
	{
			astep = 255.0f;
	}
	else
	{
			astep = 255.0f / (oscillog_max - oscillog_min);
	}
	for (int i = 0; i < OSCILLOG_TRANSFER_SIZE; i++)
	{
			pack->data[i] = (OSC_DATA[req->num][i] - oscillog_min) * astep;
	}
	pack->max = oscillog_max;
	pack->min = oscillog_min;


	package_set_data_len(&(((protocol_context_t*)pc)->packageToSend), sizeof(struct s_answer_get_oscillog));
	protocol_send_package(pc); 
}



static void	protocol_command_get_settings_calibrations(void *pc)
{
	struct s_command_get_settings_calibrations *req=0;
	struct s_answer_get_settings_calibrations *pack=0;

	preprocess_answer((void**)&req, (void**)&pack, pc, sizeof(struct s_answer_get_settings_calibrations), PFC_COMMAND_GET_SETTINGS_CALIBRATIONS);
	
	settings_calibrations_t calibrations = settings_get_calibrations();
	
	int i;
	for (i = 0; i < ADC_CHANNEL_NUMBER; i++)
	{
			pack->calibration[i] = calibrations.calibration[i];
			pack->offset[i] = calibrations.offset[i];
	}

	package_set_data_len(&(((protocol_context_t*)pc)->packageToSend), sizeof(struct s_answer_get_settings_calibrations));
	protocol_send_package(pc); 
}



static void	protocol_command_set_settings_calibrations(void *pc)
{
	struct s_command_set_settings_calibrations *req=0;
	struct s_answer_set_settings_calibrations *pack=0;

	preprocess_answer((void**)&req, (void**)&pack, pc, sizeof(struct s_answer_set_settings_calibrations), PFC_COMMAND_SET_SETTINGS_CALIBRATIONS);
	
	settings_calibrations_t calibrations = settings_get_calibrations();
	
	int i;
	for (i = 0; i < ADC_CHANNEL_NUMBER; i++)
	{
			calibrations.calibration[i] = req->calibration[i];
			calibrations.offset[i] = req->offset[i];
	}
	
	settings_set_calibrations(calibrations);

	package_set_data_len(&(((protocol_context_t*)pc)->packageToSend), sizeof(struct s_answer_set_settings_calibrations));
	protocol_send_package(pc); 
}


static void	protocol_command_get_settings_protection(void *pc)
{
	struct s_command_get_settings_protection *req=0;
	struct s_answer_get_settings_protection *pack=0;

	preprocess_answer((void**)&req, (void**)&pack, pc, sizeof(struct s_answer_get_settings_protection), PFC_COMMAND_GET_SETTINGS_PROTECTION);
	
	settings_protection_t protection = settings_get_protection();
	
	pack->Ud_min = protection.Ud_min;
	pack->Ud_max = protection.Ud_max;
	pack->temperature = protection.temperature;
	pack->U_min = protection.U_min;
	pack->U_max = protection.U_max;
	pack->Fnet_min = protection.Fnet_min;
	pack->Fnet_max = protection.Fnet_max;
	pack->I_max_rms = protection.I_max_rms;
	pack->I_max_peak = protection.I_max_peak;

	package_set_data_len(&(((protocol_context_t*)pc)->packageToSend), sizeof(struct s_answer_get_settings_protection));
	protocol_send_package(pc); 
}



static void	protocol_command_set_settings_protection(void *pc)
{
	struct s_command_set_settings_protection *req=0;
	struct s_answer_set_settings_protection *pack=0;

	preprocess_answer((void**)&req, (void**)&pack, pc, sizeof(struct s_answer_set_settings_protection), PFC_COMMAND_SET_SETTINGS_PROTECTION);
	
	settings_protection_t protection = settings_get_protection();
	
	protection.Ud_min = req->Ud_min;
	protection.Ud_max = req->Ud_max;
	protection.temperature = req->temperature;
	protection.U_min = req->U_min;
	protection.U_max = req->U_max;
	protection.Fnet_min = req->Fnet_min;
	protection.Fnet_max = req->Fnet_max;
	protection.I_max_rms = req->I_max_rms;
	protection.I_max_peak = req->I_max_peak;
	
	settings_set_protection(protection);

	package_set_data_len(&(((protocol_context_t*)pc)->packageToSend), sizeof(struct s_answer_set_settings_protection));
	protocol_send_package(pc); 
}


static void	protocol_command_get_settings_capacitors(void *pc)
{
	struct s_command_get_settings_capacitors *req=0;
	struct s_answer_get_settings_capacitors *pack=0;

	preprocess_answer((void**)&req, (void**)&pack, pc, sizeof(struct s_answer_get_settings_capacitors), PFC_COMMAND_GET_SETTINGS_CAPACITORS);
	
	settings_capacitors_t capacitors = settings_get_capacitors();
	
	pack->ctrlUd_Kp = capacitors.ctrlUd_Kp;
	pack->ctrlUd_Ki = capacitors.ctrlUd_Ki;
	pack->ctrlUd_Kd = capacitors.ctrlUd_Kd;
	pack->Ud_nominal = capacitors.Ud_nominal;
	pack->Ud_precharge = capacitors.Ud_precharge;

	package_set_data_len(&(((protocol_context_t*)pc)->packageToSend), sizeof(struct s_answer_get_settings_capacitors));
	protocol_send_package(pc); 
}


static void	protocol_command_set_settings_capacitors(void *pc)
{
	struct s_command_set_settings_capacitors *req=0;
	struct s_answer_set_settings_capacitors *pack=0;

	preprocess_answer((void**)&req, (void**)&pack, pc, sizeof(struct s_answer_set_settings_capacitors), PFC_COMMAND_SET_SETTINGS_CAPACITORS);
	
	settings_capacitors_t capacitors = settings_get_capacitors();
	
	capacitors.ctrlUd_Kp = req->ctrlUd_Kp;
	capacitors.ctrlUd_Ki = req->ctrlUd_Ki;
	capacitors.ctrlUd_Kd = req->ctrlUd_Kd;
	capacitors.Ud_nominal = req->Ud_nominal;
	capacitors.Ud_precharge = req->Ud_precharge;
	
	settings_set_capacitors(capacitors);

	package_set_data_len(&(((protocol_context_t*)pc)->packageToSend), sizeof(struct s_answer_set_settings_capacitors));
	protocol_send_package(pc); 
}


static void	protocol_command_get_settings_filters(void *pc)
{
	struct s_command_get_settings_filters *req=0;
	struct s_answer_get_settings_filters *pack=0;

	preprocess_answer((void**)&req, (void**)&pack, pc, sizeof(struct s_answer_get_settings_filters), PFC_COMMAND_GET_SETTINGS_FILTERS);
	
	settings_filters_t filters = settings_get_filters();
	
	pack->K_I = filters.K_I;
	pack->K_U = filters.K_U;
	pack->K_UD = filters.K_UD;
	

	package_set_data_len(&(((protocol_context_t*)pc)->packageToSend), sizeof(struct s_answer_get_settings_filters));
	protocol_send_package(pc); 
}


static void	protocol_command_set_settings_filters(void *pc)
{
	struct s_command_set_settings_filters *req=0;
	struct s_answer_set_settings_filters *pack=0;

	preprocess_answer((void**)&req, (void**)&pack, pc, sizeof(struct s_answer_set_settings_filters), PFC_COMMAND_SET_SETTINGS_FILTERS);
	
	settings_filters_t filters = settings_get_filters();
	
	filters.K_I = req->K_I;
	filters.K_U = req->K_U;
	filters.K_UD = req->K_UD;
	
	settings_set_filters(filters);

	package_set_data_len(&(((protocol_context_t*)pc)->packageToSend), sizeof(struct s_answer_set_settings_filters));
	protocol_send_package(pc); 
}


static void	protocol_command_get_events(void *pc)
{
	struct s_command_get_events *req=0;
	struct s_answer_get_events *pack=0;

	preprocess_answer((void**)&req, (void**)&pack, pc, sizeof(struct s_answer_get_events), PFC_COMMAND_GET_EVENTS);
	
	pack->num = events_get(req->after_index, MAX_NUM_TRANSFERED_EVENTS, pack->events);

	package_set_data_len(&(((protocol_context_t*)pc)->packageToSend), sizeof(struct s_answer_get_events));
	protocol_send_package(pc); 
}


static void	protocol_command_test(void *pc)
{
	/* TEST */
}

/*--------------------------------------------------------------
                       PUBLIC FUNCTIONS
--------------------------------------------------------------*/
	
void protocol_write_osc_data(float** osc_adc_ch)
{
	/* TODO: Add lock protection */
	for (int i = 0; i < PFC_NCHAN; i++)
	{
			memcpy(OSC_DATA[OSC_U_A + i], osc_adc_ch[ADC_MATH_A + i], sizeof(OSC_DATA[OSC_U_A + i]));
			memcpy(OSC_DATA[OSC_I_A + i], osc_adc_ch[ADC_I_A + i], sizeof(OSC_DATA[OSC_I_A + i]));
			memcpy(OSC_DATA[OSC_COMP_A + i], osc_adc_ch[ADC_MATH_C_A + i], sizeof(OSC_DATA[OSC_COMP_A + i]));
	}
	memcpy(OSC_DATA[OSC_UD], osc_adc_ch[ADC_UD], sizeof(OSC_DATA[OSC_UD]));
}

void prothandlers_init(SciPort *_port)
{
    protocol_init(p_mode_client, handlers_array, PFC_COMMAND_COUNT, _port);
}
