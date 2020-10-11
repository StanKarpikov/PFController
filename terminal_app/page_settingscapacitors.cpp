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

void MainWindow::UPDATE_SPINBOX(QDoubleSpinBox *SPIN, float VAL)
{
    if (!SPIN->hasFocus())
    {
        SPIN->blockSignals(true);
        SPIN->setValue(static_cast<double>(VAL));
        SPIN->blockSignals(false);
    }
}

void MainWindow::setSettingsCapacitors(
    float ctrlUd_Kp,  //!< Управление накачкой
    float ctrlUd_Ki,
    float ctrlUd_Kd,
    float Ud_nominal,
    float Ud_precharge)
{
    pfc_settings.SETTINGS.CAPACITORS.ctrlUd_Kp = ctrlUd_Kp;
    pfc_settings.SETTINGS.CAPACITORS.ctrlUd_Ki = ctrlUd_Ki;
    pfc_settings.SETTINGS.CAPACITORS.ctrlUd_Kd = ctrlUd_Kd;
    pfc_settings.SETTINGS.CAPACITORS.Ud_nominal = Ud_nominal;
    pfc_settings.SETTINGS.CAPACITORS.Ud_precharge = Ud_precharge;

    uint16_t val = 0;
    if (pfc_settings.ADC.ADC_UD < 0)
    {  //< precharge
        val = 0;
    }
    else if (pfc_settings.ADC.ADC_UD < (Ud_precharge))
    {  //< precharge
        val = static_cast<uint16_t>(pfc_settings.ADC.ADC_UD / Ud_precharge * (1000.0f / 4.0f));
    }
    else if (pfc_settings.ADC.ADC_UD < (Ud_nominal))
    {  //precharge < x < nominal
        val = static_cast<uint16_t>((pfc_settings.ADC.ADC_UD - Ud_precharge) / (Ud_nominal - Ud_precharge) * (1000.0f / 2.0f) + (1000.0f / 4.0f));
    }
    else
    {  //> nominal
        val = static_cast<uint16_t>((pfc_settings.ADC.ADC_UD - Ud_nominal) / (UD_MAX_VALUE - Ud_nominal) * (1000.0f / 4.0f) + (3.0f * 1000.0f / 4.0f));
    }

    ui->progressBar_Ud1->setValue(val);

    UPDATE_SPINBOX(ui->doubleSpinBox_cap_nominal, Ud_nominal);
    UPDATE_SPINBOX(ui->doubleSpinBox_cap_precharge, Ud_precharge);
    UPDATE_SPINBOX(ui->doubleSpinBox_capacitors_Kp, ctrlUd_Kp);
    UPDATE_SPINBOX(ui->doubleSpinBox_capacitors_Ki, ctrlUd_Ki);
    UPDATE_SPINBOX(ui->doubleSpinBox_capacitors_Kd, ctrlUd_Kd);

    ui->label_Ud1->setText(QString().sprintf("%.0f В", static_cast<double>(pfc_settings.ADC.ADC_UD)));
}

void MainWindow::on_doubleSpinBox_capacitors_Kp_valueChanged(double arg1)
{
    pfc_settings.SETTINGS.CAPACITORS.ctrlUd_Kp = static_cast<float>(arg1);

    writeSettingsCapacitors(
        pfc_settings.SETTINGS.CAPACITORS.ctrlUd_Kp,
        pfc_settings.SETTINGS.CAPACITORS.ctrlUd_Ki,
        pfc_settings.SETTINGS.CAPACITORS.ctrlUd_Kd,
        pfc_settings.SETTINGS.CAPACITORS.Ud_nominal,
        pfc_settings.SETTINGS.CAPACITORS.Ud_precharge);
}
void MainWindow::on_doubleSpinBox_capacitors_Ki_valueChanged(double arg1)
{
    pfc_settings.SETTINGS.CAPACITORS.ctrlUd_Ki = static_cast<float>(arg1);

    writeSettingsCapacitors(
        pfc_settings.SETTINGS.CAPACITORS.ctrlUd_Kp,
        pfc_settings.SETTINGS.CAPACITORS.ctrlUd_Ki,
        pfc_settings.SETTINGS.CAPACITORS.ctrlUd_Kd,
        pfc_settings.SETTINGS.CAPACITORS.Ud_nominal,
        pfc_settings.SETTINGS.CAPACITORS.Ud_precharge);
}
void MainWindow::on_doubleSpinBox_capacitors_Kd_valueChanged(double arg1)
{
    pfc_settings.SETTINGS.CAPACITORS.ctrlUd_Kd = static_cast<float>(arg1);

    writeSettingsCapacitors(
        pfc_settings.SETTINGS.CAPACITORS.ctrlUd_Kp,
        pfc_settings.SETTINGS.CAPACITORS.ctrlUd_Ki,
        pfc_settings.SETTINGS.CAPACITORS.ctrlUd_Kd,
        pfc_settings.SETTINGS.CAPACITORS.Ud_nominal,
        pfc_settings.SETTINGS.CAPACITORS.Ud_precharge);
}
void MainWindow::on_doubleSpinBox_cap_nominal_valueChanged(double arg1)
{
    pfc_settings.SETTINGS.CAPACITORS.Ud_nominal = static_cast<float>(arg1);

    writeSettingsCapacitors(
        pfc_settings.SETTINGS.CAPACITORS.ctrlUd_Kp,
        pfc_settings.SETTINGS.CAPACITORS.ctrlUd_Ki,
        pfc_settings.SETTINGS.CAPACITORS.ctrlUd_Kd,
        pfc_settings.SETTINGS.CAPACITORS.Ud_nominal,
        pfc_settings.SETTINGS.CAPACITORS.Ud_precharge);
}
void MainWindow::on_doubleSpinBox_cap_precharge_valueChanged(double arg1)
{
    pfc_settings.SETTINGS.CAPACITORS.Ud_precharge = static_cast<float>(arg1);

    writeSettingsCapacitors(
        pfc_settings.SETTINGS.CAPACITORS.ctrlUd_Kp,
        pfc_settings.SETTINGS.CAPACITORS.ctrlUd_Ki,
        pfc_settings.SETTINGS.CAPACITORS.ctrlUd_Kd,
        pfc_settings.SETTINGS.CAPACITORS.Ud_nominal,
        pfc_settings.SETTINGS.CAPACITORS.Ud_precharge);
}
