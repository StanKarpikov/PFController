#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "settingsdialog.h"
#include <QMessageBox>
#include <QMessageBox>
#include <QDateTime>
#include <QGraphicsDropShadowEffect>
#include <QListWidgetItem>

#define UD_MAX_VALUE 500

void MainWindow::page_SettingsCapacitors_Init(){

}
#define UPDATE_SPINBOX(SPIN,VAL) \
    if(!SPIN->hasFocus()){\
        SPIN->blockSignals(true);\
        SPIN->setValue(VAL);\
        SPIN->blockSignals(false);\
    }
//========================================================================
void MainWindow::setSettingsCapacitors(
        float ctrlUd_Kp, //!< Управление накачкой
        float ctrlUd_Ki,
        float ctrlUd_Kd,
        float Ud_nominal,
        float Ud_precharge){
    KKM_var.SETTINGS.CAPACITORS.ctrlUd_Kp         =ctrlUd_Kp;
    KKM_var.SETTINGS.CAPACITORS.ctrlUd_Ki         =ctrlUd_Ki;
    KKM_var.SETTINGS.CAPACITORS.ctrlUd_Kd         =ctrlUd_Kd;
    KKM_var.SETTINGS.CAPACITORS.Ud_nominal        =Ud_nominal;
    KKM_var.SETTINGS.CAPACITORS.Ud_precharge      =Ud_precharge;

    uint16_t val=0;
    if(KKM_var.ADC.ADC_UD<0){ //< precharge
        val=0;
    }else if(KKM_var.ADC.ADC_UD<(Ud_precharge)){ //< precharge
        val=KKM_var.ADC.ADC_UD/Ud_precharge*(1000/4);
    }else if(KKM_var.ADC.ADC_UD<(Ud_nominal)){ //precharge < x < nominal
        val=(KKM_var.ADC.ADC_UD-Ud_precharge)/(Ud_nominal-Ud_precharge)*(1000/2)+(1000/4);
    }else{ //> nominal
        val=(KKM_var.ADC.ADC_UD-Ud_nominal)/(UD_MAX_VALUE-Ud_nominal)*(1000/4)+(3*1000/4);
    }

    ui->progressBar_Ud1->setValue(val);

    UPDATE_SPINBOX(ui->doubleSpinBox_cap_nominal,Ud_nominal);
    UPDATE_SPINBOX(ui->doubleSpinBox_cap_precharge,Ud_precharge);
    UPDATE_SPINBOX(ui->doubleSpinBox_capacitors_Kp,ctrlUd_Kp);
    UPDATE_SPINBOX(ui->doubleSpinBox_capacitors_Ki,ctrlUd_Ki);
    UPDATE_SPINBOX(ui->doubleSpinBox_capacitors_Kd,ctrlUd_Kd);

    ui->label_Ud1->setText(QString().sprintf("%.0f В",KKM_var.ADC.ADC_UD));
}

void MainWindow::on_doubleSpinBox_capacitors_Kp_valueChanged(double arg1)
{
    KKM_var.SETTINGS.CAPACITORS.ctrlUd_Kp=arg1;

    writeSettingsCapacitors(
        KKM_var.SETTINGS.CAPACITORS.ctrlUd_Kp,
        KKM_var.SETTINGS.CAPACITORS.ctrlUd_Ki,
        KKM_var.SETTINGS.CAPACITORS.ctrlUd_Kd,
        KKM_var.SETTINGS.CAPACITORS.Ud_nominal,
        KKM_var.SETTINGS.CAPACITORS.Ud_precharge
    );
}
void MainWindow::on_doubleSpinBox_capacitors_Ki_valueChanged(double arg1)
{
    KKM_var.SETTINGS.CAPACITORS.ctrlUd_Ki=arg1;

    writeSettingsCapacitors(
        KKM_var.SETTINGS.CAPACITORS.ctrlUd_Kp,
        KKM_var.SETTINGS.CAPACITORS.ctrlUd_Ki,
        KKM_var.SETTINGS.CAPACITORS.ctrlUd_Kd,
        KKM_var.SETTINGS.CAPACITORS.Ud_nominal,
        KKM_var.SETTINGS.CAPACITORS.Ud_precharge
    );
}
void MainWindow::on_doubleSpinBox_capacitors_Kd_valueChanged(double arg1)
{
    KKM_var.SETTINGS.CAPACITORS.ctrlUd_Kd=arg1;

    writeSettingsCapacitors(
        KKM_var.SETTINGS.CAPACITORS.ctrlUd_Kp,
        KKM_var.SETTINGS.CAPACITORS.ctrlUd_Ki,
        KKM_var.SETTINGS.CAPACITORS.ctrlUd_Kd,
        KKM_var.SETTINGS.CAPACITORS.Ud_nominal,
        KKM_var.SETTINGS.CAPACITORS.Ud_precharge
    );
}
void MainWindow::on_doubleSpinBox_cap_nominal_valueChanged(double arg1)
{
    KKM_var.SETTINGS.CAPACITORS.Ud_nominal=arg1;

    writeSettingsCapacitors(
        KKM_var.SETTINGS.CAPACITORS.ctrlUd_Kp,
        KKM_var.SETTINGS.CAPACITORS.ctrlUd_Ki,
        KKM_var.SETTINGS.CAPACITORS.ctrlUd_Kd,
        KKM_var.SETTINGS.CAPACITORS.Ud_nominal,
        KKM_var.SETTINGS.CAPACITORS.Ud_precharge
    );
}
void MainWindow::on_doubleSpinBox_cap_precharge_valueChanged(double arg1)
{
    KKM_var.SETTINGS.CAPACITORS.Ud_precharge=arg1;

    writeSettingsCapacitors(
        KKM_var.SETTINGS.CAPACITORS.ctrlUd_Kp,
        KKM_var.SETTINGS.CAPACITORS.ctrlUd_Ki,
        KKM_var.SETTINGS.CAPACITORS.ctrlUd_Kd,
        KKM_var.SETTINGS.CAPACITORS.Ud_nominal,
        KKM_var.SETTINGS.CAPACITORS.Ud_precharge
    );
}

