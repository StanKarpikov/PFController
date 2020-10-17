#ifndef __DEVICE_H
#define __DEVICE_H

/*--------------------------------------------------------------
                       INCLUDES
--------------------------------------------------------------*/

#include <QtCore/QtGlobal>
#include "serial/deviceserialinterface.h"
#include "device_definition.h"
#include "device_interface_commands.h"

/*--------------------------------------------------------------
                       CLASSES
--------------------------------------------------------------*/

class PFC : public QObject
{
    Q_OBJECT

    /*--------------------------------------------------------------
                           PRIVATE DATA
    --------------------------------------------------------------*/
private:
    typedef std::function<void(package_general*)> package_handler;

private:
    PFCSerialInterface* _interface;
    std::vector<package_handler> _handlers;
    QThread* _thread;

    /*--------------------------------------------------------------
                           PUBLIC FUNCTIONS
    --------------------------------------------------------------*/
public:
    PFC(void);
    ~PFC(void);

signals:
    void message(uint8_t type, uint8_t level, uint8_t target, std::string message);

    void interfaceConnected(void);
    void interfaceDisconnected(void);

    void setOscillog(PFCconfig::Interface::OscillogCnannel channel, std::vector<double> data);
    void setNetVoltage(	float ADC_UD,
                        float ADC_U_A,
                        float ADC_U_B,
                        float ADC_U_C,
                        float ADC_I_A,
                        float ADC_I_B,
                        float ADC_I_C,
                        float ADC_I_ET,
                        float ADC_I_TEMP1,
                        float ADC_I_TEMP2,
                        float ADC_EMS_A,
                        float ADC_EMS_B,
                        float ADC_EMS_C,
                        float ADC_EMS_I,
                        float ADC_MATH_A,
                        float ADC_MATH_B,
                        float ADC_MATH_C);
    void setNetVoltageRAW(float ADC_UD,
                          float ADC_U_A,
                          float ADC_U_B,
                          float ADC_U_C,
                          float ADC_I_A,
                          float ADC_I_B,
                          float ADC_I_C,
                          float ADC_I_ET,
                          float ADC_I_TEMP1,
                          float ADC_I_TEMP2,
                          float ADC_EMS_A,
                          float ADC_EMS_B,
                          float ADC_EMS_C,
                          float ADC_EMS_I);
    void setWorkState(uint32_t state, uint32_t ch_a, uint32_t ch_b, uint32_t ch_c);
    void setEvents(std::list<PFCconfig::Events::EventRecord> ev);
    void setSwitchOnOff(uint32_t result);
    void setVersionInfo(
            uint32_t major,
            uint32_t minor,
            uint32_t micro,
            uint32_t build,
            uint32_t day,
            uint32_t month,
            uint32_t year,
            uint32_t hour,
            uint32_t minute,
            uint32_t second);
    void setConnection(bool connected);
    void setNetParams(float period_fact,
                    float U0Hz_A,
                    float U0Hz_B,
                    float U0Hz_C,
                    float I0Hz_A,
                    float I0Hz_B,
                    float I0Hz_C,
                    float thdu_A,
                    float thdu_B,
                    float thdu_C,
                    float U_phase_A,
                    float U_phase_B,
                    float U_phase_C);
    void setSettingsCalibrations(
            std::vector<float> calibration,
            std::vector<float> offset
            );
    void setSettingsProtection(
            float Ud_min,
            float Ud_max,
            float temperature,
            float U_min,
            float U_max,
            float Fnet_min,
            float Fnet_max,
            float I_max_rms,
            float I_max_peak
            );
    void setSettingsCapacitors(
            float ctrlUd_Kp,
            float ctrlUd_Ki,
            float ctrlUd_Kd,
            float Ud_nominal,
            float Ud_precharge);
    void setSettingsFilters(
            float K_I,
            float K_U,
            float K_UD);
    void setSettingsHarmonicsSuppression(
            std::vector<float> fft_suppress);
    void ansSettingsCalibrations(bool writed);
    void ansSettingsProtection(bool writed);
    void ansSettingsFilters(bool writed);
    void ansSettingsCapacitors(bool writed);

public slots:
    void connectionChanged(bool connected);

