/**
 * @file device.cpp
 * @author Stanislav Karpikov
 * @brief Implements PFC device control and configuration via the interface
 */

/*--------------------------------------------------------------
                       INCLUDES
--------------------------------------------------------------*/

#include "device.h"
#include "device_definition.h"

/*--------------------------------------------------------------
                       NAMESPACES
--------------------------------------------------------------*/

using namespace PFCconfig;
using namespace PFCconfig::ADC;
using namespace PFCconfig::Interface;

/*--------------------------------------------------------------
               PRIVATE CLASS FUNCTIONS
--------------------------------------------------------------*/

PFC::PFC():
    _interface(new ADFSerialInterface(Q_NULLPTR)),
    handlers(enum_int(pfc_interface_commands_t::PFC_COMMAND_COUNT)),
    thread(new QThread())
{
        _interface->moveToThread(thread);
        connect(thread,  SIGNAL(finished()), thread, SLOT(deleteLater()));
        connect(thread,  SIGNAL(started()), _interface, SLOT(run()));
        connect(_interface,SIGNAL(ConnectionChanged(bool)),this,SLOT(ConnectionChanged(bool)));
        thread->start();

        for (uint i = 0; i < enum_int(pfc_interface_commands_t::PFC_COMMAND_COUNT); i++)
            handlers[i] = std::bind(&PFC::protocol_unknow_command_handle, this, std::placeholders::_1);

        handlers[enum_int(pfc_interface_commands_t::PFC_COMMAND_GET_ADC_ACTIVE)] =
                std::bind(&PFC::protocol_GET_ADC_ACTIVE, this, std::placeholders::_1);
        handlers[enum_int(pfc_interface_commands_t::PFC_COMMAND_SWITCH_ON_OFF)] =
                std::bind(&PFC::protocol_SWITCH_ON_OFF, this, std::placeholders::_1);
        handlers[enum_int(pfc_interface_commands_t::PFC_COMMAND_GET_ADC_ACTIVE_RAW)] =
                std::bind(&PFC::protocol_GET_ADC_ACTIVE_RAW, this, std::placeholders::_1);
        handlers[enum_int(pfc_interface_commands_t::PFC_COMMAND_GET_OSCILLOG)] =
                std::bind(&PFC::protocol_GET_OSCILOG, this, std::placeholders::_1);
        handlers[enum_int(pfc_interface_commands_t::PFC_COMMAND_GET_NET_PARAMS)] =
                std::bind(&PFC::protocol_GET_NET_PARAMS, this, std::placeholders::_1);
        handlers[enum_int(pfc_interface_commands_t::PFC_COMMAND_GET_WORK_STATE)] =
                std::bind(&PFC::protocol_GET_WORK_STATE, this, std::placeholders::_1);
        handlers[enum_int(pfc_interface_commands_t::PFC_COMMAND_GET_VERSION_INFO)] =
                std::bind(&PFC::protocol_GET_VERSION_INFO, this, std::placeholders::_1);
        handlers[enum_int(pfc_interface_commands_t::PFC_COMMAND_GET_EVENTS)] =
                std::bind(&PFC::protocol_GET_EVENTS, this, std::placeholders::_1);

        handlers[enum_int(pfc_interface_commands_t::PFC_COMMAND_GET_SETTINGS_CALIBRATIONS)] =
                std::bind(&PFC::protocol_GET_SETTINGS_CALIBRATIONS, this, std::placeholders::_1);
        handlers[enum_int(pfc_interface_commands_t::PFC_COMMAND_GET_SETTINGS_PROTECTION)] =
                std::bind(&PFC::protocol_GET_SETTINGS_PROTECTION, this, std::placeholders::_1);
        handlers[enum_int(pfc_interface_commands_t::PFC_COMMAND_GET_SETTINGS_CAPACITORS)] =
                std::bind(&PFC::protocol_GET_SETTINGS_CAPACITORS, this, std::placeholders::_1);
        handlers[enum_int(pfc_interface_commands_t::PFC_COMMAND_GET_SETTINGS_FILTERS)] =
                std::bind(&PFC::protocol_GET_SETTINGS_FILTERS, this, std::placeholders::_1);

        handlers[enum_int(pfc_interface_commands_t::PFC_COMMAND_SET_SETTINGS_CALIBRATIONS)] =
                std::bind(&PFC::protocol_SET_SETTINGS_CALIBRATIONS, this, std::placeholders::_1);
        handlers[enum_int(pfc_interface_commands_t::PFC_COMMAND_SET_SETTINGS_PROTECTION)] =
                std::bind(&PFC::protocol_SET_SETTINGS_PROTECTION, this, std::placeholders::_1);
        handlers[enum_int(pfc_interface_commands_t::PFC_COMMAND_SET_SETTINGS_CAPACITORS)] =
                std::bind(&PFC::protocol_SET_SETTINGS_CAPACITORS, this, std::placeholders::_1);
        handlers[enum_int(pfc_interface_commands_t::PFC_COMMAND_SET_SETTINGS_FILTERS)] =
                std::bind(&PFC::protocol_SET_SETTINGS_FILTERS, this, std::placeholders::_1);

        connect(_interface,SIGNAL(Message(quint8,quint8,quint8,QString)),
                this, SIGNAL(Message(quint8,quint8,quint8,QString)));

        Message(MESSAGE_TYPE_GENERAL,MESSAGE_NORMAL,MESSAGE_TARGET_DEBUG,"Поток АДФ запущен");
    }

