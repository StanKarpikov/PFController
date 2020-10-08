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

/** The array of handlers for procotol command functions */
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

/** Oscillogram channels */
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

/** The last save oscillogram data */
static float OSC_DATA[OSC_CHANNEL_NUMBER][OSCILLOG_TRANSFER_SIZE]={0};

/*--------------------------------------------------------------
                       PRIVATE FUNCTIONS
--------------------------------------------------------------*/

/**
 * @brief Extract date and time from the compiller strings
 * 
 * @param pc A pointer to the protocol context
 */
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

/**
 * @brief Preprocess the data before answering
 * 
 * @param req A pointer to the request packet structure
 * @param answer A pointer to the answer packet structure
 * @param pc A pointer to the protocol context
 * @param len The message length
 * @param command A protocol command to execute (or to answer)
 */
static void preprocess_answer(
															void **req, void **answer, 
															protocol_context_t *pc, 
															uint32_t len, 
															pfc_interface_commands_t command
															)
{
	*req    = (pc->packet_received.data + MINIMUM_PACKET_LENGTH);
	*answer = (pc->packet_to_send.data + MINIMUM_PACKET_LENGTH);
	
	pc->packet_to_send.fields.len = len;
	pc->packet_to_send.fields.status.raw = 0;
	pc->packet_to_send.fields.command = command;
}

/**
 * @brief Protocol command: switch ON/OFF
 * 
 * @param pc A pointer to the protocol context
 */
static void	protocol_command_switch_on_off(void *pc)
{
	struct s_command_switch_on_off *req=0;
	struct s_answer_switch_on_off *answer=0;

	preprocess_answer((void**)&req, (void**)&answer, pc, sizeof(struct s_answer_switch_on_off), PFC_COMMAND_SWITCH_ON_OFF);
	
	answer->result = 1;
	if(pfc_apply_command((pfc_commands_t)req->command, req->data) != PFC_SUCCESS)
	{
		answer->result=0;
	}

	packet_set_data_len(&(((protocol_context_t*)pc)->packet_to_send), sizeof(struct s_answer_switch_on_off));
	protocol_send_packet(pc); 
}

/**
 * @brief Protocol command: get instantenous ADC values
 * 
 * @param pc A pointer to the protocol context
 */
static void	protocol_command_get_adc_active(void *pc)
{
	struct s_command_get_adc_active *req=0;
	struct s_answer_get_adc_active *answer=0;

	preprocess_answer((void**)&req, (void**)&answer, pc, sizeof(struct s_answer_get_adc_active), PFC_COMMAND_GET_ADC_ACTIVE);
	
	float active[ADC_CHANNEL_FULL_COUNT]={0}; 
	adc_get_active(active);
	
	answer->ADC_UD = active[ADC_UD];
	answer->ADC_U_A = active[ADC_EMS_A];
	answer->ADC_U_B = active[ADC_EMS_B];
	answer->ADC_U_C = active[ADC_EMS_C];
	answer->ADC_I_A = active[ADC_I_A];
	answer->ADC_I_B = active[ADC_I_B];
	answer->ADC_I_C = active[ADC_I_C];
	answer->ADC_I_ET = active[ADC_I_ET];
	answer->ADC_I_TEMP1 = active[ADC_I_TEMP1];
	answer->ADC_I_TEMP2 = active[ADC_I_TEMP2];
	answer->ADC_EMS_A = active[ADC_EMS_A];
	answer->ADC_EMS_B = active[ADC_EMS_B];
	answer->ADC_EMS_C = active[ADC_EMS_C];
	answer->ADC_EMS_I = active[ADC_EMS_I];
	answer->ADC_MATH_A = active[ADC_EMS_A];
	answer->ADC_MATH_B = active[ADC_EMS_B];
	answer->ADC_MATH_C = active[ADC_EMS_C];

	packet_set_data_len(&(((protocol_context_t*)pc)->packet_to_send), sizeof(struct s_answer_get_adc_active));
	protocol_send_packet(pc); 	
}

/**
 * @brief Protocol command: get network parameters
 * 
 * @param pc A pointer to the protocol context
 */
