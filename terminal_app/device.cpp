/**
 * @file device.cpp
 * @author Stanislav Karpikov
 * @brief Implements PFC device control and configuration via the interface
 */

/*--------------------------------------------------------------
                       INCLUDES
--------------------------------------------------------------*/

#include "device.h"

/*--------------------------------------------------------------
                       NAMESPACES
--------------------------------------------------------------*/

using namespace PFCconfig;
using namespace PFCconfig::ADC;
using namespace PFCconfig::Interface;
using namespace PFCconfig::Events;

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

void PFC::protocol_unknow_command_handle(package_general* answer){
    Q_UNUSED(answer)
    Message(MESSAGE_TYPE_GENERAL,MESSAGE_NORMAL,MESSAGE_TARGET_DEBUG,
            QString("Неизвестная команда"));
}

/*--------------------------------------------------------------
         PUBLIC CLASS FUNCTIONS::REQUESTS
--------------------------------------------------------------*/

void PFC::EndRequest(package_general &req, pfc_interface_commands_t command, ADFMessagePriority priority)
{
    std::vector<uint8_t> datareq;
    /* TODO: Verify sizeof */
    if(sizeof(req)){
        datareq.assign(reinterpret_cast<uint8_t*>(&req), reinterpret_cast<uint8_t*>(&req)+sizeof(req));
    }
    PackageCommand *p = new PackageCommand();
    p->package_out->fill(
                priority,
                Sender::GUI,
                static_cast<uint8_t>(enum_int(command)),
                datareq
             );
    _interface->enqueueCommand(p);
    connect(p, &PackageCommand::complete, this, &PFC::getAnswer/*,Qt::DirectConnection*/);
}

void PFC::updateNetVoltage(){
    command_get_adc_active req;
    EndRequest(req, pfc_interface_commands_t::PFC_COMMAND_GET_ADC_ACTIVE);
}
void PFC::updateWorkState(uint64_t currentTime){
    command_get_work_state req;
    req.currentTime=currentTime;
    EndRequest(req, pfc_interface_commands_t::PFC_COMMAND_GET_WORK_STATE);
}
void PFC::updateEvents(uint64_t afterIndex){
    command_get_events req;
    req.after_index=afterIndex;
    EndRequest(req, pfc_interface_commands_t::PFC_COMMAND_GET_EVENTS);
}
void PFC::updateVersionInfo(){
    command_get_version_info req;
    EndRequest(req, pfc_interface_commands_t::PFC_COMMAND_GET_VERSION_INFO);
}
void PFC::updateNetVoltageRAW(){
    command_get_adc_active_raw req;
    EndRequest(req, pfc_interface_commands_t::PFC_COMMAND_GET_ADC_ACTIVE_RAW);
}
void PFC::updateOscillog(PFCOscillogCnannel channel){
    command_get_oscillog req;
    req.num=static_cast<decltype(req.num)>(enum_int(channel));
    EndRequest(req, pfc_interface_commands_t::PFC_COMMAND_GET_OSCILLOG);
}
void PFC::updateNetParams(){
    command_get_net_params req;
    EndRequest(req, pfc_interface_commands_t::PFC_COMMAND_GET_NET_PARAMS);
}
void PFC::updateSettingsCalibrations(){
    command_get_settings_calibrations req;
    EndRequest(req, pfc_interface_commands_t::PFC_COMMAND_GET_SETTINGS_CALIBRATIONS);
}
void PFC::updateSettingsProtection(){
    command_get_settings_protection req;
    EndRequest(req, pfc_interface_commands_t::PFC_COMMAND_GET_SETTINGS_PROTECTION);
}
void PFC::updateSettingsCapacitors(){
    command_get_settings_capacitors req;
    EndRequest(req, pfc_interface_commands_t::PFC_COMMAND_GET_SETTINGS_CAPACITORS);
}
void PFC::updateSettingsFilters(){
    command_get_settings_filters req;
    EndRequest(req, pfc_interface_commands_t::PFC_COMMAND_GET_SETTINGS_FILTERS);
}

