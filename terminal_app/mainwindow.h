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
#include <QtSerialPort/QSerialPort>
#include <QTimer>
#include <device.h>
#include <QThread>
#include <QSignalSpy>

#include "qcustomplot.h"
#include "device_definition.h"

/*--------------------------------------------------------------
                       PUBLIC TYPES
--------------------------------------------------------------*/

QT_BEGIN_NAMESPACE

namespace Ui {
    class MainWindow;
}

QT_END_NAMESPACE

class SettingsDialog;
class PFC;

/*--------------------------------------------------------------
                       CLASSES
--------------------------------------------------------------*/

class PFCsettings{
    /* TODO: is not thread safe */
public:
    struct {
        float ADC_UD;
        float ADC_U_A;
        float ADC_U_B;
        float ADC_U_C;
        float ADC_I_A;
        float ADC_I_B;
        float ADC_I_C;
        float ADC_I_ET;
        float ADC_I_TEMP1;
        float ADC_I_TEMP2;
        float ADC_EMS_A;
        float ADC_EMS_B;
        float ADC_EMS_C;
        float ADC_EMS_I;
        float ADC_MATH_A;
        float ADC_MATH_B;
        float ADC_MATH_C;
    }adc;
    struct {
        float ADC_UD;
        float ADC_U_A;
        float ADC_U_B;
        float ADC_U_C;
        float ADC_I_A;
        float ADC_I_B;
        float ADC_I_C;
        float ADC_I_ET;
        float ADC_I_TEMP1;
        float ADC_I_TEMP2;
        float ADC_EMS_A;
        float ADC_EMS_B;
        float ADC_EMS_C;
        float ADC_EMS_I;
    }adc_raw;
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
    }net_params;
    struct {
        struct {
            std::vector<float> calibration;
            std::vector<float> offset;
        }calibrations;
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
        }protection;
        struct {
            float ctrlUd_Kp;
            float ctrlUd_Ki;
            float ctrlUd_Kd;
            float Ud_nominal;
            float Ud_precharge;
        }capacitors;
        struct{
            float K_I;
            float K_U;
            float K_Ud;
        }filters;
    }settings;
    uint32_t status;
    uint32_t active_channels[PFCconfig::PFC_NCHAN];

    PFCsettings(void)
    {
        memset(&adc, 0, sizeof(adc));
        memset(&adc_raw, 0, sizeof(adc_raw));
        memset(&net_params, 0, sizeof(net_params));
        memset(&settings.protection, 0, sizeof(settings.protection));
        memset(&settings.capacitors, 0, sizeof(settings.capacitors));
        memset(&settings.filters, 0, sizeof(settings.filters));
        status = 0;
        memset(&active_channels, 0, sizeof(active_channels));
    }
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = Q_NULLPTR);
    ~MainWindow();

    /*--------------------------------------------------------------
                           PRIVATE DATA
    --------------------------------------------------------------*/
private:
    /* Constants */
    inline static const std::string DARK_GREY = "#808080";
    inline static const std::string DARK_RED = "#800000";
    inline static const std::string LIGHT_GREY = "#c0c0c0";
    inline static const std::string DARK_GREEN = "#008000";

    static const QBrush editableCellBrush;

    static constexpr auto FCOEFF = 0.9f;

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

    enum class TableProtectionRows
    {
        ROW_UD_MIN,
        ROW_UD_MAX,
        ROW_TEMPERATURE,
        ROW_U_MIN,
        ROW_U_MAX,
        ROW_F_MIN,
        ROW_F_MAX,
        ROW_I_MAX_RMS,
        ROW_I_MAX_PEAK
    };

    enum class TableFiltersRows
    {
        table_filters_row_K_I,
        table_filters_row_K_U,
        table_filters_row_K_Ud
    };

    /* Data */
    Ui::MainWindow *_ui;
    PFC *_pfc;
    PFCsettings _pfc_settings;
    std::vector<double> _oscillog_xval, _harmonics_xval;
    std::vector<std::vector<double>> _oscillog_data;
    std::list<QPushButton*> _buttons_edit;
    uint64_t _last_index_events;
    QMap<PFCconfig::Interface::PFCOscillogCnannel,OscillogChannels> _oscillog_array;
    SettingsDialog *_port_settings;
    QTimer _timer_main_params;
    QTimer _timer_raw;
    QTimer _timer_state;
    QTimer _timer_voltage;
    QTimer _timer_version;
    QTimer _timer_oscillog;
    QTimer _timer_events;
    QTimer _timer_settings_calibrations;
    QTimer _timer_settings_capacitors;
    QTimer _timer_settings_protection;
    QTimer _timer_settings_filters;
    bool _connected;
    QSharedPointer<QCPAxisTickerFixed> _fixed_ticker;

    /*--------------------------------------------------------------
                           PRIVATE FUNCTIONS
    --------------------------------------------------------------*/
private:
    bool eventFilter(QObject *object, QEvent *event);
    void pageMainInit(void);
    void pageOscillogInit(void);
    void pageSettingsCalibrationsInit(void);
    void pageSettingsCapacitorsInit(void);
    void pageSettingsProtectionInit(void);
    void pageSettingsFiltersInit(void);
    void setTableProtectionsVal(TableProtectionRows row, float value);
    void updateSpinVal(QDoubleSpinBox *spinbox, float value);
    void updateCheckboxVal(QCheckBox* checkbox, bool value);
    void initInterfaceConnections(void);
    void filterApply(float &A, float B);
    void setFilter(QEvent* event, QObject* object, QWidget* ui_obj, QTimer* obj, std::chrono::milliseconds timeout);
    std::string stringWithColor(std::string str, std::string color);

private slots:
    void buttonAutoConfigOscClicked(void);
    void capacitorsKpValueChanged(double arg);
    void capacitorsKiValueChanged(double arg);
    void capacitorsKdValueChanged(double arg);
    void capacitorsNominalValueChanged(double arg);
    void capacitorsPrechargeValueChanged(double arg);
    void stopClicked(void);
    void startClicked(void);
    void saveClicked(void);
    void actionClearTriggered(void);
    void channelACheckToggled(bool checked);
    void channelBCheckToggled(bool checked);
    void channelCCheckToggled(bool checked);
    void chargeOnClicked(void);
    void chargeOffClicked(void);

    /*--------------------------------------------------------------
                           PUBLIC FUNCTIONS
    --------------------------------------------------------------*/
public slots:
    void openSerialPort(void);
    void closeSerialPort(void);
    void about(void);
    void message(uint8_t type, uint8_t level, uint8_t target, std::string message);
    void deviceConnected(void);
    void deviceDisconnected(void);

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
    void setWorkState(uint32_t state, uint32_t ch_a, uint32_t ch_b, uint32_t ch_c);
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

    void tableSettingsCalibrationsChanged(int row, int col);
    void tableSettingsProtectionChanged(int row, int col);
    void tableSettingsFiltersChanged(int row, int col);

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
            std::vector<float> offset);
    void writeSettingsProtection(
            float Ud_min,
            float Ud_max,
            float temperature,
            float U_min,
            float U_max,
            float Fnet_min,
            float Fnet_max,
            float I_max_rms,
            float I_max_peak);
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
};

/** @} */

#endif /* MAINWINDOW_H */