void protocol_unknow_command_handle(std::vector<uint8_t> data){
    Q_UNUSED(data)
    Message(MESSAGE_TYPE_GENERAL,MESSAGE_NORMAL,MESSAGE_TARGET_DEBUG,
            QString("Неизвестная команда"));
}

/*--------------------------------------------------------------
         PUBLIC CLASS FUNCTIONS::REQUESTS
--------------------------------------------------------------*/

void updateNetVoltage(){
    START_REQUEST(GET_ADC_ACTIVE)
    END_REQUEST(GET_ADC_ACTIVE);
}
void updateWorkState(uint64_t currentTime){
    START_REQUEST(GET_WORK_STATE);
    req.currentTime=currentTime;
    END_REQUEST(GET_WORK_STATE);
}
void updateEvents(uint64_t afterIndex){
    START_REQUEST(GET_EVENTS);
    req.afterIndex=afterIndex;
    END_REQUEST(GET_EVENTS);
}
void updateVersionInfo(){
    START_REQUEST(GET_VERSION_INFO)
    END_REQUEST(GET_VERSION_INFO);
}
void updateNetVoltageRAW(){
    START_REQUEST(GET_ADC_ACTIVE_RAW)
    END_REQUEST(GET_ADC_ACTIVE_RAW);
}
void updateOscillog(uint16_t channel){
    START_REQUEST(GET_OSCILOG);
    req.num=channel;
    END_REQUEST(GET_OSCILOG);
}
void updateNetParams(){
    START_REQUEST(GET_NET_PARAMS)
    END_REQUEST(GET_NET_PARAMS);
}
void updateSettingsCalibrations(){
    START_REQUEST(GET_SETTINGS_CALIBRATIONS);
    END_REQUEST(GET_SETTINGS_CALIBRATIONS);
}
void updateSettingsProtection(){
    START_REQUEST(GET_SETTINGS_PROTECTION);
    END_REQUEST(GET_SETTINGS_PROTECTION);
}
void updateSettingsCapacitors(){
    START_REQUEST(GET_SETTINGS_CAPACITORS);
    END_REQUEST(GET_SETTINGS_CAPACITORS);
}
void updateSettingsFilters(){
    START_REQUEST(GET_SETTINGS_FILTERS);
    END_REQUEST(GET_SETTINGS_FILTERS);
}

/*--------------------------------------------------------------
         PUBLIC CLASS FUNCTIONS::PARAMETERS WRITE
--------------------------------------------------------------*/

void writeSettingsCalibrations(
        QVector<float> calibration,
        QVector<float> offset
        ){
    START_REQUEST(SET_SETTINGS_CALIBRATIONS);
    for(int i=0;i<calibration.size();i++)req.calibration[i]=calibration[i];
    for(int i=0;i<offset.size();i++)req.offset[i]=offset[i];
    END_REQUEST(SET_SETTINGS_CALIBRATIONS);
}

void writeSettingsProtection(
        float Ud_min, //!< Граничные значения для Ud
        float Ud_max, //!< Граничные значения для Ud
        float temperature, //!< Граничные значения для Температуры
        float U_min, //!< Граничные значения для напряжения
        float U_max,
        float Fnet_min, //!< минимальная частота сети
        float Fnet_max, //!< максимальная частота сети
        float I_max_rms, //!< Максимальное граничное значение тока фильтра по RMS
        float I_max_peak //!< Максимальное граничное мгновенное значение тока фильтра
        ){
    START_REQUEST(SET_SETTINGS_PROTECTION);
        req.Ud_min=Ud_min;
        req.Ud_max=Ud_max;
        req.temperature=temperature;
        req.U_min=U_min;
        req.U_max=U_max;
        req.Fnet_min=Fnet_min;
        req.Fnet_max=Fnet_max;
        req.I_max_rms=I_max_rms;
        req.I_max_peak=I_max_peak;
    END_REQUEST(SET_SETTINGS_PROTECTION);
}