static void	protocol_command_get_net_params(void *pc)
{
	struct s_command_get_net_params *req=0;
	struct s_answer_get_net_params *answer=0;

	preprocess_answer((void**)&req, (void**)&answer, pc, sizeof(struct s_answer_get_net_params), PFC_COMMAND_GET_NET_PARAMS);
	
	float U_0Hz[PFC_NCHAN]={0};
  float I_0Hz[PFC_NCHAN]={0};
  float U_phase[PFC_NCHAN]={0};
  float thdu[PFC_NCHAN]={0};
	float period_fact=0;
	adc_get_params(U_0Hz, I_0Hz, U_phase, thdu, &period_fact);
	
	answer->period_fact = period_fact;
	answer->U0Hz_A = U_0Hz[PFC_ACHAN];
	answer->U0Hz_B = U_0Hz[PFC_BCHAN];
	answer->U0Hz_C = U_0Hz[PFC_CCHAN];
	answer->I0Hz_A = I_0Hz[PFC_ACHAN];
	answer->I0Hz_B = I_0Hz[PFC_BCHAN];
	answer->I0Hz_C = I_0Hz[PFC_CCHAN];

	answer->thdu_A = thdu[PFC_ACHAN];
	answer->thdu_B = thdu[PFC_BCHAN];
	answer->thdu_C = thdu[PFC_CCHAN];

	answer->U_phase_A = U_phase[PFC_ACHAN];
	answer->U_phase_B = U_phase[PFC_BCHAN];
	answer->U_phase_C = U_phase[PFC_CCHAN];

	packet_set_data_len(&(((protocol_context_t*)pc)->packet_to_send), sizeof(struct s_answer_get_net_params));
	protocol_send_packet(pc); 
}

/**
 * @brief Protocol command: get instantenous ADC values (raw format)
 * 
 * @param pc A pointer to the protocol context
 */
static void	protocol_command_get_adc_active_raw(void *pc)
{
	struct s_command_get_adc_active_raw *req=0;
	struct s_answer_get_adc_active_raw *answer=0;

	preprocess_answer((void**)&req, (void**)&answer, pc, sizeof(struct s_answer_get_adc_active_raw), PFC_COMMAND_GET_ADC_ACTIVE_RAW);
		
	float active_raw[ADC_CHANNEL_NUMBER]={0}; 
	adc_get_active_raw(active_raw);
		
	answer->ADC_UD = active_raw[ADC_UD];
	answer->ADC_U_A = active_raw[ADC_U_A];
	answer->ADC_U_B = active_raw[ADC_U_B];
	answer->ADC_U_C = active_raw[ADC_U_C];
	answer->ADC_I_A = active_raw[ADC_I_A];
	answer->ADC_I_B = active_raw[ADC_I_B];
	answer->ADC_I_C = active_raw[ADC_I_C];
	answer->ADC_I_ET = active_raw[ADC_I_ET];
	answer->ADC_I_TEMP1 = active_raw[ADC_I_TEMP1];
	answer->ADC_I_TEMP2 = active_raw[ADC_I_TEMP2];
	answer->ADC_EMS_A = active_raw[ADC_EMS_A];
	answer->ADC_EMS_B = active_raw[ADC_EMS_B];
	answer->ADC_EMS_C = active_raw[ADC_EMS_C];
	answer->ADC_EMS_I = active_raw[ADC_EMS_I];

	packet_set_data_len(&(((protocol_context_t*)pc)->packet_to_send), sizeof(struct s_answer_get_adc_active_raw));
	protocol_send_packet(pc); 
}

/**
 * @brief Protocol command: get work state
 * 
 * @param pc A pointer to the protocol context
 */
static void	protocol_command_get_work_state(void *pc)
{
	struct s_command_get_work_state *req=0;
	struct s_answer_get_work_state *answer=0;

	preprocess_answer((void**)&req, (void**)&answer, pc, sizeof(struct s_answer_get_work_state), PFC_COMMAND_GET_WORK_STATE);

	system_set_time(req->currentTime);

	answer->state = pfc_get_state();
	
	settings_pwm_t pwm_settings = settings_get_pwm();
	answer->activeChannels[PFC_ACHAN] = pwm_settings.activeChannels[PFC_ACHAN];
	answer->activeChannels[PFC_BCHAN] = pwm_settings.activeChannels[PFC_BCHAN];
	answer->activeChannels[PFC_CCHAN] = pwm_settings.activeChannels[PFC_CCHAN];

	packet_set_data_len(&(((protocol_context_t*)pc)->packet_to_send), sizeof(struct s_answer_get_work_state));
	protocol_send_packet(pc); 
}

