#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "settingsdialog.h"
#include <QMessageBox>
#include <QMessageBox>
#include <QDateTime>
#include <QGraphicsDropShadowEffect>
#include <QListWidgetItem>

void MainWindow::pageMainInit(){
    ui->radioButton_Connection->setDisabled(true);    
}
void MainWindow::setConnection(bool connected){
    ui->radioButton_Connection->setChecked(connected);
    ui->groupBox_State->setDisabled(!connected);
    ui->groupBox_net->setDisabled(!connected);
    ui->tabWidget->setDisabled(!connected);

    connected=connected;
    if(connected){        
        ui->radioButton_Connection->setText("Есть связь");
    }else{
        last_index_events=0;
        ui->radioButton_Connection->setText("Нет связи");
    }
}
void MainWindow::setWorkState(uint32_t state,uint32_t chA,uint32_t chB,uint32_t chC){
    pfc_settings.status=state;
    pfc_settings.activeChannels[0]=chA;
    pfc_settings.activeChannels[1]=chB;
    pfc_settings.activeChannels[2]=chC;

    if(pfc_settings.status!=PFC_STATE_STOP){
        ui->pushButton_Save->setDisabled(true);
    }else{
        ui->pushButton_Save->setDisabled(false);
    }

#define UPDATE_CHECKBOX(CHECK,VAL) \
        CHECK->blockSignals(true);\
        CHECK->setChecked(VAL);\
        CHECK->blockSignals(false);

    UPDATE_CHECKBOX(ui->checkBox_channelA,chA);
    UPDATE_CHECKBOX(ui->checkBox_channelB,chB);
    UPDATE_CHECKBOX(ui->checkBox_channelC,chC);

    switch(pfc_settings.status){
        case PFC_STATE_INIT: //предзаряд
            ui->label_WorkState->setText("Инициализация");
        break;
        case PFC_STATE_STOP: //не работает
            ui->label_WorkState->setText("Остановлен");
        break;
        case PFC_STATE_SYNC: //синхронизация с сетью
            ui->label_WorkState->setText("Синхронизация");
        break;
        case PFC_STATE_PRECHARGE_PREPARE:
            ui->label_WorkState->setText("Подг.предзаряда");
        break;
        case PFC_STATE_PRECHARGE: //предзаряд
            ui->label_WorkState->setText("Предзаряд");
        break;
        case PFC_STATE_MAIN:
            ui->label_WorkState->setText("Контактор");
        break;
        case PFC_STATE_PRECHARGE_DISABLE: //работает, но без компенсации
            ui->label_WorkState->setText("Выкл.предзаряд");
        break;
        case PFC_STATE_WORK: //работает, но без компенсации
             ui->label_WorkState->setText("Работа");
        break;
        case PFC_STATE_CHARGE: //работает, но без компенсации
             ui->label_WorkState->setText("Заряд");
        break;
        case PFC_STATE_TEST: //тестирование сети
            ui->label_WorkState->setText("Тест");
        break;
        case PFC_STATE_STOPPING: //ошибка
            ui->label_WorkState->setText("Остановка..");
        break;
        case PFC_STATE_FAULTBLOCK: //ошибка
            ui->label_WorkState->setText("Авария");
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