/*--------------------------------------------------------------
         PUBLIC CLASS FUNCTIONS::PARAMETERS WRITE
--------------------------------------------------------------*/

void PFC::writeSettingsCalibrations(
        std::vector<float> calibration,
        std::vector<float> offset
        ){
    command_set_settings_calibrations req;
    for(uint i=0;i<calibration.size();i++)req.calibration[i]=calibration[i];
    for(uint i=0;i<offset.size();i++)req.offset[i]=offset[i];
    EndRequest(req, pfc_interface_commands_t::PFC_COMMAND_SET_SETTINGS_CALIBRATIONS);
}

void PFC::writeSettingsProtection(
        float Ud_min,
        float Ud_max,
        float temperature,
        float U_min,
        float U_max,
        float Fnet_min,
        float Fnet_max,
        float I_max_rms,
        float I_max_peak
        ){
    command_set_settings_protection req;
        req.Ucap_min=Ud_min;
        req.Ucap_max=Ud_max;
        req.temperature=temperature;
        req.U_min=U_min;
        req.U_max=U_max;
        req.F_min=Fnet_min;
        req.F_max=Fnet_max;
        req.I_max_rms=I_max_rms;
        req.I_max_peak=I_max_peak;
    EndRequest(req, pfc_interface_commands_t::PFC_COMMAND_SET_SETTINGS_PROTECTION);
}

void PFC::writeSettingsCapacitors(
        float ctrlUd_Kp,
        float ctrlUd_Ki,
        float ctrlUd_Kd,
        float Ud_nominal,
        float Ud_precharge){
    command_set_settings_capacitors req;
        req.ctrl_Ucap_Kp=ctrlUd_Kp;
        req.ctrl_Ucap_Ki=ctrlUd_Ki;
        req.ctrl_Ucap_Kd=ctrlUd_Kd;
        req.Ucap_nominal=Ud_nominal;
        req.Ucap_precharge=Ud_precharge;
    EndRequest(req, pfc_interface_commands_t::PFC_COMMAND_SET_SETTINGS_CAPACITORS);
}

void PFC::writeSettingsFilters(
        float K_I,
        float K_U,
        float K_UD){
    command_set_settings_filters req;
        req.K_I         =K_I;
        req.K_U         =K_U;
        req.K_Ucap      =K_UD;
    EndRequest(req, pfc_interface_commands_t::PFC_COMMAND_SET_SETTINGS_FILTERS);
}

void PFC::writeSwitchOnOff(pfc_commands_t command,uint32_t data){
    command_switch_on_off req;
         req.command=static_cast<decltype(req.command)>(command);
         req.data=data;
    EndRequest(req, pfc_interface_commands_t::PFC_COMMAND_SWITCH_ON_OFF, ADFMessagePriority::HIGH);
}

/*--------------------------------------------------------------
         PRIVATE CLASS FUNCTIONS::ANSWER
--------------------------------------------------------------*/

void PFC::getAnswer(bool is_timeout, PackageCommand *pc){
    //qDebug() << Q_FUNC_INFO;
    if(is_timeout)return;

    auto command = static_cast<pfc_interface_commands_t>(pc->package_in->command());

    if(command >= pfc_interface_commands_t::PFC_COMMAND_COUNT){
        Message(MESSAGE_TYPE_CONNECTION,MESSAGE_NORMAL,MESSAGE_TARGET_DEBUG,"Неизвестный ответ");
        return;
    }
    //qDebug() << pc->package_in->command();
    if(pc->package_in->error()){
        Message(MESSAGE_TYPE_GLOBALWARNING,MESSAGE_NORMAL,MESSAGE_TARGET_DEBUG,"Ошибка выполнения команды");
        return;
    }
    uint8_t* command_data = pc->package_in->data().data();

    package_general* package = reinterpret_cast<package_general*>(command_data);

    //Add try catch
    handlers[static_cast<uint>(command)](package);
    //QTimer::singleShot(100, [this,apc]() { handlers[pc->package_in->command()](&apc); } );
}

