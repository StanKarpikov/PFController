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

void MainWindow::updateCheckboxVal(QCheckBox* checkbox, bool value)
{
    checkbox->blockSignals(true);
    checkbox->setChecked(value);
    checkbox->blockSignals(false);
}

void MainWindow::pageMainInit()
{
    _ui->radioButton_Connection->setDisabled(true);
}
void MainWindow::setConnection(bool connected)
{
    _ui->radioButton_Connection->setChecked(connected);
    _ui->groupBox_State->setDisabled(!connected);
    _ui->groupBox_net->setDisabled(!connected);
    _ui->tabWidget->setDisabled(!connected);

    _connected = connected;
    if (connected)
    {
        _ui->radioButton_Connection->setText("Есть связь");
    }
    else
    {
        _last_index_events = 0;
        _ui->radioButton_Connection->setText("Нет связи");
    }
}
void MainWindow::setWorkState(uint32_t state, uint32_t ch_a, uint32_t ch_b, uint32_t ch_c)
{
    _pfc_settings.status = state;
    _pfc_settings.active_channels[PFC_ACHAN] = ch_a;
    _pfc_settings.active_channels[PFC_BCHAN] = ch_b;
    _pfc_settings.active_channels[PFC_CCHAN] = ch_c;

    if (static_cast<PFCstate>(_pfc_settings.status) != PFCstate::PFC_STATE_STOP)
    {
        _ui->pushButton_Save->setDisabled(true);
    }
    else
    {
        _ui->pushButton_Save->setDisabled(false);
    }

    updateCheckboxVal(_ui->checkBox_channelA, ch_a);
    updateCheckboxVal(_ui->checkBox_channelB, ch_b);
    updateCheckboxVal(_ui->checkBox_channelC, ch_c);

    switch (static_cast<PFCstate>(_pfc_settings.status))
    {
        case PFCstate::PFC_STATE_INIT:  //предзаряд
            _ui->label_WorkState->setText("Инициализация");
            break;
        case PFCstate::PFC_STATE_STOP:  //не работает
            _ui->label_WorkState->setText("Остановлен");
            break;
        case PFCstate::PFC_STATE_SYNC:  //синхронизация с сетью
            _ui->label_WorkState->setText("Синхронизация");
            break;
        case PFCstate::PFC_STATE_PRECHARGE_PREPARE:
            _ui->label_WorkState->setText("Подг.предзаряда");
            break;
        case PFCstate::PFC_STATE_PRECHARGE:  //предзаряд
            _ui->label_WorkState->setText("Предзаряд");
            break;
        case PFCstate::PFC_STATE_MAIN:
            _ui->label_WorkState->setText("Контактор");
            break;
        case PFCstate::PFC_STATE_PRECHARGE_DISABLE:  //работает, но без компенсации
            _ui->label_WorkState->setText("Выкл.предзаряд");
            break;
        case PFCstate::PFC_STATE_WORK:  //работает, но без компенсации
            _ui->label_WorkState->setText("Работа");
            break;
        case PFCstate::PFC_STATE_CHARGE:  //работает, но без компенсации
            _ui->label_WorkState->setText("Заряд");
            break;
        case PFCstate::PFC_STATE_TEST:  //тестирование сети
            _ui->label_WorkState->setText("Тест");
            break;
        case PFCstate::PFC_STATE_STOPPING:  //ошибка
            _ui->label_WorkState->setText("Остановка..");
            break;
        case PFCstate::PFC_STATE_FAULTBLOCK:  //ошибка
            _ui->label_WorkState->setText("Авария");
            break;
        default:
            _ui->label_WorkState->setText("Unknown");
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
    uint32_t second)
{
    _ui->label_ver->setText(QString().sprintf("ККМ: %d.%d.%d.%d \n Build %02d.%02d.%04d %02d:%02d:%02d",
                                             major, minor, micro, build,
                                             day, month, year, hour, minute, second));
}
