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

PFC::PFC()
    : _interface(new PFCSerialInterface(Q_NULLPTR)),
      _handlers(enum_int(pfc_interface_commands_t::PFC_COMMAND_COUNT)),
      _thread(new QThread())
{
    /* Translate interface link signals to the upper level */
    connect(_interface, &PFCSerialInterface::connected, this, &PFC::interfaceConnected);
    connect(_interface, &PFCSerialInterface::disconnected, this, &PFC::interfaceDisconnected);

    /* Run the interface in a separate thread */
    _interface->moveToThread(_thread);
    connect(_thread, SIGNAL(finished()), _thread, SLOT(deleteLater()));
    connect(_thread, SIGNAL(started()), _interface, SLOT(run()));
    connect(_interface, SIGNAL(informConnectionChanged(bool)), this, SLOT(connectionChanged(bool)));
    _thread->start();

    /* Initialize protocol handlers */
    for (uint i = 0; i < enum_int(pfc_interface_commands_t::PFC_COMMAND_COUNT); i++)
        _handlers[i] = std::bind(&PFC::protocolUnknownCommand, this, std::placeholders::_1);

    _handlers[enum_int(pfc_interface_commands_t::PFC_COMMAND_GET_ADC_ACTIVE)] =
        std::bind(&PFC::protocolGetADCActive, this, std::placeholders::_1);
    _handlers[enum_int(pfc_interface_commands_t::PFC_COMMAND_SWITCH_ON_OFF)] =
        std::bind(&PFC::protocolSwitchOnOff, this, std::placeholders::_1);
    _handlers[enum_int(pfc_interface_commands_t::PFC_COMMAND_GET_ADC_ACTIVE_RAW)] =
        std::bind(&PFC::protocolGetADCActiveRAW, this, std::placeholders::_1);
    _handlers[enum_int(pfc_interface_commands_t::PFC_COMMAND_GET_OSCILLOG)] =
        std::bind(&PFC::protocolGetOscillog, this, std::placeholders::_1);
    _handlers[enum_int(pfc_interface_commands_t::PFC_COMMAND_GET_NET_PARAMS)] =
        std::bind(&PFC::protocolGetNetParams, this, std::placeholders::_1);
    _handlers[enum_int(pfc_interface_commands_t::PFC_COMMAND_GET_WORK_STATE)] =
        std::bind(&PFC::protocolGetWorkState, this, std::placeholders::_1);
    _handlers[enum_int(pfc_interface_commands_t::PFC_COMMAND_GET_VERSION_INFO)] =
        std::bind(&PFC::protocolGetVersionInfo, this, std::placeholders::_1);
    _handlers[enum_int(pfc_interface_commands_t::PFC_COMMAND_GET_EVENTS)] =
        std::bind(&PFC::protocolGetEvents, this, std::placeholders::_1);

    _handlers[enum_int(pfc_interface_commands_t::PFC_COMMAND_GET_SETTINGS_CALIBRATIONS)] =
        std::bind(&PFC::protocolGetSettingsCalibrations, this, std::placeholders::_1);
    _handlers[enum_int(pfc_interface_commands_t::PFC_COMMAND_GET_SETTINGS_PROTECTION)] =
        std::bind(&PFC::protocolGetSettingsProtection, this, std::placeholders::_1);
    _handlers[enum_int(pfc_interface_commands_t::PFC_COMMAND_GET_SETTINGS_CAPACITORS)] =
        std::bind(&PFC::protocolGetSettingsCapacitors, this, std::placeholders::_1);
    _handlers[enum_int(pfc_interface_commands_t::PFC_COMMAND_GET_SETTINGS_FILTERS)] =
        std::bind(&PFC::protocolGetSettingsFilters, this, std::placeholders::_1);

    _handlers[enum_int(pfc_interface_commands_t::PFC_COMMAND_SET_SETTINGS_CALIBRATIONS)] =
        std::bind(&PFC::protocolSetSettingsCalibrations, this, std::placeholders::_1);
    _handlers[enum_int(pfc_interface_commands_t::PFC_COMMAND_SET_SETTINGS_PROTECTION)] =
        std::bind(&PFC::protocolSetSettingsProtection, this, std::placeholders::_1);
    _handlers[enum_int(pfc_interface_commands_t::PFC_COMMAND_SET_SETTINGS_CAPACITORS)] =
        std::bind(&PFC::protocolSetSettingsCapacitors, this, std::placeholders::_1);
    _handlers[enum_int(pfc_interface_commands_t::PFC_COMMAND_SET_SETTINGS_FILTERS)] =
        std::bind(&PFC::protocolSetSettingsFilters, this, std::placeholders::_1);

    connect(_interface, &PFCSerialInterface::message, this, &PFC::message);

    message(MESSAGE_TYPE_GENERAL, MESSAGE_NORMAL, MESSAGE_TARGET_DEBUG, "Поток АДФ запущен");
}