/*--------------------------------------------------------------
         PRIVATE CLASS FUNCTIONS::HANDLERS
--------------------------------------------------------------*/

void PFC::protocol_SWITCH_ON_OFF(package_general* package)
{
    auto answer = static_cast<answer_switch_on_off*>(package);
    emit setSwitchOnOff(answer->result);
}

void PFC::protocol_GET_ADC_ACTIVE(package_general* package)
{
    auto answer = static_cast<answer_get_adc_active*>(package);
     emit setNetVoltage(answer->ADC_UCAP,
                        answer->ADC_U_A,
                        answer->ADC_U_B,
                        answer->ADC_U_C,
                        answer->ADC_I_A,
                        answer->ADC_I_B,
                        answer->ADC_I_C,
                        answer->ADC_I_ET,
                        answer->ADC_I_TEMP1,
                        answer->ADC_I_TEMP2,
                        answer->ADC_EDC_A,
                        answer->ADC_EDC_B,
                        answer->ADC_EDC_C,
                        answer->ADC_EDC_I,
                        answer->ADC_MATH_A,
                        answer->ADC_MATH_B,
                        answer->ADC_MATH_C
                        );
}

void PFC::protocol_GET_ADC_ACTIVE_RAW(package_general* package)
{
    auto answer = static_cast<answer_get_adc_active_raw*>(package);
     emit setNetVoltageRAW(answer->ADC_UCAP,
                           answer->ADC_U_A,
                           answer->ADC_U_B,
                           answer->ADC_U_C,
                           answer->ADC_I_A,
                           answer->ADC_I_B,
                           answer->ADC_I_C,
                           answer->ADC_I_ET,
                           answer->ADC_I_TEMP1,
                           answer->ADC_I_TEMP2,
                           answer->ADC_EDC_A,
                           answer->ADC_EDC_B,
                           answer->ADC_EDC_C,
                           answer->ADC_EDC_I
                           );
}

void PFC::protocol_GET_WORK_STATE(package_general* package)
{
    auto answer = static_cast<answer_get_work_state*>(package);
     emit setWorkState(answer->state,
                       answer->active_channels[PFC_ACHAN],
                       answer->active_channels[PFC_BCHAN],
                       answer->active_channels[PFC_CCHAN]);
}

void PFC::protocol_GET_EVENTS(package_general* package)
{
    auto answer = static_cast<answer_get_events*>(package);
     if(answer->num==0)return;
     std::list<EventRecord> ev;
     for(int i=0;i<answer->num;i++){
         ev.push_back(answer->events[i]);
     }
     emit setEvents(ev);
}

void PFC::protocol_GET_VERSION_INFO(package_general* package)
{
    auto answer = static_cast<answer_get_version_info*>(package);
     emit setVersionInfo(
        answer->major,
        answer->minor,
        answer->micro,
        answer->build,
        answer->day,
        answer->month,
        answer->year,
        answer->hour,
        answer->minute,
        answer->second);
}

inline float PFC::line_float(float x, float x1, float y1, float x2, float y2)
{
    float a;
    a = (y2 - y1) / (x2 - x1);
    return y1 + a * (x - x1);
}

