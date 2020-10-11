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

#include "device_definition.h"

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
        float U0Hz_A;
        float U0Hz_B;
        float U0Hz_C;
        float I0Hz_A;
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
            std::vector<float> calibration;
            std::vector<float> offset;
        }CALIBRATIONS;
        struct {
            float Ud_min;
            float Ud_max;
            float temperature;
            float U_min;
            float U_max;
            float Fnet_min;
            float Fnet_max;
            float I_max_rms;
            float I_max_peak;
        }PROTECTION;
        struct {
            float ctrlUd_Kp;
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
    uint32_t status;
    uint32_t activeChannels[PFCconfig::PFC_NCHAN];
};

class SettingsDialog;
class PFC;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:

    /* Constants */
    inline static const std::string DARK_GREY = "#808080";
    inline static const std::string DARK_RED = "#800000";
    inline static const std::string LIGHT_GREY = "#c0c0c0";
    inline static const std::string DARK_GREEN = "#008000";

    static constexpr auto TIMEOUT_UPDATE_MAIN_PARAMS = static_cast<std::chrono::milliseconds>(300);
    static constexpr auto TIMEOUT_UPDATE_VOLTAGES = static_cast<std::chrono::milliseconds>(300);
    static constexpr auto TIMEOUT_UPDATE_ADC_RAW = static_cast<std::chrono::milliseconds>(300);
    static constexpr auto TIMEOUT_UPDATE_STATE = static_cast<std::chrono::milliseconds>(300);
    static constexpr auto TIMEOUT_UPDATE_VERSION = static_cast<std::chrono::milliseconds>(3000);
    static constexpr auto TIMEOUT_UPDATE_OSCILLOG = static_cast<std::chrono::milliseconds>(54);
    static constexpr auto TIMEOUT_UPDATE_SETTINGS_CALIBRATIONS = static_cast<std::chrono::milliseconds>(300);
    static constexpr auto TIMEOUT_UPDATE_SETTINGS_CAPACITORS = static_cast<std::chrono::milliseconds>(300);
    static constexpr auto TIMEOUT_UPDATE_SETTINGS_PROTECTION = static_cast<std::chrono::milliseconds>(300);
    static constexpr auto TIMEOUT_UPDATE_SETTINGS_FILTERS = static_cast<std::chrono::milliseconds>(300);

    const static auto EVENTS_TIMER_TIMEOUT = 1000;    
    const static auto UD_MAX_VALUE = 500;

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

    enum class table_rows
    {
        table_protection_row_Ud_min,
        table_protection_row_Ud_max,
        table_protection_row_temperature,
        table_protection_row_U_min,
        table_protection_row_U_max,
        table_protection_row_Fnet_min,
        table_protection_row_Fnet_max,
        table_protection_row_I_max_rms,
        table_protection_row_I_max_peak
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
    void SET_TABLE_PROT(table_rows row, float VAL);
    void UPDATE_SPINBOX(QDoubleSpinBox *SPIN,float VAL);
    void UPDATE_CHECKBOX(QCheckBox* CHECK,bool VAL);

    /* Data */
    Ui::MainWindow *ui;
    PFC *pfc;
    std::vector<double> oscillog_xval,harmonics_xval;
    std::vector<std::vector<double>> oscillog_data;
    std::list<QPushButton*> buttons_edit;
    uint64_t last_index_events;
    PFCsettings pfc_settings;
    QMap<PFCconfig::Interface::PFCOscillogCnannel,OscillogChannels> OSCILLOG_ARR;

public slots:

    void openSerialPort();
    void closeSerialPort();
    void about();
    void Message(quint8 type, quint8 level, quint8 target, std::string message);
    void deviceConnected();
    void deviceDisconnected();

    /* Interface commands */
    void setOscillog(PFCconfig::Interface::PFCOscillogCnannel channel, std::vector<double> data);
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
    void setEvents(std::list<PFCconfig::Events::EventRecord> ev);
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
    void setConnection(bool _connected);
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
    void setSettingsFilters(
            float K_I,
            float K_U,
            float K_Ud);
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
    void updateOscillog(PFCconfig::Interface::PFCOscillogCnannel channel);
    void updateEvents(uint64_t afterIndex);
    void updateSettingsCalibrations();
    void updateSettingsProtection();
    void updateSettingsCapacitors();
    void updateSettingsFilters();    

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
            float K_Ud);
    void writeSwitchOnOff(PFCconfig::Interface::pfc_commands_t command,uint32_t data);

private slots:
    void onPushButtonClicked();
    void on_doubleSpinBox_capacitors_Kp_valueChanged(double arg1);
    void on_doubleSpinBox_capacitors_Ki_valueChanged(double arg1);
    void on_doubleSpinBox_capacitors_Kd_valueChanged(double arg1);
    void on_doubleSpinBox_cap_nominal_valueChanged(double arg1);
    void on_doubleSpinBox_cap_precharge_valueChanged(double arg1);
    void on_pushButton_STOP_clicked(void);
    void on_pushButton_Start_clicked(void);
    void on_pushButton_Save_clicked(void);
    void on_actionClear_triggered(void);
    void on_pushButton_2_clicked(void);
    void on_checkBox_channelA_toggled(bool checked);
    void on_checkBox_channelB_toggled(bool checked);
    void on_checkBox_channelC_toggled(bool checked);
    void on_pushButton_CHARGE_ON_clicked(void);
    void on_pushButton_CHARGE_OFF_clicked(void);

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
    bool _connected=false;
    QSharedPointer<QCPAxisTickerFixed> fixed_ticker;
};

/** @} */

#endif /* MAINWINDOW_H */