PFC::~PFC(void)
{
    /* TODO: Clear resources */
}

void PFC::protocolUnknownCommand(package_general* answer)
{
    Q_UNUSED(answer)
    message(MESSAGE_TYPE_GENERAL, MESSAGE_NORMAL, MESSAGE_TARGET_DEBUG,
            "Неизвестная команда");
}

/*--------------------------------------------------------------
         PUBLIC CLASS FUNCTIONS::REQUESTS
--------------------------------------------------------------*/

void PFC::endRequest(package_general& req, pfc_interface_commands_t command, uint32_t packet_size, DeviceSerialMessage::MessagePriority priority)
{
    std::vector<uint8_t> datareq;
    /* TODO: Verify sizeof */
    if (packet_size)
    {
        datareq.assign(reinterpret_cast<uint8_t*>(&req), reinterpret_cast<uint8_t*>(&req) + packet_size);
    }
    InterfacePackage* p = new InterfacePackage();
    p->package_to_send->fill(
        priority,
        DeviceSerialMessage::Sender::GUI,
        static_cast<uint8_t>(enum_int(command)),
        datareq);
    _interface->enqueueCommand(p);
    connect(p, &InterfacePackage::complete, this, &PFC::getAnswer /*,Qt::DirectConnection*/);
}

void PFC::updateNetVoltage()
{
    command_get_adc_active req;
    endRequest(req, pfc_interface_commands_t::PFC_COMMAND_GET_ADC_ACTIVE, sizeof(req));
}
void PFC::updateWorkState(uint64_t currentTime)
{
    command_get_work_state req;
    req.currentTime = currentTime;
    endRequest(req, pfc_interface_commands_t::PFC_COMMAND_GET_WORK_STATE, sizeof(req));
}
void PFC::updateEvents(uint64_t afterIndex)
{
    command_get_events req;
    req.after_index = afterIndex;
    endRequest(req, pfc_interface_commands_t::PFC_COMMAND_GET_EVENTS, sizeof(req));
}
void PFC::updateVersionInfo()
{
    command_get_version_info req;
    endRequest(req, pfc_interface_commands_t::PFC_COMMAND_GET_VERSION_INFO, sizeof(req));
}
void PFC::updateNetVoltageRAW()
{
    command_get_adc_active_raw req;
    endRequest(req, pfc_interface_commands_t::PFC_COMMAND_GET_ADC_ACTIVE_RAW, sizeof(req));
}
void PFC::updateOscillog(PFCOscillogCnannel channel)
{
    command_get_oscillog req;
    req.num = static_cast<decltype(req.num)>(enum_int(channel));
    endRequest(req, pfc_interface_commands_t::PFC_COMMAND_GET_OSCILLOG, sizeof(req));
}
void PFC::updateNetParams()
{
    command_get_net_params req;
    endRequest(req, pfc_interface_commands_t::PFC_COMMAND_GET_NET_PARAMS, sizeof(req));
}
void PFC::updateSettingsCalibrations()
{
    command_get_settings_calibrations req;
    endRequest(req, pfc_interface_commands_t::PFC_COMMAND_GET_SETTINGS_CALIBRATIONS, sizeof(req));
}
void PFC::updateSettingsProtection()
{
    command_get_settings_protection req;
    endRequest(req, pfc_interface_commands_t::PFC_COMMAND_GET_SETTINGS_PROTECTION, sizeof(req));
}
void PFC::updateSettingsCapacitors()
{
    command_get_settings_capacitors req;
    endRequest(req, pfc_interface_commands_t::PFC_COMMAND_GET_SETTINGS_CAPACITORS, sizeof(req));
}
void PFC::updateSettingsFilters()
{
    command_get_settings_filters req;
    endRequest(req, pfc_interface_commands_t::PFC_COMMAND_GET_SETTINGS_FILTERS, sizeof(req));
}

/*--------------------------------------------------------------
         PUBLIC CLASS FUNCTIONS::PARAMETERS WRITE
--------------------------------------------------------------*/

void PFC::writeSettingsCalibrations(
    std::vector<float> calibration,
    std::vector<float> offset)
{
    command_set_settings_calibrations req;
    for (uint i = 0; i < calibration.size(); i++) req.calibration[i] = calibration[i];
    for (uint i = 0; i < offset.size(); i++) req.offset[i] = offset[i];
    endRequest(req, pfc_interface_commands_t::PFC_COMMAND_SET_SETTINGS_CALIBRATIONS, sizeof(req));
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
    float I_max_peak)
{
    command_set_settings_protection req;
    req.Ucap_min = Ud_min;
    req.Ucap_max = Ud_max;
    req.temperature = temperature;
    req.U_min = U_min;
    req.U_max = U_max;
    req.F_min = Fnet_min;
    req.F_max = Fnet_max;
    req.I_max_rms = I_max_rms;
    req.I_max_peak = I_max_peak;
    endRequest(req, pfc_interface_commands_t::PFC_COMMAND_SET_SETTINGS_PROTECTION, sizeof(req));
}

