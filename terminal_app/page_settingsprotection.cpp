#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "settingsdialog.h"
#include <QMessageBox>
#include <QMessageBox>
#include <QDateTime>
#include <QGraphicsDropShadowEffect>
#include <QListWidgetItem>

enum {
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

void MainWindow::pageSettingsProtectionInit(){
    for(int i=0;i<ui->tableWidget_settings_protection->rowCount();i++){
        ui->tableWidget_settings_protection->item(i, 0)->setFlags(Qt::ItemIsEnabled | Qt::ItemIsEditable);
        ui->tableWidget_settings_protection->item(i, 1)->setFlags(0);
        ui->tableWidget_settings_protection->item(i, 2)->setFlags(0);
        ui->tableWidget_settings_protection->item(i, 0)->setBackground(editableCellBrush);
    }
    connect(ui->tableWidget_settings_protection, SIGNAL(cellChanged(int,int)),
            this, SLOT(tableSettingsProtection_changed(int,int)));
}

void MainWindow::tableSettingsProtection_changed(int row, int col){
    QTableWidgetItem *item=ui->tableWidget_settings_protection->item(row,col);
    float val=item->text().toFloat();
    switch(row){
        case table_protection_row_Ud_min:
            pfc_settings.SETTINGS.PROTECTION.Ud_min=val;
        break;
        case table_protection_row_Ud_max:
            pfc_settings.SETTINGS.PROTECTION.Ud_max=val;
        break;
        case table_protection_row_temperature:
            pfc_settings.SETTINGS.PROTECTION.temperature=val;
        break;
        case table_protection_row_U_min:
            pfc_settings.SETTINGS.PROTECTION.U_min=val;
        break;
        case table_protection_row_U_max:
            pfc_settings.SETTINGS.PROTECTION.U_max=val;
        break;
        case table_protection_row_Fnet_min:
            pfc_settings.SETTINGS.PROTECTION.Fnet_min=val;
        break;
        case table_protection_row_Fnet_max:
            pfc_settings.SETTINGS.PROTECTION.Fnet_max=val;
        break;
        case table_protection_row_I_max_rms:
            pfc_settings.SETTINGS.PROTECTION.I_max_rms=val;
        break;
        case table_protection_row_I_max_peak:
            pfc_settings.SETTINGS.PROTECTION.I_max_peak=val;
        break;
    }
    writeSettingsProtection(
                pfc_settings.SETTINGS.PROTECTION.Ud_min,
                pfc_settings.SETTINGS.PROTECTION.Ud_max,
                pfc_settings.SETTINGS.PROTECTION.temperature,
                pfc_settings.SETTINGS.PROTECTION.U_min,
                pfc_settings.SETTINGS.PROTECTION.U_max,
                pfc_settings.SETTINGS.PROTECTION.Fnet_min,
                pfc_settings.SETTINGS.PROTECTION.Fnet_max,
                pfc_settings.SETTINGS.PROTECTION.I_max_rms,
                pfc_settings.SETTINGS.PROTECTION.I_max_peak
                );
}
//========================================================================
void MainWindow::setSettingsProtection(
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
        ){
    pfc_settings.SETTINGS.PROTECTION.Ud_min         =Ud_min;
    pfc_settings.SETTINGS.PROTECTION.Ud_max         =Ud_max;
    pfc_settings.SETTINGS.PROTECTION.temperature    =temperature;
    pfc_settings.SETTINGS.PROTECTION.U_min          =U_min;
    pfc_settings.SETTINGS.PROTECTION.U_max          =U_max;
    pfc_settings.SETTINGS.PROTECTION.Fnet_min       =Fnet_min;
    pfc_settings.SETTINGS.PROTECTION.Fnet_max       =Fnet_max;
    pfc_settings.SETTINGS.PROTECTION.I_max_rms      =I_max_rms;
    pfc_settings.SETTINGS.PROTECTION.I_max_peak     =I_max_peak;

#define SET_TABLE_PROT(VAL)\
    ui->tableWidget_settings_protection->item(table_protection_row_##VAL,0)->setText(QString().sprintf("%.2f",VAL));

    SET_TABLE_PROT(Ud_min);
    SET_TABLE_PROT(Ud_max);
    SET_TABLE_PROT(temperature);
    SET_TABLE_PROT(U_min);
    SET_TABLE_PROT(U_max);
    SET_TABLE_PROT(Fnet_min);
    SET_TABLE_PROT(Fnet_max);
    SET_TABLE_PROT(I_max_rms);
    SET_TABLE_PROT(I_max_peak);
}