void PFC::interpolate_shift_resize_float(std::vector<float> &in,
                                         uint32_t in_size,
                                         float offset,
                                         std::vector<float> &out)
{
    float x,x1, x2, y1, y2;
    out.resize(in_size);
    uint j,j_1;
    j=static_cast<uint>(offset);
    j_1=j+1;
    float diff=offset-j;
    for (uint i = 0; i < in_size; i++,j++) {
        while(j >= in_size)j-=in_size;
        while(j_1 >= in_size)j_1-=in_size;

        x1=j;
        y1=in[j];

        x2=j_1;
        y2=in[j_1];

        x=diff+x1;

        out[i] = line_float(x, x1, y1, x2, y2);
    }
}
void PFC::protocol_GET_OSCILOG(package_general* package)
{
    auto answer = static_cast<answer_get_oscillog*>(package);
     float astep = (answer->max - answer->min) / 255.0f;
     uint16_t len    = answer->len&0xFFFF;
     uint16_t phase  = (answer->len>>16)&0xFFFF;
     std::vector<double> yval(128);
     Q_UNUSED(phase)
     //qDebug()<<"Osc get";
     if(len>128)
     {
         qDebug()<<"Wrong len";
         return;
     }

     for(uint i=0;i<len;i++)
     {
         yval[i] = static_cast<double>(answer->min + astep * answer->data[i]);
     }

     setOscillog(static_cast<PFCOscillogCnannel>(answer->ch),yval);
}

 void PFC::protocol_GET_NET_PARAMS(package_general* package)
 {
     auto answer = static_cast<answer_get_net_params*>(package);
      emit setNetParams(answer->period_fact,
                        answer->U0Hz_A,
                        answer->U0Hz_B,
                        answer->U0Hz_C,
                        answer->I0Hz_A,
                        answer->I0Hz_B,
                        answer->I0Hz_C,
                        answer->thdu_A,
                        answer->thdu_B,
                        answer->thdu_C,
                        answer->U_phase_A,
                        answer->U_phase_B,
                        answer->U_phase_C);
}

void PFC::protocol_GET_SETTINGS_CALIBRATIONS(package_general* package)
{
    auto answer = static_cast<answer_get_settings_calibrations*>(package);
    std::vector<float> calibration(ADC_CHANNEL_NUMBER);
    std::vector<float> offset(ADC_CHANNEL_NUMBER);
    for(uint i=0;i<calibration.size();i++)   calibration[i]=answer->calibration[i];
    for(uint i=0;i<offset.size();i++)        offset[i]=answer->offset[i];
    setSettingsCalibrations(
            calibration,
            offset
            );
}

void PFC::protocol_GET_SETTINGS_PROTECTION(package_general* package)
{
    auto answer = static_cast<answer_get_settings_protection*>(package);
    setSettingsProtection(
        answer->Ucap_min,
        answer->Ucap_max,
        answer->temperature,
        answer->U_min,
        answer->U_max,
        answer->F_min,
        answer->F_max,
        answer->I_max_rms,
        answer->I_max_peak);
}

void PFC::protocol_GET_SETTINGS_CAPACITORS(package_general* package)
{
    auto answer = static_cast<answer_get_settings_capacitors*>(package);
    setSettingsCapacitors(
        answer->ctrl_Ucap_Kp,
        answer->ctrl_Ucap_Ki,
        answer->ctrl_Ucap_Kd,
        answer->Ucap_nominal,
        answer->Ucap_precharge);
}

void PFC::protocol_GET_SETTINGS_FILTERS(package_general* package)
{
    auto answer = static_cast<answer_get_settings_filters*>(package);
    setSettingsFilters(
        answer->K_I,
        answer->K_U,
        answer->K_Ucap);
}

void PFC::protocol_SET_SETTINGS_CALIBRATIONS(package_general* package)
{
    auto answer = static_cast<answer_set_settings_calibrations*>(package);
    Q_UNUSED(answer)
    ansSettingsCalibrations(true);
}

void PFC::protocol_SET_SETTINGS_PROTECTION(package_general* package)
{
    auto answer = static_cast<answer_set_settings_protection*>(package);
    Q_UNUSED(answer)
    ansSettingsProtection(true);
}

void PFC::protocol_SET_SETTINGS_FILTERS(package_general* package)
{
    auto answer = static_cast<answer_set_settings_filters*>(package);
    Q_UNUSED(answer)
    ansSettingsFilters(true);
}

void PFC::protocol_SET_SETTINGS_CAPACITORS(package_general* package)
{
    auto answer = static_cast<answer_set_settings_capacitors*>(package);
    Q_UNUSED(answer)
    ansSettingsCapacitors(true);
}