void PFC::writeSettingsCapacitors(
    float ctrlUd_Kp,
    float ctrlUd_Ki,
    float ctrlUd_Kd,
    float Ud_nominal,
    float Ud_precharge)
{
    command_set_settings_capacitors req;
    req.ctrl_Ucap_Kp = ctrlUd_Kp;
    req.ctrl_Ucap_Ki = ctrlUd_Ki;
    req.ctrl_Ucap_Kd = ctrlUd_Kd;
    req.Ucap_nominal = Ud_nominal;
    req.Ucap_precharge = Ud_precharge;
    endRequest(req, pfc_interface_commands_t::PFC_COMMAND_SET_SETTINGS_CAPACITORS, sizeof(req));
}

void PFC::writeSettingsFilters(
    float K_I,
    float K_U,
    float K_UD)
{
    command_set_settings_filters req;
    req.K_I = K_I;
    req.K_U = K_U;
    req.K_Ucap = K_UD;
    endRequest(req, pfc_interface_commands_t::PFC_COMMAND_SET_SETTINGS_FILTERS, sizeof(req));
}

void PFC::writeSwitchOnOff(pfc_commands_t command, uint32_t data)
{
    command_switch_on_off req;
    req.command = static_cast<decltype(req.command)>(command);
    req.data = data;
    endRequest(req, pfc_interface_commands_t::PFC_COMMAND_SWITCH_ON_OFF, sizeof(req), DeviceSerialMessage::MessagePriority::HIGH);
}

/*--------------------------------------------------------------
         PRIVATE CLASS FUNCTIONS::ANSWER
--------------------------------------------------------------*/

void PFC::getAnswer(bool is_timeout, InterfacePackage* pc)
{
    //qDebug() << Q_FUNC_INFO;
    if (is_timeout) return;

    auto command = static_cast<pfc_interface_commands_t>(pc->package_read->command());

    if (command >= pfc_interface_commands_t::PFC_COMMAND_COUNT)
    {
        message(MESSAGE_TYPE_CONNECTION, MESSAGE_NORMAL, MESSAGE_TARGET_DEBUG, "Неизвестный ответ");
        return;
    }
    //qDebug() << pc->package_in->command();
    if (pc->package_read->error())
    {
        message(MESSAGE_TYPE_GLOBALWARNING, MESSAGE_NORMAL, MESSAGE_TARGET_DEBUG, "Ошибка выполнения команды");
        return;
    }
    auto package_data = pc->package_read->data();
    uint8_t* command_data = &package_data[0];

    package_general* package = reinterpret_cast<package_general*>(command_data);

    //Add try catch
    _handlers[static_cast<uint>(command)](package);
    //QTimer::singleShot(100, [this,apc]() { handlers[pc->package_in->command()](&apc); } );
}

/*--------------------------------------------------------------
         PRIVATE CLASS FUNCTIONS::HANDLERS
--------------------------------------------------------------*/

void PFC::protocolSwitchOnOff(package_general* package)
{
    auto answer = static_cast<answer_switch_on_off*>(package);
    emit setSwitchOnOff(answer->result);
}

void PFC::protocolGetADCActive(package_general* package)
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
                       answer->ADC_MATH_C);
}

void PFC::protocolGetADCActiveRAW(package_general* package)
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
                          answer->ADC_EDC_I);
}

void PFC::protocolGetWorkState(package_general* package)
{
    auto answer = static_cast<answer_get_work_state*>(package);
    emit setWorkState(answer->state,
                      answer->active_channels[PFC_ACHAN],
                      answer->active_channels[PFC_BCHAN],
                      answer->active_channels[PFC_CCHAN]);
}

void PFC::protocolGetEvents(package_general* package)
{
    auto answer = static_cast<answer_get_events*>(package);
    if (answer->num == 0) return;
    std::list<EventRecord> ev;
    if(answer->num > MAX_NUM_TRANSFERED_EVENTS)
    {
        return;
    }
    for (int i = 0; i < answer->num; i++)
    {
        ev.push_back(answer->events[i]);
    }
    emit setEvents(ev);
}

void PFC::protocolGetVersionInfo(package_general* package)
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

inline float PFC::lineFloat(float x, float x1, float y1, float x2, float y2)
{
    float a;
    a = (y2 - y1) / (x2 - x1);
    return y1 + a * (x - x1);
}

