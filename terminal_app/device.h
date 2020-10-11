#ifndef ADF_H
#define ADF_H
#include <QModbusRtuSerialMaster>
#include <QWidget>
#include <QTimer>
#include "mainwindow.h"
#include "types.h"
#include <QEventLoop>
#include "serial/DeviceSerialInterface.h"
#include "ui_mainwindow.h"

#include "device_definition.h"
#include "device_interface_commands.h"

#define shifted(x, offset) ((float)(x) + (float)(offset))
#define unshifted(x, offset) ((float)(x) - (float)(offset))

class PFC : public QObject{
    Q_OBJECT

public:

    /*--------------------------------------------------------------
                           PUBLIC TYPES
    --------------------------------------------------------------*/

    typedef std::function<void(package_general*)> fp;

    /*--------------------------------------------------------------
                           PUBLIC DATA
    --------------------------------------------------------------*/

    ADFSerialInterface *_interface;
    std::vector<fp> handlers;
    QThread* thread;

    PFC();

    /*--------------------------------------------------------------
                            PRIVATE FUNCTIONS
    --------------------------------------------------------------*/

private slots:
    void protocol_SWITCH_ON_OFF(package_general* package);
    void protocol_GET_ADC_ACTIVE(package_general* package);
    void protocol_GET_ADC_ACTIVE_RAW(package_general* package);
    void protocol_GET_WORK_STATE(package_general* package);
    void protocol_GET_EVENTS(package_general* package);
    void protocol_GET_VERSION_INFO(package_general* package);
    void protocol_GET_OSCILOG(package_general* package);
    void protocol_GET_NET_PARAMS(package_general* package);
    void protocol_GET_SETTINGS_CALIBRATIONS(package_general* package);
    void protocol_GET_SETTINGS_PROTECTION(package_general* package);
    void protocol_GET_SETTINGS_CAPACITORS(package_general* package);
    void protocol_GET_SETTINGS_FILTERS(package_general* package);
    void protocol_SET_SETTINGS_CALIBRATIONS(package_general* package);
    void protocol_SET_SETTINGS_PROTECTION(package_general* package);
    void protocol_SET_SETTINGS_FILTERS(package_general* package);
    void protocol_SET_SETTINGS_CAPACITORS(package_general* package);

private:
    void getAnswer(bool is_timeout, PackageCommand *pc);

    inline float line_float(float x, float x1, float y1, float x2, float y2);
    void interpolate_shift_resize_float(
            std::vector<float> &in,
            uint32_t in_size,
            float offset,
            std::vector<float> &out);
    void protocol_unknow_command_handle(package_general* answer);
    void EndRequest(package_general &req, PFCconfig::Interface::pfc_interface_commands_t command, ADFMessagePriority priority = ADFMessagePriority::NORMAL);

    /*--------------------------------------------------------------
             PUBLIC FUNCTIONS
    --------------------------------------------------------------*/
signals:
    void Message(quint8 type, quint8 level, quint8 target, QString message);

    void setOscillog(PFCconfig::Interface::PFCOscillogCnannel channel, std::vector<double> data);
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
    void setWorkState(uint32_t state, uint32_t chA, uint32_t chB, uint32_t chC);
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
    void ConnectionChanged(bool connected){
        setConnection(connected);
    }

    /*--------------------------------------------------------------
             PUBLIC CLASS FUNCTIONS::REQUESTS
    --------------------------------------------------------------*/

    void updateNetVoltage(void);
    void updateWorkState(uint64_t currentTime);
    void updateEvents(uint64_t afterIndex);
    void updateVersionInfo(void);
    void updateNetVoltageRAW(void);
    void updateOscillog(PFCconfig::Interface::PFCOscillogCnannel channel);
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
    void writeSwitchOnOff(PFCconfig::Interface::pfc_commands_t command,uint32_t data);
};

#endif // ADF_H
