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

/*--------------------------------------------------------------
                       PRIVATE FUNCTIONS PROTOTYPES
--------------------------------------------------------------*/

static void	protocol_command_test(protocol_context_t *pc);
static void	protocol_command_switch_on_off(protocol_context_t *pc);
static void	protocol_command_get_ac_active(protocol_context_t *pc);
static void	protocol_command_get_adc_active_raw(protocol_context_t *pc);
static void	protocol_command_get_oscillog(protocol_context_t *pc);
static void	protocol_command_get_net_params(protocol_context_t *pc);
static void	protocol_command_set_settings_calbrations(protocol_context_t *pc);
static void	protocol_command_set_settings_protection(protocol_context_t *pc);
static void	protocol_command_set_settings_capacitors(protocol_context_t *pc);
static void	protocol_command_set_settings_filters(protocol_context_t *pc);
static void	protocol_command_get_settings_calibrations(protocol_context_t *pc);
static void	protocol_command_get_settings_protection(protocol_context_t *pc);
static void	protocol_command_get_settings_capacitors(protocol_context_t *pc);
static void	protocol_command_get_settings_filters(protocol_context_t *pc);
static void	protocol_command_get_work_state(protocol_context_t *pc);
static void	protocol_command_get_version_info(protocol_context_t *pc);
static void	protocol_command_get_events(protocol_context_t *pc);

/*--------------------------------------------------------------
                       PRIVATE TYPES
--------------------------------------------------------------*/

typedef void (*PFC_COMMAND_CALLBACK)(protocol_context_t *pc); /**<  */

static PFC_COMMAND_CALLBACK handlers_array[]=
{
	protocol_command_test,
	protocol_command_switch_on_off,
	protocol_command_get_ac_active,
	protocol_command_get_adc_active_raw,
	protocol_command_get_oscillog,
	protocol_command_get_net_params,

	protocol_command_set_settings_calbrations,
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

float OSC_DATA[OSC_CHANNEL_NUMBER][OSCILLOG_TRANSFER_SIZE];

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

static void	protocol_command_switch_on_off(protocol_context_t *pc)
{
	struct s_command_switch_on_off *req=0;
	struct s_answer_switch_on_off *pack=0;

	preprocess_answer((void**)&req, (void**)&pack, pc, sizeof(struct s_answer_switch_on_off), PFC_COMMAND_SWITCH_ON_OFF);
	
	pack->result = 1;
	if(pfc_apply_command(req->command, req->data) != PFC_SUCCESS)
	{
		pack->result=0;
	}

	package_set_data_len(&pc->packageToSend, sizeof(struct s_answer_switch_on_off));
	protocol_send_package(pc); 
}

static void	protocol_command_get_adc_active(protocol_context_t *pc)
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

	package_set_data_len(&pc->packageToSend, sizeof(struct s_answer_get_adc_active));
	protocol_send_package(pc); 	
}

//================================================================================

static void	protocol_command_get_net_params(protocol_context_t *pc)
{
	struct s_command_get_net_params *req=0;
	struct s_answer_get_net_params *pack=0;

	preprocess_answer((void**)&req, (void**)&pack, pc, sizeof(struct s_answer_get_net_params), PFC_COMMAND_GET_NET_PARAMS);
	
	U_0Hz[PFC_NCHAN];
  I_0Hz[PFC_NCHAN];
  U_phase[PFC_NCHAN];
  thdu[PFC_NCHAN];
	
	pack->period_fact = PFC.period_fact;
	pack->U0Hz_A = PFC.U_0Hz[0];  //Постоянная составляющая
	pack->U0Hz_B = PFC.U_0Hz[1];
	pack->U0Hz_C = PFC.U_0Hz[2];
	pack->I0Hz_A = PFC.I_0Hz[0];  //Постоянная составляющая
	pack->I0Hz_B = PFC.I_0Hz[1];
	pack->I0Hz_C = PFC.I_0Hz[2];

	pack->thdu_A = PFC.thdu[0];
	pack->thdu_B = PFC.thdu[1];
	pack->thdu_C = PFC.thdu[2];

	pack->U_phase_A = PFC.U_phase[0];
	pack->U_phase_B = PFC.U_phase[1];
	pack->U_phase_C = PFC.U_phase[2];

	package_set_data_len(&pc->packageToSend, sizeof(struct s_answer_get_net_params));
	protocol_send_package(pc); 
}

//================================================================================

