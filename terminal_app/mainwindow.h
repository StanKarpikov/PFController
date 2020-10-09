/**
 * @file mainwindow.h
 * @author Stanislav Karpikov
 * @brief Main application window management
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

/** @addtogroup app_main
 * @{
 */

/*--------------------------------------------------------------
                       INCLUDES
--------------------------------------------------------------*/

#include "types.h"
#include <QtCore/QtGlobal>
#include <QDebug>
#include <QMainWindow>
//#include <windows.h>
#include <QtSerialPort/QSerialPort>
#include <QModbusRtuSerialMaster>
#include <QTimer>
#include <device.h>
#include <QThread>
#include <QCustomPlot.h>
#include <QSignalSpy>

/*--------------------------------------------------------------
                       DEFINES
--------------------------------------------------------------*/

#define NOMINAL_VOLTAGE 220

/*--------------------------------------------------------------
                       PUBLIC TYPES
--------------------------------------------------------------*/

QT_BEGIN_NAMESPACE

namespace Ui {
    class MainWindow;
}

QT_END_NAMESPACE

const QBrush editableCellBrush(QColor(240, 255, 240));


struct PFCsettings{
    struct {
        float ADC_UD=0;
        float ADC_U_A=0;
        float ADC_U_B=0;
        float ADC_U_C=0;
        float ADC_I_A=0;
        float ADC_I_B=0;
        float ADC_I_C=0;
        float ADC_I_ET=0;
        float ADC_I_TEMP1=0;
        float ADC_I_TEMP2=0;
        float ADC_EMS_A=0;
        float ADC_EMS_B=0;
        float ADC_EMS_C=0;
        float ADC_EMS_I=0;
        float ADC_MATH_A=0;
        float ADC_MATH_B=0;
        float ADC_MATH_C=0;
    }ADC;
    struct {
        float ADC_UD=0;
        float ADC_U_A=0;
        float ADC_U_B=0;
        float ADC_U_C=0;
        float ADC_I_A=0;
        float ADC_I_B=0;
        float ADC_I_C=0;
        float ADC_I_ET=0;
        float ADC_I_TEMP1=0;
        float ADC_I_TEMP2=0;
        float ADC_EMS_A=0;
        float ADC_EMS_B=0;
        float ADC_EMS_C=0;
        float ADC_EMS_I=0;
    }ADC_RAW;
    struct {
        float period_fact;
        float U0Hz_A;//Постоянная составляющая
        float U0Hz_B;
        float U0Hz_C;
        float I0Hz_A;//Постоянная составляющая
        float I0Hz_B;
        float I0Hz_C;
        float thdu_A;
        float thdu_B;
        float thdu_C;
        float U_phase_A;
        float U_phase_B;
        float U_phase_C;
    }NET_PARAMS;
    struct {
        struct {
            //=== калибровки АЦП
            QVector<float> calibration; //!< Калибровки для каналов
            QVector<float> offset; //!< Смещения для каналов
        }CALIBRATIONS;
        struct {
            //=== защиты ======
            float Ud_min; //!< Граничные значения для Ud
            float Ud_max; //!< Граничные значения для Ud
            float temperature; //!< Граничные значения для Температуры
            float U_min; //!< Граничные значения для напряжения
            float U_max;
            float Fnet_min; //!< минимальная частота сети
            float Fnet_max; //!< максимальная частота сети
            float I_max_rms; //!< Максимальное граничное значение тока фильтра по RMS
            float I_max_peak; //!< Максимальное граничное мгновенное значение тока фильтра
        }PROTECTION;
        struct {
            //==== накачка =====
            float ctrlUd_Kp; //!< Управление накачкой
            float ctrlUd_Ki;
            float ctrlUd_Kd;
            float Ud_nominal;
            float Ud_precharge;
        }CAPACITORS;
        struct{
            float K_I;
            float K_U;
            float K_Ud;
        }FILTERS;
    }SETTINGS;
    uint32_t status; //!< Текущее состояние работы фильтра
    uint32_t activeChannels[3];
};

class SettingsDialog;
class PFC;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:

    /* Constants */
    const std::string DARK_GREY = "#808080";
    const std::string DARK_RED = "#800000";
    const std::string LIGHT_GREY = "#c0c0c0";
    const std::string DARK_GREEN = "#008000";

    const static auto EVENTS_TIMER_TIMEOUT = 1000;

    enum class OscillogChannels
    {
        OSCILLOG_I_A,
        OSCILLOG_I_B,
        OSCILLOG_I_C,

        OSCILLOG_U_A,
        OSCILLOG_U_B,
        OSCILLOG_U_C,

        OSCILLOG_UD,

        OSCILLOG_COMP_A,
        OSCILLOG_COMP_B,
        OSCILLOG_COMP_C,

        OSCILLOG_SIZE
    };

    /** @brief State of the PFC */
    enum class PFCstate
    {
        PFC_STATE_INIT,              /**< Initial state */
        PFC_STATE_STOP,              /**< Stop state (power hardware is switched off) */
        PFC_STATE_SYNC,              /**< Syncronisation with the network */
        PFC_STATE_PRECHARGE_PREPARE, /**< Prepare precharge */
        PFC_STATE_PRECHARGE,         /**< Precharge (connector is in on state) */
        PFC_STATE_MAIN,              /**< Main state. Precharge is finished */
        PFC_STATE_PRECHARGE_DISABLE, /**< Precharge is switching off */
        PFC_STATE_WORK,              /**< Ready state */
        PFC_STATE_CHARGE,            /**< Main capacitors charge is ongoing */
        PFC_STATE_TEST,              /**< Test state */
        PFC_STATE_STOPPING,          /**< Stoping work: disable sensitive and power peripheral */
        PFC_STATE_FAULTBLOCK,        /**< Fault state */
        PFC_STATE_COUNT              /**< The count of the states */
    };

    /* Functions */
    explicit MainWindow(QWidget *parent = Q_NULLPTR);
    ~MainWindow();

    bool eventFilter(QObject *object, QEvent *event);
    void pageMainInit(void);
    void pageOscillogInit(void);
    void pageSettingsCalibrationsInit(void);
    void pageSettingsCapacitorsInit(void);
    void pageSettingsProtectionInit(void);
    void pageSettingsFiltersInit(void);

    /* Data */
    Ui::MainWindow *ui;
    PFC *pfc;
    std::vector<double> oscillog_xval,harmonics_xval;
    std::vector<double> oscillog_data;
    std::vector<QPushButton*> buttons_edit;
    uint64_t last_index_events;
    PFCsettings pfc_settings;