/**
 * @brief Protocol command: get version info
 * 
 * @param pc A pointer to the protocol context
 */
static void	protocol_command_get_version_info(void *pc)
{
	struct s_command_get_version_info *req=0;
	struct s_answer_get_version_info *answer=0;

	preprocess_answer((void**)&req, (void**)&answer, pc, sizeof(struct s_answer_get_version_info), PFC_COMMAND_GET_VERSION_INFO);
	
	answer->major = FW_VERSION_MAJOR;
	answer->minor = FW_VERSION_MINOR;
	answer->micro = FW_VERSION_MICRO;
	answer->build = FW_VERSION_BUILD;

	uint16_t DD=0, MM=0, YYYY=0, HH=0, MN=0, SS=0;
	get_date_from_compiler_string(__DATE__, __TIME__, &DD, &MM, &YYYY, &HH, &MN, &SS);

	answer->day = DD;
	answer->month = MM;
	answer->year = YYYY;
	answer->hour = HH;
	answer->minute = MN;
	answer->second = SS;

	packet_set_data_len(&(((protocol_context_t*)pc)->packet_to_send), sizeof(struct s_answer_get_version_info));
	protocol_send_packet(pc); 
}

/**
 * @brief Protocol command: get oscillogram
 * 
 * @param pc A pointer to the protocol context
 */
static void	protocol_command_get_oscillog(void *pc)
{
	struct s_command_get_oscillog *req=0;
	struct s_answer_get_oscillog *answer=0;

	preprocess_answer((void**)&req, (void**)&answer, pc, sizeof(struct s_answer_get_oscillog), PFC_COMMAND_GET_OSCILLOG);
	
	if (req->num > OSC_CHANNEL_NUMBER)
	{
			protocol_error_handle(pc, packet_get_command(&(((protocol_context_t*)pc)->packet_received)));
			return;
	}
	answer->ch = req->num;
	answer->len = OSCILLOG_TRANSFER_SIZE;

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
			answer->data[i] = (OSC_DATA[req->num][i] - oscillog_min) * astep;
	}
	answer->max = oscillog_max;
	answer->min = oscillog_min;


	packet_set_data_len(&(((protocol_context_t*)pc)->packet_to_send), sizeof(struct s_answer_get_oscillog));
	protocol_send_packet(pc); 
}

/**
 * @brief Protocol command: get calibration settings
 * 
 * @param pc A pointer to the protocol context
 */
static void	protocol_command_get_settings_calibrations(void *pc)
{
	struct s_command_get_settings_calibrations *req=0;
	struct s_answer_get_settings_calibrations *answer=0;

	preprocess_answer((void**)&req, (void**)&answer, pc, sizeof(struct s_answer_get_settings_calibrations), PFC_COMMAND_GET_SETTINGS_CALIBRATIONS);
	
	settings_calibrations_t calibrations = settings_get_calibrations();
	
	int i;
	for (i = 0; i < ADC_CHANNEL_NUMBER; i++)
	{
			answer->calibration[i] = calibrations.calibration[i];
			answer->offset[i] = calibrations.offset[i];
	}

	packet_set_data_len(&(((protocol_context_t*)pc)->packet_to_send), sizeof(struct s_answer_get_settings_calibrations));
	protocol_send_packet(pc); 
}

/**
 * @brief Protocol command: set calibration settings
 * 
 * @param pc A pointer to the protocol context
 */
static void	protocol_command_set_settings_calibrations(void *pc)
{
	struct s_command_set_settings_calibrations *req=0;
	struct s_answer_set_settings_calibrations *answer=0;

	preprocess_answer((void**)&req, (void**)&answer, pc, sizeof(struct s_answer_set_settings_calibrations), PFC_COMMAND_SET_SETTINGS_CALIBRATIONS);
	
	settings_calibrations_t calibrations = settings_get_calibrations();
	
	int i;
	for (i = 0; i < ADC_CHANNEL_NUMBER; i++)
	{
			calibrations.calibration[i] = req->calibration[i];
			calibrations.offset[i] = req->offset[i];
	}
	
	settings_set_calibrations(calibrations);

	packet_set_data_len(&(((protocol_context_t*)pc)->packet_to_send), sizeof(struct s_answer_set_settings_calibrations));
	protocol_send_packet(pc); 
}

