#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "settingsdialog.h"
#include <QMessageBox>
#include <QMessageBox>
#include <QDateTime>
#include <QGraphicsDropShadowEffect>
#include <QListWidgetItem>

void MainWindow::pageSettingsCalibrationsInit(){
    //------------------------------------------------------
    for(int row=0;row<ui->tableWidget_settings_calibrations->rowCount();row++){
        ui->tableWidget_settings_calibrations->setItem(row,0,new QTableWidgetItem());
        ui->tableWidget_settings_calibrations->item(row, 0)->setFlags(Qt::ItemIsEnabled | Qt::ItemIsEditable);
        ui->tableWidget_settings_calibrations->item(row, 0)->setBackground(editableCellBrush);
        ui->tableWidget_settings_calibrations->setItem(row,1,new QTableWidgetItem());
        ui->tableWidget_settings_calibrations->setItem(row,2,new QTableWidgetItem());
        Qt::ItemFlags eFlags;
        QWidget* pWidget;
        QHBoxLayout* pLayout;
        QPushButton *btn_edit;
        btn_edit=new QPushButton();
        buttons_edit.append(btn_edit);
        //Ячейка для авто значения
          eFlags = ui->tableWidget_settings_calibrations->item(row, 1)->flags();
          eFlags &= ~Qt::ItemIsEditable;
          ui->tableWidget_settings_calibrations->item(row, 1)->setFlags(eFlags);
        //
        /*if(row<ADC_CHANNEL_NUMBER){
              //Ячейка с кнопкой авто
                pWidget = new QWidget();
                btn_edit->setText("Авто");
                pLayout = new QHBoxLayout(pWidget);
                pLayout->addWidget(btn_edit);
                pLayout->setAlignment(Qt::AlignCenter);
                pLayout->setContentsMargins(0, 0, 0, 0);
                pWidget->setLayout(pLayout);
                ui->tableWidget_settings_calibrations->setCellWidget(row, 2, pWidget);
                connect(btn_edit,SIGNAL(clicked(bool)),
                        this,SLOT(tableSettingsCalibrations_auto_clicked(bool)));
        }*/
    }
    connect(ui->tableWidget_settings_calibrations, SIGNAL(cellChanged(int,int)),
            this, SLOT(tableSettingsCalibrations_changed(int,int)));
    pfc_settings.SETTINGS.CALIBRATIONS.calibration.resize(ADC_CHANNEL_NUMBER);
    pfc_settings.SETTINGS.CALIBRATIONS.offset.resize(ADC_CHANNEL_NUMBER);
}
/*void MainWindow::tableSettingsCalibrations_SetAutoSettings(){
    float autoval=0;

    #define CALC_AUTO_COEF(CALIB,NOW,NOMINAL) \
        KKM_var.SETTINGS.CALIBRATIONS.calibration[CALIB]/KKM_var.ADC.NOW*NOMINAL;

    for(int row=0;row<ui->tableWidget_settings_calibrations->rowCount();row++){
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
                autoval=KKM_var.ADC_RAW.ADC_U_A_RAW-KKM_var.ADC.ADC_UREF;
            break;
            case table_settings_calib_row_offset_Ub:
                autoval=KKM_var.ADC_RAW.ADC_U_B_RAW-KKM_var.ADC.ADC_UREF;
            break;
            case table_settings_calib_row_offset_Uc:
                autoval=KKM_var.ADC_RAW.ADC_U_C_RAW-KKM_var.ADC.ADC_UREF;
            break;
            case table_settings_calib_row_coef_ILa:
            case table_settings_calib_row_coef_ILb:
            case table_settings_calib_row_coef_ILc:
            break;
            case table_settings_calib_row_offset_ILa:
                autoval=KKM_var.ADC_RAW.ADC_IL_A_RAW-KKM_var.ADC.ADC_UREF;
            break;
            case table_settings_calib_row_offset_ILb:
                autoval=KKM_var.ADC_RAW.ADC_IL_B_RAW-KKM_var.ADC.ADC_UREF;
            break;
            case table_settings_calib_row_offset_ILc:
                autoval=KKM_var.ADC_RAW.ADC_IL_C_RAW-KKM_var.ADC.ADC_UREF;
            break;
            case table_settings_calib_row_coef_IAFGa:
            case table_settings_calib_row_coef_IAFGb:
            case table_settings_calib_row_coef_IAFGc:
            break;
            case table_settings_calib_row_offset_IAFGa:
                autoval=KKM_var.ADC_RAW.ADC_IAFG_A_RAW-KKM_var.ADC.ADC_UREF;
            break;
            case table_settings_calib_row_offset_IAFGb:
                autoval=KKM_var.ADC_RAW.ADC_IAFG_B_RAW-KKM_var.ADC.ADC_UREF;
            break;
            case table_settings_calib_row_offset_IAFGc:
                autoval=KKM_var.ADC_RAW.ADC_IAFG_C_RAW-KKM_var.ADC.ADC_UREF;
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
                autoval=KKM_var.ADC.ADC_UREF-2048;
            break;
        }
        ui->tableWidget_settings_calibrations->blockSignals(true);
        ui->tableWidget_settings_calibrations->item(row,1)->setText(QString().sprintf("%.4f",autoval));
        ui->tableWidget_settings_calibrations->blockSignals(false);
    }
}

void MainWindow::tableSettingsCalibrations_auto_clicked(bool check){
    QObject* obj = sender();
    int i=-1;
    for(int j=0;j<btns_edit.size();j++){
        if(obj==btns_edit[j]){
            i=j;
            break;
        }
    }
    if(i<0)return;
    //переносим из ячейки "авто" в основную
    ui->tableWidget_settings_calibrations->selectRow(i);
    float autoval=ui->tableWidget_settings_calibrations->item(i,1)->text().toFloat();
    ui->tableWidget_settings_calibrations->item(i,0)->setText(QString().sprintf("%.4f",autoval));
}*/

void MainWindow::tableSettingsCalibrations_changed(int row, int col){
    QTableWidgetItem *item=ui->tableWidget_settings_calibrations->item(row,col);
    float val=item->text().toFloat();
    if(row<ADC_CHANNEL_NUMBER){
       pfc_settings.SETTINGS.CALIBRATIONS.offset[row]=val;
    }else{
       pfc_settings.SETTINGS.CALIBRATIONS.calibration[row-ADC_CHANNEL_NUMBER]=val;
    }
    writeSettingsCalibrations(
                pfc_settings.SETTINGS.CALIBRATIONS.calibration,
                pfc_settings.SETTINGS.CALIBRATIONS.offset);
}
//========================================================================
void MainWindow::setSettingsCalibrations(
        QVector<float> calibration, //!< Калибровки для каналов
        QVector<float> offset //!< Смещения для каналов
        ){
    ui->tableWidget_settings_calibrations->blockSignals(true);
    for(int i=0;i<ADC_CHANNEL_NUMBER;i++){
       ui->tableWidget_settings_calibrations->item(i+ADC_CHANNEL_NUMBER,0)->setText(QString().sprintf("%.5f",calibration[i]));
       ui->tableWidget_settings_calibrations->item(i,0)->setText(QString().sprintf("%.2f",offset[i]));
    }
    ui->tableWidget_settings_calibrations->blockSignals(false);
    pfc_settings.SETTINGS.CALIBRATIONS.calibration=calibration;
    pfc_settings.SETTINGS.CALIBRATIONS.offset=offset;
    //tableSettingsCalibrations_SetAutoSettings();
}
