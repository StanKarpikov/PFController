#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "settingsdialog.h"
#include <QMessageBox>
#include <QMessageBox>
#include <QDateTime>
#include <QGraphicsDropShadowEffect>
#include <QListWidgetItem>

/*--------------------------------------------------------------
                       NAMESPACES
--------------------------------------------------------------*/

using namespace PFCconfig;
using namespace PFCconfig::ADC;
using namespace PFCconfig::Interface;
using namespace PFCconfig::Events;

/*--------------------------------------------------------------
                       PRIVATE FUNCTIONS
--------------------------------------------------------------*/

void MainWindow::pageSettingsCalibrationsInit(void)
{
    for (int row = 0; row < _ui->tableSettingsCalibrations->rowCount(); row++)
    {
        _ui->tableSettingsCalibrations->setItem(row, 0, new QTableWidgetItem());
        _ui->tableSettingsCalibrations->item(row, 0)->setFlags(Qt::ItemIsEnabled | Qt::ItemIsEditable);
        _ui->tableSettingsCalibrations->item(row, 0)->setBackground(editableCellBrush);
        _ui->tableSettingsCalibrations->setItem(row, 1, new QTableWidgetItem());
        _ui->tableSettingsCalibrations->setItem(row, 2, new QTableWidgetItem());
        Qt::ItemFlags eFlags;
        QPushButton *btn_edit;
        btn_edit = new QPushButton();
        _buttons_edit.push_back(btn_edit);

        eFlags = _ui->tableSettingsCalibrations->item(row, 1)->flags();
        eFlags &= ~Qt::ItemIsEditable;
        _ui->tableSettingsCalibrations->item(row, 1)->setFlags(eFlags);

        QWidget* pWidget;
        QHBoxLayout* pLayout;

        pWidget = new QWidget();
        btn_edit->setText("Auto");
        pLayout = new QHBoxLayout(pWidget);
        pLayout->addWidget(btn_edit);
        pLayout->setAlignment(Qt::AlignCenter);
        pLayout->setContentsMargins(0, 0, 0, 0);
        pWidget->setLayout(pLayout);
        _ui->tableSettingsCalibrations->setCellWidget(row, enum_int(TableCalibrationColumns::TABLE_CALIBRATIONS_AUTO_BUTTON_COLUMN), pWidget);
        connect(btn_edit, &QPushButton::clicked,
                this, &MainWindow::tableSettingsCalibrationsAutoClicked);
        _btns_edit.push_back(btn_edit);
    }
    connect(_ui->tableSettingsCalibrations, &QTableWidget::cellChanged,
            this, &MainWindow::tableSettingsCalibrationsChanged);
    _pfc_settings.settings.calibrations.calibration.resize(ADC_CHANNEL_NUMBER);
    _pfc_settings.settings.calibrations.offset.resize(ADC_CHANNEL_NUMBER);
}


float MainWindow::CALC_AUTO_COEF(ADCchannel CALIB, float NOW, float NOMINAL)
{
    return _pfc_settings.settings.calibrations.calibration[enum_uint(CALIB)] / NOW * NOMINAL;
}

