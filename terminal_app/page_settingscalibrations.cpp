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
        //Ячейка для авто значения
        eFlags = _ui->tableSettingsCalibrations->item(row, 1)->flags();
        eFlags &= ~Qt::ItemIsEditable;
        _ui->tableSettingsCalibrations->item(row, 1)->setFlags(eFlags);
        /*
         * TODO: Auto calibration button implementation
         *
            QWidget* pWidget;
            QHBoxLayout* pLayout;
            if(row<ADC_CHANNEL_NUMBER){
                pWidget = new QWidget();
                btn_edit->setText("Авто");
                pLayout = new QHBoxLayout(pWidget);
                pLayout->addWidget(btn_edit);
                pLayout->setAlignment(Qt::AlignCenter);
                pLayout->setContentsMargins(0, 0, 0, 0);
                pWidget->setLayout(pLayout);
                ui->tableSettingsCalibrations->setCellWidget(row, 2, pWidget);
                connect(btn_edit, &QPushButton::clickeded,
                        this, &MainWindow::tableSettingsCalibrationsAutoClicked);
        }
        */
    }
    connect(_ui->tableSettingsCalibrations, &QTableWidget::cellChanged,
            this, &MainWindow::tableSettingsCalibrationsChanged);
    _pfc_settings.settings.calibrations.calibration.resize(ADC_CHANNEL_NUMBER);
    _pfc_settings.settings.calibrations.offset.resize(ADC_CHANNEL_NUMBER);
}

/*
 * TODO: Auto calibration button implementation
 *
 * void MainWindow::tableSettingsCalibrationsSetAutoSettings(void)
{
    float autoval=0;

    #define CALC_AUTO_COEF(CALIB,NOW,NOMINAL) \
        PFC_var.SETTINGS.CALIBRATIONS.calibration[CALIB]/PFC_var.ADC.NOW*NOMINAL;

    for(int row=0;row<ui->tableSettingsCalibrations->rowCount();row++){
        autoval=0;
        switch(row){
            case table_settings_calib_row_coef_Ua:
                autoval=CALC_AUTO_COEF(CALIB_U_A,ADC_U_A,NOMINAL_VOLTAGE);
            break;
            case table_settings_calib_row_coef_Ub:
                autoval=CALC_AUTO_COEF(CALIB_U_B,ADC_U_B,NOMINAL_VOLTAGE);
            break;
            case table_settings_calib_row_coef_Uc:
                autoval=CALC_AUTO_COEF(CALIB_U_C,ADC_U_C,NOMINAL_VOLTAGE);
            break;
            case table_settings_calib_row_offset_Ua:
                autoval=PFC_var.ADC_RAW.ADC_U_A_RAW-PFC_var.ADC.ADC_UREF;
            break;
            case table_settings_calib_row_offset_Ub:
                autoval=PFC_var.ADC_RAW.ADC_U_B_RAW-PFC_var.ADC.ADC_UREF;
            break;
            case table_settings_calib_row_offset_Uc:
                autoval=PFC_var.ADC_RAW.ADC_U_C_RAW-PFC_var.ADC.ADC_UREF;
            break;
            case table_settings_calib_row_coef_ILa:
            case table_settings_calib_row_coef_ILb:
            case table_settings_calib_row_coef_ILc:
            break;
            case table_settings_calib_row_offset_ILa:
                autoval=PFC_var.ADC_RAW.ADC_IL_A_RAW-PFC_var.ADC.ADC_UREF;
            break;
            case table_settings_calib_row_offset_ILb:
                autoval=PFC_var.ADC_RAW.ADC_IL_B_RAW-PFC_var.ADC.ADC_UREF;
            break;
            case table_settings_calib_row_offset_ILc:
                autoval=PFC_var.ADC_RAW.ADC_IL_C_RAW-PFC_var.ADC.ADC_UREF;
            break;
            case table_settings_calib_row_coef_IAFGa:
            case table_settings_calib_row_coef_IAFGb:
            case table_settings_calib_row_coef_IAFGc:
            break;
            case table_settings_calib_row_offset_IAFGa:
                autoval=PFC_var.ADC_RAW.ADC_IAFG_A_RAW-PFC_var.ADC.ADC_UREF;
            break;
            case table_settings_calib_row_offset_IAFGb:
                autoval=PFC_var.ADC_RAW.ADC_IAFG_B_RAW-PFC_var.ADC.ADC_UREF;
            break;
            case table_settings_calib_row_offset_IAFGc:
                autoval=PFC_var.ADC_RAW.ADC_IAFG_C_RAW-PFC_var.ADC.ADC_UREF;
            break;
            case table_settings_calib_row_coef_Ud1:
            case table_settings_calib_row_coef_Ud2:
            case table_settings_calib_row_offset_Ud1:
            case table_settings_calib_row_offset_Ud2:
            case table_settings_calib_row_coef_temp:
            case table_settings_calib_row_offset_temp:
            break;
            case table_settings_calib_row_coef_Uref:
                autoval=1;
            break;
            case table_settings_calib_row_offset_Uref:
                autoval=PFC_var.ADC.ADC_UREF-2048;
            break;
        }
        ui->tableSettingsCalibrations->blockSignals(true);
        ui->tableSettingsCalibrations->item(row,1)->setText(QString().sprintf("%.4f",autoval));
        ui->tableSettingsCalibrations->blockSignals(false);
    }
}

void MainWindow::tableSettingsCalibrationsAutoClicked(bool check)
{
    QObject* obj = sender();
    int i=-1;
    for(int j=0;j<btns_edit.size();j++){
        if(obj==btns_edit[j]){
            i=j;
            break;
        }
    }
    if(i<0)return;
    ui->tableSettingsCalibrations->selectRow(i);
    float autoval=ui->tableSettingsCalibrations->item(i,1)->text().toFloat();
    ui->tableSettingsCalibrations->item(i,0)->setText(QString().sprintf("%.4f",autoval));
}*/

void MainWindow::tableSettingsCalibrationsChanged(int row, int col)
{
    QTableWidgetItem *item = _ui->tableSettingsCalibrations->item(row, col);
    float value = item->text().toFloat();
    if (row < ADC_CHANNEL_NUMBER)
    {
        _pfc_settings.settings.calibrations.offset[static_cast<uint>(row)] = value;
    }
    else
    {
        _pfc_settings.settings.calibrations.calibration[static_cast<uint>(row - ADC_CHANNEL_NUMBER)] = value;
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
    for (int i = 0; i < ADC_CHANNEL_NUMBER; i++)
    {
        _ui->tableSettingsCalibrations->item(i + ADC_CHANNEL_NUMBER, 0)->setText(QString().sprintf("%.5f", static_cast<double>(calibration[static_cast<uint>(i)])));
        _ui->tableSettingsCalibrations->item(i, 0)->setText(QString().sprintf("%.2f", static_cast<double>(offset[static_cast<uint>(i)])));
    }
    _ui->tableSettingsCalibrations->blockSignals(false);
    _pfc_settings.settings.calibrations.calibration = calibration;
    _pfc_settings.settings.calibrations.offset = offset;
    //tableSettingsCalibrations_SetAutoSettings();
}
