
/*--------------------------------------------------------------
                       INCLUDES
--------------------------------------------------------------*/

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "settingsdialog.h"
#include <QMessageBox>
#include <QMessageBox>
#include <QDateTime>
#include <QGraphicsDropShadowEffect>
#include <QListWidgetItem>

/*--------------------------------------------------------------
                       PRIVATE FUNCTIONS
--------------------------------------------------------------*/

void MainWindow::setTableProtectionsVal(TableProtectionRows row, float value)
{
    _ui->tableWidget_settings_protection->item(enum_int(row), 0)->setText(QString().sprintf("%.2f", static_cast<double>(value)));
}

void MainWindow::pageSettingsProtectionInit()
{
    for (int i = 0; i < _ui->tableWidget_settings_protection->rowCount(); i++)
    {
        _ui->tableWidget_settings_protection->item(i, 0)->setFlags(Qt::ItemIsEnabled | Qt::ItemIsEditable);
        _ui->tableWidget_settings_protection->item(i, 1)->setFlags(Q_NULLPTR);
        _ui->tableWidget_settings_protection->item(i, 2)->setFlags(Q_NULLPTR);
        _ui->tableWidget_settings_protection->item(i, 0)->setBackground(editableCellBrush);
    }
    connect(_ui->tableWidget_settings_protection, SIGNAL(cellChanged(int, int)),
            this, SLOT(tableSettingsProtectionChanged(int, int)));
}

void MainWindow::tableSettingsProtectionChanged(int row, int col)
{
    QTableWidgetItem *item = _ui->tableWidget_settings_protection->item(row, col);
    float value = item->text().toFloat();
    switch (static_cast<TableProtectionRows>(row))
    {
        case TableProtectionRows::ROW_UD_MIN:
            _pfc_settings.settings.protection.Ud_min = value;
            break;
        case TableProtectionRows::ROW_UD_MAX:
            _pfc_settings.settings.protection.Ud_max = value;
            break;
        case TableProtectionRows::ROW_TEMPERATURE:
            _pfc_settings.settings.protection.temperature = value;
            break;
        case TableProtectionRows::ROW_U_MIN:
            _pfc_settings.settings.protection.U_min = value;
            break;
        case TableProtectionRows::ROW_U_MAX:
            _pfc_settings.settings.protection.U_max = value;
            break;
        case TableProtectionRows::ROW_F_MIN:
            _pfc_settings.settings.protection.Fnet_min = value;
            break;
        case TableProtectionRows::ROW_F_MAX:
            _pfc_settings.settings.protection.Fnet_max = value;
            break;
        case TableProtectionRows::ROW_I_MAX_RMS:
            _pfc_settings.settings.protection.I_max_rms = value;
            break;
        case TableProtectionRows::ROW_I_MAX_PEAK:
            _pfc_settings.settings.protection.I_max_peak = value;
            break;
    }
    writeSettingsProtection(
        _pfc_settings.settings.protection.Ud_min,
        _pfc_settings.settings.protection.Ud_max,
        _pfc_settings.settings.protection.temperature,
        _pfc_settings.settings.protection.U_min,
        _pfc_settings.settings.protection.U_max,
        _pfc_settings.settings.protection.Fnet_min,
        _pfc_settings.settings.protection.Fnet_max,
        _pfc_settings.settings.protection.I_max_rms,
        _pfc_settings.settings.protection.I_max_peak);
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
    _pfc_settings.settings.protection.Ud_min = Ud_min;
    _pfc_settings.settings.protection.Ud_max = Ud_max;
    _pfc_settings.settings.protection.temperature = temperature;
    _pfc_settings.settings.protection.U_min = U_min;
    _pfc_settings.settings.protection.U_max = U_max;
    _pfc_settings.settings.protection.Fnet_min = Fnet_min;
    _pfc_settings.settings.protection.Fnet_max = Fnet_max;
    _pfc_settings.settings.protection.I_max_rms = I_max_rms;
    _pfc_settings.settings.protection.I_max_peak = I_max_peak;

    setTableProtectionsVal(TableProtectionRows::ROW_UD_MIN, Ud_min);
    setTableProtectionsVal(TableProtectionRows::ROW_UD_MAX, Ud_max);
    setTableProtectionsVal(TableProtectionRows::ROW_TEMPERATURE, temperature);
    setTableProtectionsVal(TableProtectionRows::ROW_U_MIN, U_min);
    setTableProtectionsVal(TableProtectionRows::ROW_U_MAX, U_max);
    setTableProtectionsVal(TableProtectionRows::ROW_F_MIN, Fnet_min);
    setTableProtectionsVal(TableProtectionRows::ROW_F_MAX, Fnet_max);
    setTableProtectionsVal(TableProtectionRows::ROW_I_MAX_RMS, I_max_rms);
    setTableProtectionsVal(TableProtectionRows::ROW_I_MAX_PEAK, I_max_peak);
}