void MainWindow::tableSettingsCalibrationsSetAutoSettings(void)
{
    float autoval=0;

    for(TableCalibrationRows row = static_cast<TableCalibrationRows>(0); row < TableCalibrationRows::table_calibrations_row_count; row++)
    {
        autoval=0;
        switch(row){
            case TableCalibrationRows::table_calibrations_row_offset_U_A:
                autoval=_pfc_settings.adc_raw.ADC_U_A-ADC_UREF;
            break;
            case TableCalibrationRows::table_calibrations_row_offset_U_B:
                autoval=_pfc_settings.adc_raw.ADC_U_B-ADC_UREF;
            break;
            case TableCalibrationRows::table_calibrations_row_offset_U_C:
                autoval=_pfc_settings.adc_raw.ADC_U_C-ADC_UREF;
            break;
            case TableCalibrationRows::table_calibrations_row_offset_I_A:
                autoval=_pfc_settings.adc_raw.ADC_I_A-ADC_UREF;
            break;
            case TableCalibrationRows::table_calibrations_row_offset_I_B:
                autoval=_pfc_settings.adc_raw.ADC_I_B-ADC_UREF;
            break;
            case TableCalibrationRows::table_calibrations_row_offset_I_C:
                autoval=_pfc_settings.adc_raw.ADC_I_C-ADC_UREF;
            break;
            case TableCalibrationRows::table_calibrations_row_offset_U_EMS_A:
            break;
            case TableCalibrationRows::table_calibrations_row_offset_U_EMS_B:
            break;
            case TableCalibrationRows::table_calibrations_row_offset_U_EMS_C:
            break;
            case TableCalibrationRows::table_calibrations_row_offset_U_EMS_I:
            break;
            case TableCalibrationRows::table_calibrations_row_multiplier_U_A:
                autoval = CALC_AUTO_COEF(ADCchannel::ADC_U_A, _pfc_settings.adc.ADC_U_A, PFC_NOMINAL_VOLTAGE);
            break;
            case TableCalibrationRows::table_calibrations_row_multiplier_U_B:
                autoval = CALC_AUTO_COEF(ADCchannel::ADC_U_B, _pfc_settings.adc.ADC_U_B, PFC_NOMINAL_VOLTAGE);
            break;
            case TableCalibrationRows::table_calibrations_row_multiplier_U_C:
                autoval = CALC_AUTO_COEF(ADCchannel::ADC_U_C, _pfc_settings.adc.ADC_U_C, PFC_NOMINAL_VOLTAGE);
            break;
            case TableCalibrationRows::table_calibrations_row_multiplier_I_A:
                autoval = CALC_AUTO_COEF(ADCchannel::ADC_I_A, _pfc_settings.adc.ADC_I_A, PFC_CALIBRATION_CURRENT);
            break;
            case TableCalibrationRows::table_calibrations_row_multiplier_I_B:
                autoval = CALC_AUTO_COEF(ADCchannel::ADC_I_B, _pfc_settings.adc.ADC_I_B, PFC_CALIBRATION_CURRENT);
            break;
            case TableCalibrationRows::table_calibrations_row_multiplier_I_C:
                autoval = CALC_AUTO_COEF(ADCchannel::ADC_I_C, _pfc_settings.adc.ADC_I_C, PFC_CALIBRATION_CURRENT);
            break;
            case TableCalibrationRows::table_calibrations_row_multiplier_I_et:
                autoval = CALC_AUTO_COEF(ADCchannel::ADC_I_ET, _pfc_settings.adc.ADC_I_ET, PFC_CALIBRATION_CURRENT);
            break;
            case TableCalibrationRows::table_calibrations_row_multiplier_U_cap:
            case TableCalibrationRows::table_calibrations_row_multiplier_temperature_1:
            case TableCalibrationRows::table_calibrations_row_multiplier_temperature_2:
                autoval = 1.0f;
            break;
            case TableCalibrationRows::table_calibrations_row_offset_I_et:
            case TableCalibrationRows::table_calibrations_row_offset_U_cap:
            case TableCalibrationRows::table_calibrations_row_offset_temperature_1:
            case TableCalibrationRows::table_calibrations_row_offset_temperature_2:
                autoval = 0.0f;
            break;
            case TableCalibrationRows::table_calibrations_row_multiplier_U_EMS_A:
                autoval = CALC_AUTO_COEF(ADCchannel::ADC_EMS_A, _pfc_settings.adc.ADC_EMS_A, PFC_NOMINAL_VOLTAGE);
            break;
            case TableCalibrationRows::table_calibrations_row_multiplier_U_EMS_B:
                autoval = CALC_AUTO_COEF(ADCchannel::ADC_EMS_B, _pfc_settings.adc.ADC_EMS_B, PFC_NOMINAL_VOLTAGE);
            break;
            case TableCalibrationRows::table_calibrations_row_multiplier_U_EMS_C:
                autoval = CALC_AUTO_COEF(ADCchannel::ADC_EMS_C, _pfc_settings.adc.ADC_EMS_C, PFC_NOMINAL_VOLTAGE);
            break;
            case TableCalibrationRows::table_calibrations_row_multiplier_U_EMS_I:
                autoval = CALC_AUTO_COEF(ADCchannel::ADC_EMS_I, _pfc_settings.adc.ADC_EMS_I, PFC_CALIBRATION_CURRENT);
            break;
            default:
            break;
        }
        _ui->tableSettingsCalibrations->blockSignals(true);
        _ui->tableSettingsCalibrations->item(enum_int(row), enum_int(TableCalibrationColumns::TABLE_CALIBRATIONS_AUTO_VALUE_COLUMN))->setText(QString().sprintf("%.4f", static_cast<double>(autoval)));
        _ui->tableSettingsCalibrations->blockSignals(false);
    }
}

void MainWindow::tableSettingsCalibrationsAutoClicked(bool check)
{
    Q_UNUSED(check)
    QObject* obj = sender();
    int i=0;
    bool found = false;
    for(size_t j=0; j<_btns_edit.size(); j++)
    {
        if(obj == _btns_edit[j])
        {
            i = static_cast<int>(j);
            found = true;
            break;
        }
    }
    if(!found)
    {
        return;
    }
    _ui->tableSettingsCalibrations->selectRow(i);
    float autoval=_ui->tableSettingsCalibrations->item(i, enum_int(TableCalibrationColumns::TABLE_CALIBRATIONS_AUTO_VALUE_COLUMN))->text().toFloat();
    _ui->tableSettingsCalibrations->item(i, enum_int(TableCalibrationColumns::TABLE_CALIBRATIONS_VALUE_COLUMN))->setText(QString().sprintf("%.4f", static_cast<double>(autoval)));
}

void MainWindow::tableSettingsCalibrationsChanged(int row, int col)
{
    QTableWidgetItem *item = _ui->tableSettingsCalibrations->item(row, col);
    float value = item->text().toFloat();
    if (row < static_cast<int>(ADC_CHANNEL_NUMBER))
    {
        _pfc_settings.settings.calibrations.offset[static_cast<uint>(row)] = value;
    }
    else
    {
        _pfc_settings.settings.calibrations.calibration[static_cast<uint>(row - static_cast<int>(ADC_CHANNEL_NUMBER))] = value;
    }
    writeSettingsCalibrations(
        _pfc_settings.settings.calibrations.calibration,
        _pfc_settings.settings.calibrations.offset);
}

void MainWindow::setSettingsCalibrations(
    std::vector<float> calibration,
    std::vector<float> offset)
{
    _ui->tableSettingsCalibrations->blockSignals(true);
    for (int i = 0; i < static_cast<int>(ADC_CHANNEL_NUMBER); i++)
    {
        _ui->tableSettingsCalibrations->item(i + static_cast<int>(ADC_CHANNEL_NUMBER), 0)->setText(QString().sprintf("%.5f", static_cast<double>(calibration[static_cast<uint>(i)])));
        _ui->tableSettingsCalibrations->item(i, 0)->setText(QString().sprintf("%.2f", static_cast<double>(offset[static_cast<uint>(i)])));
    }
    _ui->tableSettingsCalibrations->blockSignals(false);
    _pfc_settings.settings.calibrations.calibration = calibration;
    _pfc_settings.settings.calibrations.offset = offset;
    tableSettingsCalibrationsSetAutoSettings();
}