void PFC::interpolateShiftResizeFloat(std::vector<float>& in,
                                         uint32_t in_size,
                                         float offset,
                                         std::vector<float>& out)
{
    float x, x1, x2, y1, y2;
    out.resize(in_size);
    uint j, j_1;
    j = static_cast<uint>(offset);
    j_1 = j + 1;
    float diff = offset - j;
    for (uint i = 0; i < in_size; i++, j++)
    {
        while (j >= in_size) j -= in_size;
        while (j_1 >= in_size) j_1 -= in_size;

        x1 = j;
        y1 = in[j];

        x2 = j_1;
        y2 = in[j_1];

        x = diff + x1;

        out[i] = lineFloat(x, x1, y1, x2, y2);
    }
}
void PFC::protocolGetOscillog(package_general* package)
{
    auto answer = static_cast<answer_get_oscillog*>(package);
    float astep = (answer->max - answer->min) / 255.0f;
    uint16_t len = answer->len & 0xFFFF;
    uint16_t phase = (answer->len >> 16) & 0xFFFF;
    std::vector<double> yval(128);
    Q_UNUSED(phase)
    //qDebug()<<"Osc get";
    if (len > 128)
    {
        qDebug() << "Wrong len";
        return;
    }

    for (uint i = 0; i < len; i++)
    {
        yval[i] = static_cast<double>(answer->min + astep * answer->data[i]);
    }

    setOscillog(static_cast<PFCOscillogCnannel>(answer->ch), yval);
}

void PFC::protocolGetNetParams(package_general* package)
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

void PFC::protocolGetSettingsCalibrations(package_general* package)
{
    auto answer = static_cast<answer_get_settings_calibrations*>(package);
    std::vector<float> calibration(ADC_CHANNEL_NUMBER);
    std::vector<float> offset(ADC_CHANNEL_NUMBER);
    for (uint i = 0; i < calibration.size(); i++) calibration[i] = answer->calibration[i];
    for (uint i = 0; i < offset.size(); i++) offset[i] = answer->offset[i];
    setSettingsCalibrations(
        calibration,
        offset);
}

void PFC::protocolGetSettingsProtection(package_general* package)
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

void PFC::protocolGetSettingsCapacitors(package_general* package)
{
    auto answer = static_cast<answer_get_settings_capacitors*>(package);
    setSettingsCapacitors(
        answer->ctrl_Ucap_Kp,
        answer->ctrl_Ucap_Ki,
        answer->ctrl_Ucap_Kd,
        answer->Ucap_nominal,
        answer->Ucap_precharge);
}

void PFC::protocolGetSettingsFilters(package_general* package)
{
    auto answer = static_cast<answer_get_settings_filters*>(package);
    setSettingsFilters(
        answer->K_I,
        answer->K_U,
        answer->K_Ucap);
}

void PFC::protocolSetSettingsCalibrations(package_general* package)
{
    auto answer = static_cast<answer_set_settings_calibrations*>(package);
    Q_UNUSED(answer)
    ansSettingsCalibrations(true);
}

void PFC::protocolSetSettingsProtection(package_general* package)
{
    auto answer = static_cast<answer_set_settings_protection*>(package);
    Q_UNUSED(answer)
    ansSettingsProtection(true);
}

void PFC::protocolSetSettingsFilters(package_general* package)
{
    auto answer = static_cast<answer_set_settings_filters*>(package);
    Q_UNUSED(answer)
    ansSettingsFilters(true);
}

void PFC::protocolSetSettingsCapacitors(package_general* package)
{
    auto answer = static_cast<answer_set_settings_capacitors*>(package);
    Q_UNUSED(answer)
    ansSettingsCapacitors(true);
}

void PFC::connectionChanged(bool connected)
{
    emit setConnection(connected);
}

void PFC::interfaceConnectTo(
    QString name,
    qint32 baudRate,
    QSerialPort::DataBits dataBits,
    QSerialPort::Parity parity,
    QSerialPort::StopBits stopBits,
    QSerialPort::FlowControl flowControl,
    bool localEchoEnabled,
    quint32 timeout,
    quint32 numberOfRetries)
{
    QMetaObject::invokeMethod(_interface, "ConnectTo",
                              Qt::QueuedConnection,
                              Q_ARG(QString, name),
                              Q_ARG(qint32, baudRate),
                              Q_ARG(QSerialPort::DataBits, dataBits),
                              Q_ARG(QSerialPort::Parity, parity),
                              Q_ARG(QSerialPort::StopBits, stopBits),
                              Q_ARG(QSerialPort::FlowControl, flowControl),
                              Q_ARG(bool, localEchoEnabled),
                              Q_ARG(quint32, timeout),
                              Q_ARG(quint32, numberOfRetries));
}

void PFC::interfaceDisconnect(void)
{
    QMetaObject::invokeMethod(_interface, "Disconnect");
}