void writeSettingsCapacitors(
        float ctrlUd_Kp, //!< Управление накачкой
        float ctrlUd_Ki,
        float ctrlUd_Kd,
        float Ud_nominal,
        float Ud_precharge){
    START_REQUEST(SET_SETTINGS_CAPACITORS);
        req.ctrlUd_Kp=ctrlUd_Kp;
        req.ctrlUd_Ki=ctrlUd_Ki;
        req.ctrlUd_Kd=ctrlUd_Kd;
        req.Ud_nominal=Ud_nominal;
        req.Ud_precharge=Ud_precharge;
    END_REQUEST(SET_SETTINGS_CAPACITORS);
}

void writeSettingsFilters(
        float K_I,
        float K_U,
        float K_UD){
    START_REQUEST(SET_SETTINGS_FILTERS);
        req.K_I         =K_I;
        req.K_U         =K_U;
        req.K_UD        =K_UD;
    END_REQUEST(SET_SETTINGS_FILTERS);
}

void writeSwitchOnOff(uint32_t command,uint32_t data){
    START_REQUEST(SWITCH_ON_OFF)
         req.command=command;
         req.data=data;
    END_REQUESTP(SWITCH_ON_OFF,ADFMessagePriority::HIGH);
}

/*--------------------------------------------------------------
         PRIVATE CLASS FUNCTIONS::ANSWER
--------------------------------------------------------------*/

void PFC::getAnswer(bool is_timeout, PackageCommand *pc){
    //qDebug() << Q_FUNC_INFO;
    if(is_timeout)return;
    if(pc->package_in->command()>=PFC_COMMAND_COUNT){
        Message(MESSAGE_TYPE_CONNECTION,MESSAGE_NORMAL,MESSAGE_TARGET_DEBUG,"Неизвестный ответ");
        return;
    }
    //qDebug() << pc->package_in->command();
    if(pc->package_in->error()){
        Message(MESSAGE_TYPE_GLOBALWARNING,MESSAGE_NORMAL,MESSAGE_TARGET_DEBUG,"Ошибка выполнения команды");
        return;
    }
    std::vector<uint8_t> vec(pc->package_in->data());

    //Add try catch
    handlers[pc->package_in->command()](vec);
    //QTimer::singleShot(100, [this,apc]() { handlers[pc->package_in->command()](&apc); } );
    //handlers[pc->package_in->command()](pc);
}

/*--------------------------------------------------------------
         PRIVATE CLASS FUNCTIONS::HANDLERS
--------------------------------------------------------------*/

void protocol_SWITCH_ON_OFF()
    Q_UNUSED(data)
    struct sANSWER_##COMM* pANSWER=(struct sANSWER_##COMM*)(&(data[0]));
    Q_UNUSED(pANSWER)
    emit setSwitchOnOff(pANSWER->result);
}
//==================================================================
HANDLER(GET_ADC_ACTIVE)
     emit setNetVoltage(pANSWER->ADC_UD,
                        pANSWER->ADC_U_A,
                        pANSWER->ADC_U_B,
                        pANSWER->ADC_U_C,
                        pANSWER->ADC_I_A,
                        pANSWER->ADC_I_B,
                        pANSWER->ADC_I_C,
                        pANSWER->ADC_I_ET,
                        pANSWER->ADC_I_TEMP1,
                        pANSWER->ADC_I_TEMP2,
                        pANSWER->ADC_EMS_A,
                        pANSWER->ADC_EMS_B,
                        pANSWER->ADC_EMS_C,
                        pANSWER->ADC_EMS_I,
                        pANSWER->ADC_MATH_A,
                        pANSWER->ADC_MATH_B,
                        pANSWER->ADC_MATH_C
                        );
}
//==================================================================
HANDLER(GET_ADC_ACTIVE_RAW)
     emit setNetVoltageRAW(pANSWER->ADC_UD,
                           pANSWER->ADC_U_A,
                           pANSWER->ADC_U_B,
                           pANSWER->ADC_U_C,
                           pANSWER->ADC_I_A,
                           pANSWER->ADC_I_B,
                           pANSWER->ADC_I_C,
                           pANSWER->ADC_I_ET,
                           pANSWER->ADC_I_TEMP1,
                           pANSWER->ADC_I_TEMP2,
                           pANSWER->ADC_EMS_A,
                           pANSWER->ADC_EMS_B,
                           pANSWER->ADC_EMS_C,
                           pANSWER->ADC_EMS_I
                           );
}
//==================================================================
HANDLER(GET_WORK_STATE)
     emit setWorkState(pANSWER->state,
                       pANSWER->activeChannels[0],
                       pANSWER->activeChannels[1],
                       pANSWER->activeChannels[2]);
}
//==================================================================
HANDLER(GET_EVENTS)
     if(pANSWER->num==0)return;
     QVector<struct sEventRecord> ev;
     for(int i=0;i<pANSWER->num;i++){
         ev.append(pANSWER->events[i]);
     }
     emit setEvents(ev);
}
//==================================================================
HANDLER(GET_VERSION_INFO)
     emit setVersionInfo(
        pANSWER->major,
        pANSWER->minor,
        pANSWER->micro,
        pANSWER->build,
        pANSWER->day,
        pANSWER->month,
        pANSWER->year,
        pANSWER->hour,
        pANSWER->minute,
        pANSWER->second);
}
//==================================================================
inline float line_float(float x, float x1, float y1, float x2, float y2) {
    float a;
    a = (y2 - y1) / (x2 - x1);
    return y1 + a * (x - x1);
}

