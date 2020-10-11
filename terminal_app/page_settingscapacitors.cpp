#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "settingsdialog.h"
#include <QMessageBox>
#include <QMessageBox>
#include <QDateTime>
#include <QGraphicsDropShadowEffect>
#include <QListWidgetItem>

void MainWindow::pageSettingsCapacitorsInit()
{
}

void MainWindow::updateSpinVal(QDoubleSpinBox *spinbox, float value)
{
    if (!spinbox->hasFocus())
    {
        spinbox->blockSignals(true);
        spinbox->setValue(static_cast<double>(value));
        spinbox->blockSignals(false);
    }
}

void MainWindow::setSettingsCapacitors(
    float ctrlUd_Kp,  //!< Управление накачкой
    float ctrlUd_Ki,
    float ctrlUd_Kd,
    float Ud_nominal,
    float Ud_precharge)
{
    _pfc_settings.SETTINGS.CAPACITORS.ctrlUd_Kp = ctrlUd_Kp;
    _pfc_settings.SETTINGS.CAPACITORS.ctrlUd_Ki = ctrlUd_Ki;
    _pfc_settings.SETTINGS.CAPACITORS.ctrlUd_Kd = ctrlUd_Kd;
    _pfc_settings.SETTINGS.CAPACITORS.Ud_nominal = Ud_nominal;
    _pfc_settings.SETTINGS.CAPACITORS.Ud_precharge = Ud_precharge;

    uint16_t value = 0;
    if (_pfc_settings.ADC.ADC_UD < 0)
    {  //< precharge
        value = 0;
    }
    else if (_pfc_settings.ADC.ADC_UD < (Ud_precharge))
    {  //< precharge
        value = static_cast<uint16_t>(_pfc_settings.ADC.ADC_UD / Ud_precharge * (1000.0f / 4.0f));
    }
    else if (_pfc_settings.ADC.ADC_UD < (Ud_nominal))
    {  //precharge < x < nominal
        value = static_cast<uint16_t>((_pfc_settings.ADC.ADC_UD - Ud_precharge) / (Ud_nominal - Ud_precharge) * (1000.0f / 2.0f) + (1000.0f / 4.0f));
    }
    else
    {  //> nominal
        value = static_cast<uint16_t>((_pfc_settings.ADC.ADC_UD - Ud_nominal) / (UD_MAX_VALUE - Ud_nominal) * (1000.0f / 4.0f) + (3.0f * 1000.0f / 4.0f));
    }

    _ui->progressBar_Ud1->setValue(value);

    updateSpinVal(_ui->doubleSpinBox_cap_nominal, Ud_nominal);
    updateSpinVal(_ui->doubleSpinBox_cap_precharge, Ud_precharge);
    updateSpinVal(_ui->doubleSpinBox_capacitors_Kp, ctrlUd_Kp);
    updateSpinVal(_ui->doubleSpinBox_capacitors_Ki, ctrlUd_Ki);
    updateSpinVal(_ui->doubleSpinBox_capacitors_Kd, ctrlUd_Kd);

    _ui->label_Ud1->setText(QString().sprintf("%.0f В", static_cast<double>(_pfc_settings.ADC.ADC_UD)));
}

void MainWindow::capacitorsKpValueChanged(double arg)
{
    _pfc_settings.SETTINGS.CAPACITORS.ctrlUd_Kp = static_cast<float>(arg);

    writeSettingsCapacitors(
        _pfc_settings.SETTINGS.CAPACITORS.ctrlUd_Kp,
        _pfc_settings.SETTINGS.CAPACITORS.ctrlUd_Ki,
        _pfc_settings.SETTINGS.CAPACITORS.ctrlUd_Kd,
        _pfc_settings.SETTINGS.CAPACITORS.Ud_nominal,
        _pfc_settings.SETTINGS.CAPACITORS.Ud_precharge);
}
void MainWindow::capacitorsKiValueChanged(double arg)
{
    _pfc_settings.SETTINGS.CAPACITORS.ctrlUd_Ki = static_cast<float>(arg);

    writeSettingsCapacitors(
        _pfc_settings.SETTINGS.CAPACITORS.ctrlUd_Kp,
        _pfc_settings.SETTINGS.CAPACITORS.ctrlUd_Ki,
        _pfc_settings.SETTINGS.CAPACITORS.ctrlUd_Kd,
        _pfc_settings.SETTINGS.CAPACITORS.Ud_nominal,
        _pfc_settings.SETTINGS.CAPACITORS.Ud_precharge);
}
void MainWindow::capacitorsKdValueChanged(double arg)
{
    _pfc_settings.SETTINGS.CAPACITORS.ctrlUd_Kd = static_cast<float>(arg);

    writeSettingsCapacitors(
        _pfc_settings.SETTINGS.CAPACITORS.ctrlUd_Kp,
        _pfc_settings.SETTINGS.CAPACITORS.ctrlUd_Ki,
        _pfc_settings.SETTINGS.CAPACITORS.ctrlUd_Kd,
        _pfc_settings.SETTINGS.CAPACITORS.Ud_nominal,
        _pfc_settings.SETTINGS.CAPACITORS.Ud_precharge);
}
void MainWindow::capacitorsNominalValueChanged(double arg)
{
    _pfc_settings.SETTINGS.CAPACITORS.Ud_nominal = static_cast<float>(arg);

    writeSettingsCapacitors(
        _pfc_settings.SETTINGS.CAPACITORS.ctrlUd_Kp,
        _pfc_settings.SETTINGS.CAPACITORS.ctrlUd_Ki,
        _pfc_settings.SETTINGS.CAPACITORS.ctrlUd_Kd,
        _pfc_settings.SETTINGS.CAPACITORS.Ud_nominal,
        _pfc_settings.SETTINGS.CAPACITORS.Ud_precharge);
}
void MainWindow::capacitorsPrechargeValueChanged(double arg)
{
    _pfc_settings.SETTINGS.CAPACITORS.Ud_precharge = static_cast<float>(arg);

    writeSettingsCapacitors(
        _pfc_settings.SETTINGS.CAPACITORS.ctrlUd_Kp,
        _pfc_settings.SETTINGS.CAPACITORS.ctrlUd_Ki,
        _pfc_settings.SETTINGS.CAPACITORS.ctrlUd_Kd,
        _pfc_settings.SETTINGS.CAPACITORS.Ud_nominal,
        _pfc_settings.SETTINGS.CAPACITORS.Ud_precharge);
}