static void	protocol_command_get_adc_active_raw(protocol_context_t *pc)
{
	struct s_command_get_adc_active_raw *req=0;
	struct s_answer_get_adc_active_raw *pack=0;

	preprocess_answer((void**)&req, (void**)&pack, pc, sizeof(struct s_answer_get_adc_active_raw), PFC_COMMAND_GET_ADC_ACTIVE_RAW);
		
	pack->ADC_UD = PFC.adc.active_raw[ADC_UD];
	pack->ADC_U_A = PFC.adc.active_raw[ADC_U_A];
	pack->ADC_U_B = PFC.adc.active_raw[ADC_U_B];
	pack->ADC_U_C = PFC.adc.active_raw[ADC_U_C];
	pack->ADC_I_A = PFC.adc.active_raw[ADC_I_A];
	pack->ADC_I_B = PFC.adc.active_raw[ADC_I_B];
	pack->ADC_I_C = PFC.adc.active_raw[ADC_I_C];
	pack->ADC_I_ET = PFC.adc.active_raw[ADC_I_ET];
	pack->ADC_I_TEMP1 = PFC.adc.active_raw[ADC_I_TEMP1];
	pack->ADC_I_TEMP2 = PFC.adc.active_raw[ADC_I_TEMP2];
	pack->ADC_EMS_A = PFC.adc.active_raw[ADC_EMS_A];
	pack->ADC_EMS_B = PFC.adc.active_raw[ADC_EMS_B];
	pack->ADC_EMS_C = PFC.adc.active_raw[ADC_EMS_C];
	pack->ADC_EMS_I = PFC.adc.active_raw[ADC_EMS_I];

	package_set_data_len(&pc->packageToSend, sizeof(struct s_answer_get_adc_active_raw));
	protocol_send_package(pc); 
}

//================================================================================

static void	protocol_command_get_work_state(protocol_context_t *pc)
{
	struct s_command_get_work_state *req=0;
	struct s_answer_get_work_state *pack=0;

	preprocess_answer((void**)&req, (void**)&pack, pc, sizeof(struct s_answer_get_work_state), PFC_COMMAND_GET_WORK_STATE);

	system_set_time(req->currentTime);

	pack->state = pfc_get_state();
	pack->activeChannels[0] = PFC.settings.PWM.activeChannels[0];
	pack->activeChannels[1] = PFC.settings.PWM.activeChannels[1];
	pack->activeChannels[2] = PFC.settings.PWM.activeChannels[2];

	package_set_data_len(&pc->packageToSend, sizeof(struct s_answer_get_work_state));
	protocol_send_package(pc); 
}

//================================================================================


static void	protocol_command_get_version_info(protocol_context_t *pc)
{
	struct s_command_get_version_info *req=0;
	struct s_answer_get_version_info *pack=0;

	preprocess_answer((void**)&req, (void**)&pack, pc, sizeof(struct s_answer_get_version_info), PFC_COMMAND_GET_VERSION_INFO);
	
	pack->major = FW_VERSION_MAJOR;
	pack->minor = FW_VERSION_MINOR;
	pack->micro = FW_VERSION_MICRO;
	pack->build = FW_VERSION_BUILD;

	uint16_t DD = 0;
	if (__DATE__[4] >= '0') DD = (__DATE__[4] - '0');
	DD = DD * 10 + (__DATE__[5] - '0');
	uint16_t MM = BUILD_MONTH_CH1;
	uint16_t YYYY = (__DATE__[7] - '0') * 1000 + (__DATE__[8] - '0') * 100 + (__DATE__[9] - '0') * 10 + (__DATE__[10] - '0');
	uint16_t HH = (__TIME__[0] - '0') * 10 + (__TIME__[1] - '0');
	uint16_t MN = (__TIME__[3] - '0') * 10 + (__TIME__[4] - '0');
	uint16_t SS = (__TIME__[6] - '0') * 10 + (__TIME__[7] - '0');

	pack->day = DD;
	pack->month = MM;
	pack->year = YYYY;
	pack->hour = HH;
	pack->minute = MN;
	pack->second = SS;

	package_set_data_len(&pc->packageToSend, sizeof(struct s_answer_get_version_info));
	protocol_send_package(pc); 
}

//================================================================================