    void interfaceConnectTo(
        QString name,
        qint32 baudRate = PFCconfig::Serial::PORT_BADRATE,
        QSerialPort::DataBits dataBits = PFCconfig::Serial::PORT_DATA_BITS,
        QSerialPort::Parity parity = PFCconfig::Serial::PORT_PARITY,
        QSerialPort::StopBits stopBits = PFCconfig::Serial::PORT_STOP_BITS,
        QSerialPort::FlowControl flowControl = PFCconfig::Serial::PORT_FLOW_CONTROL,
        bool localEchoEnabled = PFCconfig::Serial::PORT_ECHO_ENABLED,
        quint32 timeout = PFCconfig::Serial::PORT_TIMEOUT,
        quint32 numberOfRetries = PFCconfig::Serial::PORT_NUMBER_OF_RETRIES);
    void interfaceDisconnect(void);

    /*--------------------------------------------------------------
             PUBLIC CLASS FUNCTIONS::REQUESTS
    --------------------------------------------------------------*/

    void updateNetVoltage(void);
    void updateWorkState(uint64_t currentTime);
    void updateEvents(uint64_t afterIndex);
    void updateVersionInfo(void);
    void updateNetVoltageRAW(void);
    void updateOscillog(PFCconfig::Interface::OscillogCnannel channel);
    void updateNetParams(void);
    void updateSettingsCalibrations(void);
    void updateSettingsProtection(void);
    void updateSettingsCapacitors(void);
    void updateSettingsFilters(void);

    /*--------------------------------------------------------------
             PUBLIC CLASS FUNCTIONS::PARAMETERS WRITE
    --------------------------------------------------------------*/

    void writeSettingsCalibrations(
            std::vector<float> calibration,
            std::vector<float> offset
            );
    void writeSettingsProtection(
            float Ud_min,
            float Ud_max,
            float temperature,
            float U_min,
            float U_max,
            float Fnet_min,
            float Fnet_max,
            float I_max_rms,
            float I_max_peak
            );
    void writeSettingsCapacitors(
            float ctrlUd_Kp,
            float ctrlUd_Ki,
            float ctrlUd_Kd,
            float Ud_nominal,
            float Ud_precharge);
    void writeSettingsFilters(
            float K_I,
            float K_U,
            float K_UD);
    void writeSwitchOnOff(PFCconfig::Interface::PFCCommands command,uint32_t data);

    /*--------------------------------------------------------------
                            PRIVATE FUNCTIONS
    --------------------------------------------------------------*/
private slots:
    /* Protocol handlers */
    void protocolSwitchOnOff(package_general* package);
    void protocolGetADCActive(package_general* package);
    void protocolGetADCActiveRAW(package_general* package);
    void protocolGetWorkState(package_general* package);
    void protocolGetEvents(package_general* package);
    void protocolGetVersionInfo(package_general* package);
    void protocolGetOscillog(package_general* package);
    void protocolGetNetParams(package_general* package);
    void protocolGetSettingsCalibrations(package_general* package);
    void protocolGetSettingsProtection(package_general* package);
    void protocolGetSettingsCapacitors(package_general* package);
    void protocolGetSettingsFilters(package_general* package);
    void protocolSetSettingsCalibrations(package_general* package);
    void protocolSetSettingsProtection(package_general* package);
    void protocolSetSettingsFilters(package_general* package);
    void protocolSetSettingsCapacitors(package_general* package);

private:
    void getAnswer(bool is_timeout, InterfacePackage *pc);

    inline float lineFloat(float x, float x1, float y1, float x2, float y2);
    void interpolateShiftResizeFloat(
            std::vector<float> &in,
            uint32_t in_size,
            float offset,
            std::vector<float> &out);
    void protocolUnknownCommand(package_general* answer);
    void endRequest(package_general &req, PFCconfig::Interface::InterfaceCommands command, uint32_t packet_size, DeviceSerialMessage::MessagePriority priority = DeviceSerialMessage::MessagePriority::NORMAL);

};

#endif // __DEVICE_H
