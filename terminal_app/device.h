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

class PFC : public QObject{
    Q_OBJECT

public:
    ADFSerialInterface *_interface;
    //typedef void ( *prot_handler )          ( void * );


    typedef std::function<void(std::vector<uint8_t>)> fp;
    std::vector<fp> handlers;
    QThread* thread;

    PFC();

public slots:

    void protocol_SWITCH_ON_OFF(std::vector<uint8_t> data);
    void protocol_GET_ADC_ACTIVE(std::vector<uint8_t> data);
    void protocol_GET_ADC_ACTIVE_RAW(std::vector<uint8_t> data);
    void protocol_GET_WORK_STATE(std::vector<uint8_t> data);
    void protocol_GET_EVENTS(std::vector<uint8_t> data);
    void protocol_GET_VERSION_INFO(std::vector<uint8_t> data);
    void protocol_GET_OSCILOG(std::vector<uint8_t> data);
    void protocol_GET_NET_PARAMS(std::vector<uint8_t> data);
    void protocol_GET_SETTINGS_CALIBRATIONS(std::vector<uint8_t> data);
    void protocol_GET_SETTINGS_PROTECTION(std::vector<uint8_t> data);
    void protocol_GET_SETTINGS_CAPACITORS(std::vector<uint8_t> data);
    void protocol_GET_SETTINGS_FILTERS(std::vector<uint8_t> data);
    void protocol_SET_SETTINGS_CALIBRATIONS(std::vector<uint8_t> data);
    void protocol_SET_SETTINGS_PROTECTION(std::vector<uint8_t> data);
    void protocol_SET_SETTINGS_FILTERS(std::vector<uint8_t> data);
    void protocol_SET_SETTINGS_CAPACITORS(std::vector<uint8_t> data);

signals:
    /*--------------------------------------------------------------
             PUBLIC CLASS SIGNALS
    --------------------------------------------------------------*/
    void Message(quint8 type, quint8 level, quint8 target, QString message);

    void setOscillog(uint16_t channel, std::vector<double> data);
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
    void setEvents(QVector<struct sEventRecord> ev);
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
            QVector<float> calibration,
            QVector<float> offset
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
            QVector<float> fft_suppress);   
    void ansSettingsCalibrations(bool writed);
    void ansSettingsProtection(bool writed);
    void ansSettingsFilters(bool writed);
    void ansSettingsCapacitors(bool writed);

public slots:
    void ConnectionChanged(bool connected){
        setConnection(connected);
    }
#define START_REQUEST(COMM)\
    struct sCOMMAND_##COMM req;

#define END_REQUEST(COMM) \
    std::vector<uint8_t> datareq;\
    if(sizeof(req)){\
        datareq.assign((uint8_t*)&req,(uint8_t*)&req+sizeof(req));\
    }\
    PackageCommand *p = new PackageCommand();\
    p->package_out->fill(\
                ADFMessagePriority::NORMAL,\
                Sender::GUI,\
                P_AFG_COMMAND_##COMM,\
                datareq\
             );\
    _interface->enqueueCommand(p);\
    connect(p, &PackageCommand::complete, this, &getAnswer/*,Qt::DirectConnection*/);

#define END_REQUESTP(COMM,PRIOR) \
    std::vector<uint8_t> datareq;\
    if(sizeof(req)){\
        datareq.assign((uint8_t*)&req,(uint8_t*)&req+sizeof(req));\
    }\
    PackageCommand *p = new PackageCommand();\
    p->package_out->fill(\
                PRIOR,\
                Sender::GUI,\
                P_AFG_COMMAND_##COMM,\
                datareq\
             );\
    _interface->enqueueCommand(p);\
    connect(p, &PackageCommand::complete, this, &getAnswer/*,Qt::DirectConnection*/);

    /*--------------------------------------------------------------
             PUBLIC CLASS FUNCTIONS::REQUESTS
    --------------------------------------------------------------*/

    void updateNetVoltage(void);
    void updateWorkState(uint64_t currentTime);
    void updateEvents(uint64_t afterIndex);
    void updateVersionInfo(void);
    void updateNetVoltageRAW(void);
    void updateOscillog(uint16_t channel);
    void updateNetParams(void);
    void updateSettingsCalibrations(void);
    void updateSettingsProtection(void);
    void updateSettingsCapacitors(void);
    void updateSettingsFilters(void);

    /*--------------------------------------------------------------
             PUBLIC CLASS FUNCTIONS::PARAMETERS WRITE
    --------------------------------------------------------------*/

    void writeSettingsCalibrations(
            QVector<float> calibration, //!< Калибровки для каналов
            QVector<float> offset //!< Смещения для каналов
            );
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
    void writeSwitchOnOff(uint32_t command,uint32_t data);

private:
    /*--------------------------------------------------------------
             PRIVATE CLASS FUNCTIONS
    --------------------------------------------------------------*/

    void getAnswer(bool is_timeout, PackageCommand *pc);

    inline float line_float(float x, float x1, float y1, float x2, float y2);
    void interpolate_shift_resize_float(QVector<double> &in, unsigned int in_size,
            float offset, QVector<double> &out);
    void protocol_unknow_command_handle(std::vector<uint8_t> data);
};

#endif // ADF_H