static void	protocol_command_get_oscillog(protocol_context_t *pc)
{
	struct s_command_get_oscillog *req=0;
	struct s_answer_get_oscillog *pack=0;

	preprocess_answer((void**)&req, (void**)&pack, pc, sizeof(struct s_answer_get_oscillog), PFC_COMMAND_GET_OSCILLOG);
	
	if (req->num > OSC_CHANNEL_NUMBER)
	{
			protocol_error_handle(pc, package_get_command(&pc->receivedPackage));
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


	package_set_data_len(&pc->packageToSend, sizeof(struct s_answer_get_oscillog));
	protocol_send_package(pc); 
}

//================================================================================

static void	protocol_command_get_settings_calibrations(protocol_context_t *pc)
{
	struct s_command_get_settings_calibrations *req=0;
	struct s_answer_get_settings_calibrations *pack=0;

	preprocess_answer((void**)&req, (void**)&pack, pc, sizeof(struct s_answer_get_settings_calibrations), PFC_COMMAND_GET_SETTINGS_CALIBRATIONS);
	
	int i;
	for (i = 0; i < ADC_CHANNEL_NUMBER; i++)
	{
			pack->calibration[i] = PFC.settings.CALIBRATIONS.calibration[i];  //!< Калибровки для каналов
			pack->offset[i] = PFC.settings.CALIBRATIONS.offset[i];            //!< Смещения для каналов
	}

	package_set_data_len(&pc->packageToSend, sizeof(struct s_answer_get_settings_calibrations));
	protocol_send_package(pc); 
}

//================================================================================

static void	protocol_command_set_settings_calbrations(protocol_context_t *pc)
{
	struct s_command_set_settings_calbrations *req=0;
	struct s_answer_set_settings_calbrations *pack=0;

	preprocess_answer((void**)&req, (void**)&pack, pc, sizeof(struct s_answer_set_settings_calbrations), PFC_COMMAND_SET_SETTINGS_CALIBRATIONS);
	
	int i;
	for (i = 0; i < ADC_CHANNEL_NUMBER; i++)
	{
			PFC.settings.CALIBRATIONS.calibration[i] = req->calibration[i];  //!< Калибровки для каналов
			PFC.settings.CALIBRATIONS.offset[i] = req->offset[i];            //!< Смещения для каналов
	}

	package_set_data_len(&pc->packageToSend, sizeof(struct s_answer_set_settings_calbrations));
	protocol_send_package(pc); 
}

//================================================================================
static void	protocol_command_get_settings_protection(protocol_context_t *pc)
{
	struct s_command_get_settings_protection *req=0;
	struct s_answer_get_settings_protection *pack=0;

	preprocess_answer((void**)&req, (void**)&pack, pc, sizeof(struct s_answer_get_settings_protection), PFC_COMMAND_GET_SETTINGS_PROTECTION);
	
	pack->Ud_min = PFC.settings.PROTECTION.Ud_min;            //!< Граничные значения для Ud
	pack->Ud_max = PFC.settings.PROTECTION.Ud_max;            //!< Граничные значения для Ud
	pack->temperature = PFC.settings.PROTECTION.temperature;  //!< Граничные значения для Температуры
	pack->U_min = PFC.settings.PROTECTION.U_min;              //!< Граничные значения для напряжения
	pack->U_max = PFC.settings.PROTECTION.U_max;
	pack->Fnet_min = PFC.settings.PROTECTION.Fnet_min;      //!< минимальная частота сети
	pack->Fnet_max = PFC.settings.PROTECTION.Fnet_max;      //!< максимальная частота сети
	pack->I_max_rms = PFC.settings.PROTECTION.I_max_rms;    //!< Максимальное граничное значение тока фильтра по RMS
	pack->I_max_peak = PFC.settings.PROTECTION.I_max_peak;  //!< Максимальное граничное мгновенное значение тока фильтра

	package_set_data_len(&pc->packageToSend, sizeof(struct s_answer_get_settings_protection));
	protocol_send_package(pc); 
}

//================================================================================

static void	protocol_command_set_settings_protection(protocol_context_t *pc)
{
	struct s_command_set_settings_protection *req=0;
	struct s_answer_set_settings_protection *pack=0;

	preprocess_answer((void**)&req, (void**)&pack, pc, sizeof(struct s_answer_set_settings_protection), PFC_COMMAND_SET_SETTINGS_PROTECTION);
	
	PFC.settings.PROTECTION.Ud_min = req->Ud_min;            //!< Граничные значения для Ud
	PFC.settings.PROTECTION.Ud_max = req->Ud_max;            //!< Граничные значения для Ud
	PFC.settings.PROTECTION.temperature = req->temperature;  //!< Граничные значения для Температуры
	PFC.settings.PROTECTION.U_min = req->U_min;              //!< Граничные значения для напряжения
	PFC.settings.PROTECTION.U_max = req->U_max;
	PFC.settings.PROTECTION.Fnet_min = req->Fnet_min;      //!< минимальная частота сети
	PFC.settings.PROTECTION.Fnet_max = req->Fnet_max;      //!< максимальная частота сети
	PFC.settings.PROTECTION.I_max_rms = req->I_max_rms;    //!< Максимальное граничное значение тока фильтра по RMS
	PFC.settings.PROTECTION.I_max_peak = req->I_max_peak;  //!< Максимальное граничное мгновенное значение тока фильтра

	package_set_data_len(&pc->packageToSend, sizeof(struct s_answer_set_settings_protection));
	protocol_send_package(pc); 
}

//================================================================================
static void	protocol_command_get_settings_capacitors(protocol_context_t *pc)
{
	struct s_command_get_settings_capacitors *req=0;
	struct s_answer_get_settings_capacitors *pack=0;

	preprocess_answer((void**)&req, (void**)&pack, pc, sizeof(struct s_answer_get_settings_capacitors), PFC_COMMAND_GET_SETTINGS_CAPACITORS);
	
	pack->ctrlUd_Kp = PFC.settings.CAPACITORS.ctrlUd_Kp;
	pack->ctrlUd_Ki = PFC.settings.CAPACITORS.ctrlUd_Ki;
	pack->ctrlUd_Kd = PFC.settings.CAPACITORS.ctrlUd_Kd;
	pack->Ud_nominal = PFC.settings.CAPACITORS.Ud_nominal;
	pack->Ud_precharge = PFC.settings.CAPACITORS.Ud_precharge;

	package_set_data_len(&pc->packageToSend, sizeof(struct s_answer_get_settings_capacitors));
	protocol_send_package(pc); 
}

//================================================================================
static void	protocol_command_set_settings_capacitors(protocol_context_t *pc)
{
	struct s_command_set_settings_capacitors *req=0;
	struct s_answer_set_settings_capacitors *pack=0;

	preprocess_answer((void**)&req, (void**)&pack, pc, sizeof(struct s_answer_set_settings_capacitors), PFC_COMMAND_SET_SETTINGS_CAPACITORS);
	
	PFC.settings.CAPACITORS.ctrlUd_Kp = req->ctrlUd_Kp;
	PFC.settings.CAPACITORS.ctrlUd_Ki = req->ctrlUd_Ki;
	PFC.settings.CAPACITORS.ctrlUd_Kd = req->ctrlUd_Kd;
	PFC.settings.CAPACITORS.Ud_nominal = req->Ud_nominal;
	PFC.settings.CAPACITORS.Ud_precharge = req->Ud_precharge;

	package_set_data_len(&pc->packageToSend, sizeof(struct s_answer_set_settings_capacitors));
	protocol_send_package(pc); 
}

//================================================================================
static void	protocol_command_get_settings_filters(protocol_context_t *pc)
{
	struct s_command_get_settings_filters *req=0;
	struct s_answer_get_settings_filters *pack=0;

	preprocess_answer((void**)&req, (void**)&pack, pc, sizeof(struct s_answer_get_settings_filters), PFC_COMMAND_GET_SETTINGS_FILTERS);
	
	pack->K_I = PFC.settings.FILTERS.K_I;
	pack->K_U = PFC.settings.FILTERS.K_U;
	pack->K_UD = PFC.settings.FILTERS.K_UD;

	package_set_data_len(&pc->packageToSend, sizeof(struct s_answer_get_settings_filters));
	protocol_send_package(pc); 
}

//================================================================================
static void	protocol_command_set_settings_filters(protocol_context_t *pc)
{
	struct s_command_set_settings_filters *req=0;
	struct s_answer_set_settings_filters *pack=0;

	preprocess_answer((void**)&req, (void**)&pack, pc, sizeof(struct s_answer_set_settings_filters), PFC_COMMAND_SET_SETTINGS_FILTERS);
	
	PFC.settings.FILTERS.K_I = req->K_I;
	PFC.settings.FILTERS.K_U = req->K_U;
	PFC.settings.FILTERS.K_UD = req->K_UD;

	package_set_data_len(&pc->packageToSend, sizeof(struct s_answer_set_settings_filters));
	protocol_send_package(pc); 
}

//================================================================================
static void	protocol_command_get_events(protocol_context_t *pc)
{
	struct s_command_get_events *req=0;
	struct s_answer_get_events *pack=0;

	preprocess_answer((void**)&req, (void**)&pack, pc, sizeof(struct s_answer_get_events), PFC_COMMAND_GET_EVENTS);
	
	pack->num = EventsGet(req->afterIndex, MAX_NUM_TRANSFERED_EVENTS, pack->events);

	package_set_data_len(&pc->packageToSend, sizeof(struct s_answer_get_events));
	protocol_send_package(pc); 
}


static void	protocol_command_test(protocol_context_t *pc)
{
	/* TEST */
}

/*--------------------------------------------------------------
                       PUBLIC FUNCTIONS
--------------------------------------------------------------*/

void prothandlers_init(SciPort *_port)
{
    protocol_init(&PFC.protocol, p_mode_client, handlers_array, PFC_COMMAND_COUNT, _port);
}