/**
 * @brief Protocol command: get protection settings
 * 
 * @param pc A pointer to the protocol context
 */
static void	protocol_command_get_settings_protection(void *pc)
{
	struct s_command_get_settings_protection *req=0;
	struct s_answer_get_settings_protection *answer=0;

	preprocess_answer((void**)&req, (void**)&answer, pc, sizeof(struct s_answer_get_settings_protection), PFC_COMMAND_GET_SETTINGS_PROTECTION);
	
	settings_protection_t protection = settings_get_protection();
	
	answer->Ud_min = protection.Ud_min;
	answer->Ud_max = protection.Ud_max;
	answer->temperature = protection.temperature;
	answer->U_min = protection.U_min;
	answer->U_max = protection.U_max;
	answer->Fnet_min = protection.Fnet_min;
	answer->Fnet_max = protection.Fnet_max;
	answer->I_max_rms = protection.I_max_rms;
	answer->I_max_peak = protection.I_max_peak;

	packet_set_data_len(&(((protocol_context_t*)pc)->packet_to_send), sizeof(struct s_answer_get_settings_protection));
	protocol_send_packet(pc); 
}

/**
 * @brief Protocol command: set protection settings
 * 
 * @param pc A pointer to the protocol context
 */
static void	protocol_command_set_settings_protection(void *pc)
{
	struct s_command_set_settings_protection *req=0;
	struct s_answer_set_settings_protection *answer=0;

	preprocess_answer((void**)&req, (void**)&answer, pc, sizeof(struct s_answer_set_settings_protection), PFC_COMMAND_SET_SETTINGS_PROTECTION);
	
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

	packet_set_data_len(&(((protocol_context_t*)pc)->packet_to_send), sizeof(struct s_answer_set_settings_protection));
	protocol_send_packet(pc); 
}

/**
 * @brief Protocol command: get capacitors settings
 * 
 * @param pc A pointer to the protocol context
 */
static void	protocol_command_get_settings_capacitors(void *pc)
{
	struct s_command_get_settings_capacitors *req=0;
	struct s_answer_get_settings_capacitors *answer=0;

	preprocess_answer((void**)&req, (void**)&answer, pc, sizeof(struct s_answer_get_settings_capacitors), PFC_COMMAND_GET_SETTINGS_CAPACITORS);
	
	settings_capacitors_t capacitors = settings_get_capacitors();
	
	answer->ctrlUd_Kp = capacitors.ctrlUd_Kp;
	answer->ctrlUd_Ki = capacitors.ctrlUd_Ki;
	answer->ctrlUd_Kd = capacitors.ctrlUd_Kd;
	answer->Ud_nominal = capacitors.Ud_nominal;
	answer->Ud_precharge = capacitors.Ud_precharge;

	packet_set_data_len(&(((protocol_context_t*)pc)->packet_to_send), sizeof(struct s_answer_get_settings_capacitors));
	protocol_send_packet(pc); 
}

/**
 * @brief Protocol command: set caacitor settings
 * 
 * @param pc A pointer to the protocol context
 */
static void	protocol_command_set_settings_capacitors(void *pc)
{
	struct s_command_set_settings_capacitors *req=0;
	struct s_answer_set_settings_capacitors *answer=0;

	preprocess_answer((void**)&req, (void**)&answer, pc, sizeof(struct s_answer_set_settings_capacitors), PFC_COMMAND_SET_SETTINGS_CAPACITORS);
	
	settings_capacitors_t capacitors = settings_get_capacitors();
	
	capacitors.ctrlUd_Kp = req->ctrlUd_Kp;
	capacitors.ctrlUd_Ki = req->ctrlUd_Ki;
	capacitors.ctrlUd_Kd = req->ctrlUd_Kd;
	capacitors.Ud_nominal = req->Ud_nominal;
	capacitors.Ud_precharge = req->Ud_precharge;
	
	settings_set_capacitors(capacitors);

	packet_set_data_len(&(((protocol_context_t*)pc)->packet_to_send), sizeof(struct s_answer_set_settings_capacitors));
	protocol_send_packet(pc); 
}