#define shifted(x, offset) ((float)(x) + (float)(offset))
#define unshifted(x, offset) ((float)(x) - (float)(offset))

void interpolate_shift_resize_float(QVector<double> &in, unsigned int in_size,
        float offset, QVector<double> &out) {
    float x,x1, x2, y1, y2;
    out.resize(in_size);
    int32_t i,j,j_1;
    j=offset;
    j_1=j+1;
    float diff=offset-j;
    for (i = 0; i < in_size; i++,j++) {
        while(j>=in_size)j-=in_size;
        while(j_1>=in_size)j_1-=in_size;

        x1=j;
        y1=in[j];

        x2=j_1;
        y2=in[j_1];

        x=diff+x1;

        out[i] = line_float(x, x1, y1, x2, y2);
    }
}
HANDLER(GET_OSCILOG)
     float astep=(pANSWER->max-pANSWER->min)/255.0;
     uint16_t len    = pANSWER->len&0xFFFF;
     uint16_t phase  = (pANSWER->len>>16)&0xFFFF;
     QVector<double> yval(128);
     Q_UNUSED(phase);
     //qDebug()<<"Osc get";
     if(len>128){
         qDebug()<<"Wrong len";
         return;
     }

     for(int i=0;i<len;i++){
         yval[i]=pANSWER->min+astep*pANSWER->data[i];
     }

     setOscillog(pANSWER->ch,yval.toStdVector());
}

 HANDLER(GET_NET_PARAMS)
      emit setNetParams(pANSWER->period_fact,
                        pANSWER->U0Hz_A,
                        pANSWER->U0Hz_B,
                        pANSWER->U0Hz_C,
                        pANSWER->I0Hz_A,
                        pANSWER->I0Hz_B,
                        pANSWER->I0Hz_C,
                        pANSWER->thdu_A,
                        pANSWER->thdu_B,
                        pANSWER->thdu_C,
                        pANSWER->U_phase_A,
                        pANSWER->U_phase_B,
                        pANSWER->U_phase_C);
 }

HANDLER(GET_SETTINGS_CALIBRATIONS)
    QVector<float> calibration(ADC_CHANNEL_NUMBER);
    QVector<float> offset(ADC_CHANNEL_NUMBER);
    for(int i=0;i<calibration.size();i++)   calibration[i]=pANSWER->calibration[i];
    for(int i=0;i<offset.size();i++)        offset[i]=pANSWER->offset[i];
    setSettingsCalibrations(
            calibration,
            offset
            );
}

HANDLER(GET_SETTINGS_PROTECTION)
    setSettingsProtection(
        pANSWER->Ud_min,
        pANSWER->Ud_max,
        pANSWER->temperature,
        pANSWER->U_min,
        pANSWER->U_max,
        pANSWER->Fnet_min,
        pANSWER->Fnet_max,
        pANSWER->I_max_rms,
        pANSWER->I_max_peak);
}

HANDLER(GET_SETTINGS_CAPACITORS)
    setSettingsCapacitors(
        pANSWER->ctrlUd_Kp,
        pANSWER->ctrlUd_Ki,
        pANSWER->ctrlUd_Kd,
        pANSWER->Ud_nominal,
        pANSWER->Ud_precharge);
}

HANDLER(GET_SETTINGS_FILTERS)
    setSettingsFilters(
        pANSWER->K_I,
        pANSWER->K_U,
        pANSWER->K_UD);
}

HANDLER(SET_SETTINGS_CALIBRATIONS)
    ansSettingsCalibrations(true);
}

HANDLER(SET_SETTINGS_PROTECTION)
    ansSettingsProtection(true);
}

HANDLER(SET_SETTINGS_FILTERS)
    ansSettingsFilters(true);
}

HANDLER(SET_SETTINGS_CAPACITORS)
    ansSettingsCapacitors(true);
}
