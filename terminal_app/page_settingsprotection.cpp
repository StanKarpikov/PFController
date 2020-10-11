#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "settingsdialog.h"
#include <QMessageBox>
#include <QMessageBox>
#include <QDateTime>
#include <QGraphicsDropShadowEffect>
#include <QListWidgetItem>

void MainWindow::SET_TABLE_PROT(table_rows row, float VAL)
{
    ui->tableWidget_settings_protection->item(enum_int(row), 0)->setText(QString().sprintf("%.2f", static_cast<double>(VAL)));
}

void MainWindow::pageSettingsProtectionInit()
{
    for (int i = 0; i < ui->tableWidget_settings_protection->rowCount(); i++)
    {
        ui->tableWidget_settings_protection->item(i, 0)->setFlags(Qt::ItemIsEnabled | Qt::ItemIsEditable);
        ui->tableWidget_settings_protection->item(i, 1)->setFlags(Q_NULLPTR);
        ui->tableWidget_settings_protection->item(i, 2)->setFlags(Q_NULLPTR);
        ui->tableWidget_settings_protection->item(i, 0)->setBackground(editableCellBrush);
    }
    connect(ui->tableWidget_settings_protection, SIGNAL(cellChanged(int, int)),
            this, SLOT(tableSettingsProtection_changed(int, int)));
}

void MainWindow::tableSettingsProtection_changed(int row, int col)
{
    QTableWidgetItem *item = ui->tableWidget_settings_protection->item(row, col);
    float val = item->text().toFloat();
    switch (static_cast<table_rows>(row))
    {
        case table_rows::table_protection_row_Ud_min:
            pfc_settings.SETTINGS.PROTECTION.Ud_min = val;
            break;
        case table_rows::table_protection_row_Ud_max:
            pfc_settings.SETTINGS.PROTECTION.Ud_max = val;
            break;
        case table_rows::table_protection_row_temperature:
            pfc_settings.SETTINGS.PROTECTION.temperature = val;
            break;
        case table_rows::table_protection_row_U_min:
            pfc_settings.SETTINGS.PROTECTION.U_min = val;
            break;
        case table_rows::table_protection_row_U_max:
            pfc_settings.SETTINGS.PROTECTION.U_max = val;
            break;
        case table_rows::table_protection_row_Fnet_min:
            pfc_settings.SETTINGS.PROTECTION.Fnet_min = val;
            break;
        case table_rows::table_protection_row_Fnet_max:
            pfc_settings.SETTINGS.PROTECTION.Fnet_max = val;
            break;
        case table_rows::table_protection_row_I_max_rms:
            pfc_settings.SETTINGS.PROTECTION.I_max_rms = val;
            break;
        case table_rows::table_protection_row_I_max_peak:
            pfc_settings.SETTINGS.PROTECTION.I_max_peak = val;
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
        pfc_settings.SETTINGS.PROTECTION.I_max_peak);
}

void MainWindow::setSettingsProtection(
    float Ud_min,
    float Ud_max,
    float temperature,
    float U_min,
    float U_max,
    float Fnet_min,
    float Fnet_max,
    float I_max_rms,
    float I_max_peak)
{
    pfc_settings.SETTINGS.PROTECTION.Ud_min = Ud_min;
    pfc_settings.SETTINGS.PROTECTION.Ud_max = Ud_max;
    pfc_settings.SETTINGS.PROTECTION.temperature = temperature;
    pfc_settings.SETTINGS.PROTECTION.U_min = U_min;
    pfc_settings.SETTINGS.PROTECTION.U_max = U_max;
    pfc_settings.SETTINGS.PROTECTION.Fnet_min = Fnet_min;
    pfc_settings.SETTINGS.PROTECTION.Fnet_max = Fnet_max;
    pfc_settings.SETTINGS.PROTECTION.I_max_rms = I_max_rms;
    pfc_settings.SETTINGS.PROTECTION.I_max_peak = I_max_peak;

    SET_TABLE_PROT(table_rows::table_protection_row_Ud_min, Ud_min);
    SET_TABLE_PROT(table_rows::table_protection_row_Ud_max, Ud_max);
    SET_TABLE_PROT(table_rows::table_protection_row_temperature, temperature);
    SET_TABLE_PROT(table_rows::table_protection_row_U_min, U_min);
    SET_TABLE_PROT(table_rows::table_protection_row_U_max, U_max);
    SET_TABLE_PROT(table_rows::table_protection_row_Fnet_min, Fnet_min);
    SET_TABLE_PROT(table_rows::table_protection_row_Fnet_max, Fnet_max);
    SET_TABLE_PROT(table_rows::table_protection_row_I_max_rms, I_max_rms);
    SET_TABLE_PROT(table_rows::table_protection_row_I_max_peak, I_max_peak);
}