/**
 * @brief Protocol command: get filter settings
 * 
 * @param pc A pointer to the protocol context
 */
static void	protocol_command_get_settings_filters(void *pc)
{
	struct s_command_get_settings_filters *req=0;
	struct s_answer_get_settings_filters *answer=0;

	preprocess_answer((void**)&req, (void**)&answer, pc, sizeof(struct s_answer_get_settings_filters), PFC_COMMAND_GET_SETTINGS_FILTERS);
	
	settings_filters_t filters = settings_get_filters();
	
	answer->K_I = filters.K_I;
	answer->K_U = filters.K_U;
	answer->K_UD = filters.K_UD;
	

	packet_set_data_len(&(((protocol_context_t*)pc)->packet_to_send), sizeof(struct s_answer_get_settings_filters));
	protocol_send_packet(pc); 
}

/**
 * @brief Protocol command: set filter settings
 * 
 * @param pc A pointer to the protocol context
 */
static void	protocol_command_set_settings_filters(void *pc)
{
	struct s_command_set_settings_filters *req=0;
	struct s_answer_set_settings_filters *answer=0;

	preprocess_answer((void**)&req, (void**)&answer, pc, sizeof(struct s_answer_set_settings_filters), PFC_COMMAND_SET_SETTINGS_FILTERS);
	
	settings_filters_t filters = settings_get_filters();
	
	filters.K_I = req->K_I;
	filters.K_U = req->K_U;
	filters.K_UD = req->K_UD;
	
	settings_set_filters(filters);

	packet_set_data_len(&(((protocol_context_t*)pc)->packet_to_send), sizeof(struct s_answer_set_settings_filters));
	protocol_send_packet(pc); 
}

/**
 * @brief Protocol command: get events
 * 
 * @param pc A pointer to the protocol context
 */
static void	protocol_command_get_events(void *pc)
{
	struct s_command_get_events *req=0;
	struct s_answer_get_events *answer=0;

	preprocess_answer((void**)&req, (void**)&answer, pc, sizeof(struct s_answer_get_events), PFC_COMMAND_GET_EVENTS);
	
	answer->num = events_get(req->after_index, MAX_NUM_TRANSFERED_EVENTS, answer->events);

	packet_set_data_len(&(((protocol_context_t*)pc)->packet_to_send), sizeof(struct s_answer_get_events));
	protocol_send_packet(pc); 
}

/**
 * @brief Protocol command: test command
 * 
 * @param pc A pointer to the protocol context
 */
static void	protocol_command_test(void *pc)
{
	/* TEST */
}

/*--------------------------------------------------------------
                       PUBLIC FUNCTIONS
--------------------------------------------------------------*/
	
/*
 * @brief Collect oscillogram data
 * 
 * @param osc_adc_ch A pointer to the ADC structure (should be in the specific format)
 */
status_t protocol_write_osc_data(float** osc_adc_ch)
{
	ARGUMENT_ASSERT(osc_adc_ch);
	/* TODO: Add lock protection */
	for (int i = 0; i < PFC_NCHAN; i++)
	{
			memcpy(OSC_DATA[OSC_U_A + i], osc_adc_ch[ADC_MATH_A + i], sizeof(OSC_DATA[OSC_U_A + i]));
			memcpy(OSC_DATA[OSC_I_A + i], osc_adc_ch[ADC_I_A + i], sizeof(OSC_DATA[OSC_I_A + i]));
			memcpy(OSC_DATA[OSC_COMP_A + i], osc_adc_ch[ADC_MATH_C_A + i], sizeof(OSC_DATA[OSC_COMP_A + i]));
	}
	memcpy(OSC_DATA[OSC_UD], osc_adc_ch[ADC_UD], sizeof(OSC_DATA[OSC_UD]));
	return PFC_SUCCESS;
}

/*
 * @brief Init handlers for protocol commands
 * 
 * @return The operation status
 */
status_t command_handlers_init(void)
{
    return protocol_init(handlers_array, PFC_COMMAND_COUNT);
}
