/****************************************************************************
**
** Copyright (C) 2012 Denis Shienkov <denis.shienkov@gmail.com>
** Copyright (C) 2012 Laszlo Papp <lpapp@kde.org>
** Contact: http://www.qt.io/licensing/
**
** This file is part of the QtSerialPort module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL21$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 or version 3 as published by the Free
** Software Foundation and appearing in the file LICENSE.LGPLv21 and
** LICENSE.LGPLv3 included in the packaging of this file. Please review the
** following information to ensure the GNU Lesser General Public License
** requirements will be met: https://www.gnu.org/licenses/lgpl.html and
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** As a special exception, The Qt Company gives you certain additional
** rights. These rights are described in The Qt Company LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

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
QT_BEGIN_NAMESPACE

namespace Ui {
    class MainWindow;
}

QT_END_NAMESPACE

enum{
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

    OSCILLOG_size
};

const QBrush editableCellBrush(QColor(240, 255, 240));

enum {
    KKM_STATE_INIT, //!< CLOGIC_INIT Первоначальное состояние. Сброс контакторов.
    KKM_STATE_STOP, //не работает
    KKM_STATE_SYNC, //синхронизация с сетью
    KKM_STATE_PRECHARGE_PREPARE, //!< CLOGIC_PRELOAD_PREPARE Подготовка к включению предзаряда. Выключает Ш� М
    KKM_STATE_PRECHARGE, //предзаряд
    KKM_STATE_MAIN, //!< CLOGIC_MAIN Включение главного контактора
    KKM_STATE_PRECHARGE_DISABLE, //!< CLOGIC_PRELOAD_DISABLE Отключение предзаряда
    KKM_STATE_WORK,
    KKM_STATE_CHARGE, //заряд (накачка)
    KKM_STATE_TEST, //тестирование сети
    KKM_STATE_STOPPING, //!< CLOGIC_STOPPING Выключение контакторов
    KKM_STATE_FAULTBLOCK //ошибка
};

#include "types.h"
class SettingsDialog;
class KKM;
#define NOMINAL_VOLTAGE 220
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    Ui::MainWindow *ui;
    KKM *kkm;
    bool eventFilter(QObject *object, QEvent *event);
    std::vector<double> oscillog_xval,harmonics_xval;
    QVector<double> oscillog_data[OSCILLOG_size];
    QVector<QPushButton*> btns_edit;
    uint64_t lastIndexEvents=0;

    void page_Main_Init();
    void page_Oscillog_Init();
    void page_SettingsCalibrations_Init();
    void page_SettingsCapacitors_Init();
    void page_SettingsProtection_Init();
    void page_SettingsFilters_Init();

    struct {
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
    }KKM_var;
public slots:
    void openSerialPort();
    void closeSerialPort();
    void about();
    void Message(quint8 type, quint8 level, quint8 target, QString message);
    void KKMconnected();
    void KKMdisconnected();
  //  void tableSettingsCalibrations_SetAutoSettings();
//==================================================================
//==================================================================
//==================================================================
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
            //=== калибровки АЦП
            QVector<float> calibration, //!< Калибровки для каналов
            QVector<float> offset //!< Смещения для каналов
            );
    void setSettingsFilters(
            float K_I,
            float K_U,
            float K_Ud);
    void setSettingsProtection(
            //=== защиты ======
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
//==================================================================
//==================================================================
//==================================================================
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
    void xAxisRangeChanged( QCPRange newRange ,QCPRange oldRange);
    void mouseWheel(QWheelEvent *event);
  // void tableSettingsCalibrations_auto_clicked(bool check);
    void tableSettingsCalibrations_changed(int row, int col);
    void tableSettingsProtection_changed(int row, int col);
    void tableSettingsFilters_changed(int row, int col);
signals:
//==================================================================
//==================================================================
//==================================================================
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
//==================================================================
    void writeSettingsCalibrations(
            //=== калибровки АЦП
            QVector<float> calibration, //!< Калибровки для каналов
            QVector<float> offset //!< Смещения для каналов
            );
    void writeSettingsProtection(
            //=== защиты ======
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
//==================================================================
//==================================================================
//==================================================================
private slots:
    void on_pushButton_clicked();
 //   void on_pushButton_autoharm_clicked();
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
    void initActionsConnections();
private:
    SettingsDialog *settings;
    QTimer timer_MainParams;
    QTimer timer_Raw;
    QTimer timer_State;
    QTimer timer_Vol;
    QTimer timer_Version;
    QTimer timer_Oscillog;    
    QTimer timer_Events;
    QTimer timer_SettingsCalibrations;
    QTimer timer_SettingsCapacitors;
    QTimer timer_SettingsProtection;
    QTimer timer_SettingsFilters;
    bool _connected=false;
    QSharedPointer<QCPAxisTickerFixed> fixedTicker;
};

#endif // MAINWINDOW_H