public slots:

    void openSerialPort();
    void closeSerialPort();
    void about();
    void Message(quint8 type, quint8 level, quint8 target, QString message);
    void deviceConnected();
    void deviceDisconnected();

    /* Interface commands */
    void setOscillog(uint16_t channel, std::vector<double> data);
    void setNetVoltage( float ADC_UD,
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
    void setNetVoltageRAW( float ADC_UD,
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
    void setSwitchOnOff(uint32_t result);
    void setEvents(QVector<struct sEventRecord> ev);
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
                    float U0Hz_A,//Постоянная составляющая
                    float U0Hz_B,
                    float U0Hz_C,
                    float I0Hz_A,//Постоянная составляющая
                    float I0Hz_B,
                    float I0Hz_C,
                    float thdu_A,
                    float thdu_B,
                    float thdu_C,
                    float U_phase_A,
                    float U_phase_B,
                    float U_phase_C);
    void setSettingsCalibrations(
            QVector<float> calibration, //!< Калибровки для каналов
            QVector<float> offset //!< Смещения для каналов
            );
    void setSettingsFilters(
            float K_I,
            float K_U,
            float K_Ud);
    void setSettingsProtection(
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
    void setSettingsCapacitors(
            float ctrlUd_Kp, //!< Управление накачкой
            float ctrlUd_Ki,
            float ctrlUd_Kd,
            float Ud_nominal,
            float Ud_precharge);
    void ansSettingsCalibrations(bool writed);
    void ansSettingsProtection(bool writed);
    void ansSettingsCapacitors(bool writed);
    void ansSettingsFilters(bool writed);

    /* Timer events callbacks */
    void timerOscillog();
    void timerNetParams();
    void timerWorkState();
    void timerVersion();
    void timerSettingsCalibrations();
    void timerSettingsCapacitors();
    void timerSettingsFilters();
    void timerSettingsProtection();    
    void timerupdateNetVoltage();
    void timerupdateNetVoltageRaw();
    void timerEvents();

    /* Other functions */
    void xAxisRangeChanged( QCPRange newRange ,QCPRange oldRange);
    void mouseWheel(QWheelEvent *event);

    void tableSettingsCalibrations_changed(int row, int col);
    void tableSettingsProtection_changed(int row, int col);
    void tableSettingsFilters_changed(int row, int col);

signals:
    void updateNetVoltage();
    void updateWorkState(uint64_t currentTime);
    void updateVersionInfo();
    void updateNetVoltageRAW();
    void updateNetParams();
    void updateOscillog(uint16_t channel);
    void updateEvents(uint64_t afterIndex);
    void updateSettingsCalibrations();
    void updateSettingsProtection();
    void updateSettingsCapacitors();
    void updateSettingsFilters();    

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
            float ctrlUd_Kp, //!< Управление накачкой
            float ctrlUd_Ki,
            float ctrlUd_Kd,
            float Ud_nominal,
            float Ud_precharge);
    void writeSettingsFilters(
            float K_I,
            float K_U,
            float K_Ud);
    void writeSwitchOnOff(uint32_t command,uint32_t data);

private slots:
    void onPushButtonClicked();
    void on_doubleSpinBox_capacitors_Kp_valueChanged(double arg1);
    void on_doubleSpinBox_capacitors_Ki_valueChanged(double arg1);
    void on_doubleSpinBox_capacitors_Kd_valueChanged(double arg1);
    void on_doubleSpinBox_cap_nominal_valueChanged(double arg1);
    void on_doubleSpinBox_cap_precharge_valueChanged(double arg1);
    void on_pushButton_STOP_clicked();
    void on_pushButton_Start_clicked();    
    void on_pushButton_Save_clicked();   
    void on_actionClear_triggered();
    void on_pushButton_2_clicked();
    void on_checkBox_channelA_toggled(bool checked);
    void on_checkBox_channelB_toggled(bool checked);
    void on_checkBox_channelC_toggled(bool checked);
    void on_pushButton_CHARGE_ON_clicked();
    void on_pushButton_CHARGE_OFF_clicked();

private:
    void initInterfaceConnections();

private:
    SettingsDialog *port_settings;
    QTimer timer_MainParams;
    QTimer timer_Raw;
    QTimer timer_State;
    QTimer timer_Vol;
    QTimer timer_Version;
    QTimer timer_Oscillog;    
    QTimer timer_events;
    QTimer timer_SettingsCalibrations;
    QTimer timer_SettingsCapacitors;
    QTimer timer_SettingsProtection;
    QTimer timer_SettingsFilters;
    bool connected=false;
    QSharedPointer<QCPAxisTickerFixed> fixed_ticker;
};

/** @} */

#endif /* MAINWINDOW_H */
