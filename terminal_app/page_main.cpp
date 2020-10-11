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

void MainWindow::UPDATE_CHECKBOX(QCheckBox* CHECK,bool VAL)
{
    CHECK->blockSignals(true);
    CHECK->setChecked(VAL);
    CHECK->blockSignals(false);
}

void MainWindow::pageMainInit(){
    ui->radioButton_Connection->setDisabled(true);    
}
void MainWindow::setConnection(bool connected){
    ui->radioButton_Connection->setChecked(connected);
    ui->groupBox_State->setDisabled(!connected);
    ui->groupBox_net->setDisabled(!connected);
    ui->tabWidget->setDisabled(!connected);

    _connected = connected;
    if(connected){
        ui->radioButton_Connection->setText("Есть связь");
    }else{
        last_index_events=0;
        ui->radioButton_Connection->setText("Нет связи");
    }
}
void MainWindow::setWorkState(uint32_t state,uint32_t chA,uint32_t chB,uint32_t chC){
    pfc_settings.status=state;
    pfc_settings.activeChannels[PFC_ACHAN]=chA;
    pfc_settings.activeChannels[PFC_BCHAN]=chB;
    pfc_settings.activeChannels[PFC_CCHAN]=chC;

    if(static_cast<PFCstate>(pfc_settings.status) != PFCstate::PFC_STATE_STOP)
    {
        ui->pushButton_Save->setDisabled(true);
    }else{
        ui->pushButton_Save->setDisabled(false);
    }

    UPDATE_CHECKBOX(ui->checkBox_channelA,chA);
    UPDATE_CHECKBOX(ui->checkBox_channelB,chB);
    UPDATE_CHECKBOX(ui->checkBox_channelC,chC);

    switch(static_cast<PFCstate>(pfc_settings.status)){
        case PFCstate::PFC_STATE_INIT: //предзаряд
            ui->label_WorkState->setText("Инициализация");
        break;
        case PFCstate::PFC_STATE_STOP: //не работает
            ui->label_WorkState->setText("Остановлен");
        break;
        case PFCstate::PFC_STATE_SYNC: //синхронизация с сетью
            ui->label_WorkState->setText("Синхронизация");
        break;
        case PFCstate::PFC_STATE_PRECHARGE_PREPARE:
            ui->label_WorkState->setText("Подг.предзаряда");
        break;
        case PFCstate::PFC_STATE_PRECHARGE: //предзаряд
            ui->label_WorkState->setText("Предзаряд");
        break;
        case PFCstate::PFC_STATE_MAIN:
            ui->label_WorkState->setText("Контактор");
        break;
        case PFCstate::PFC_STATE_PRECHARGE_DISABLE: //работает, но без компенсации
            ui->label_WorkState->setText("Выкл.предзаряд");
        break;
        case PFCstate::PFC_STATE_WORK: //работает, но без компенсации
             ui->label_WorkState->setText("Работа");
        break;
        case PFCstate::PFC_STATE_CHARGE: //работает, но без компенсации
             ui->label_WorkState->setText("Заряд");
        break;
        case PFCstate::PFC_STATE_TEST: //тестирование сети
            ui->label_WorkState->setText("Тест");
        break;
        case PFCstate::PFC_STATE_STOPPING: //ошибка
            ui->label_WorkState->setText("Остановка..");
        break;
        case PFCstate::PFC_STATE_FAULTBLOCK: //ошибка
            ui->label_WorkState->setText("Авария");
        break;
        default:
            ui->label_WorkState->setText("Unknown");
        break;
    }
}
void MainWindow::setVersionInfo(
        uint32_t major,
        uint32_t minor,
        uint32_t micro,
        uint32_t build,
        uint32_t day,
        uint32_t month,
        uint32_t year,
        uint32_t hour,
        uint32_t minute,
        uint32_t second){

    ui->label_ver->setText(QString().sprintf("ККМ: %d.%d.%d.%d \n Build %02d.%02d.%04d %02d:%02d:%02d",
                                             major,minor,micro,build,
                                             day,month,year,hour,minute,second));
}
