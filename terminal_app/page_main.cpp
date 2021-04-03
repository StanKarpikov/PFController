
#include "ui_mainwindow.h"
#include "settingsdialog.h"
#include "interface_definitions.h"
#include <QMessageBox>
#include <QMessageBox>
#include <QDateTime>
#include <QGraphicsDropShadowEffect>
#include <QListWidgetItem>
#include "page_main.h"

/*--------------------------------------------------------------
                       NAMESPACES
--------------------------------------------------------------*/

using namespace PFCconfig;
using namespace PFCconfig::ADC;
using namespace PFCconfig::Interface;
using namespace PFCconfig::Events;
using namespace InterfaceDefinitions; /* For GUI text strings, GUI parameters, constants */

/*--------------------------------------------------------------
                       PRIVATE FUNCTIONS
--------------------------------------------------------------*/

PageMain::PageMain(Ui::MainWindow *ui, PFCconfig::PFCsettings *pfc_settings, PFC *pfc):
    _ui(ui), _pfc_settings(pfc_settings), _pfc(pfc)
{
}

PageMain::~PageMain(void)
{

}

void PageMain::updateCheckboxVal(QCheckBox* checkbox, bool value)
{
    checkbox->blockSignals(true);
    checkbox->setChecked(value);
    checkbox->blockSignals(false);
}

void PageMain::pageMainInit(void)
{
    connect(_pfc, &PFC::setVersionInfo, this, &PageMain::setVersionInfo);
    connect(_pfc, &PFC::setWorkState, this, &PageMain::setWorkState);
    connect(this, &PageMain::updateWorkState,
         _pfc, &PFC::updateWorkState);
    connect(this, &PageMain::updateVersionInfo,
         _pfc, &PFC::updateVersionInfo);
    _ui->radioConnection->setDisabled(true);
}

void PageMain::update(void)
{
    _ui->radioConnection->setDisabled(true);
    emit updateWorkState(static_cast<uint64_t>(QDateTime::currentMSecsSinceEpoch()));
    emit updateVersionInfo();
}

void PageMain::setConnection(bool connected)
{
    _ui->radioConnection->setChecked(connected);
    _ui->groupState->setDisabled(!connected);
    _ui->groupNetworkParameters->setDisabled(!connected);
    _ui->tabWidget->setDisabled(!connected);

    if (connected)
    {
        _ui->radioConnection->setText("Connected");
    }
    else
    {
        _ui->radioConnection->setText("Disconnected");
    }
}

void PageMain::setWorkState(uint32_t state, uint32_t ch_a, uint32_t ch_b, uint32_t ch_c)
{
    _pfc_settings->status = state;
    _pfc_settings->active_channels[PFC_ACHAN] = ch_a;
    _pfc_settings->active_channels[PFC_BCHAN] = ch_b;
    _pfc_settings->active_channels[PFC_CCHAN] = ch_c;

    if (static_cast<PFCstate>(_pfc_settings->status) != PFCstate::PFC_STATE_STOP)
    {
        _ui->buttonSave->setDisabled(true);
    }
    else
    {
        _ui->buttonSave->setDisabled(false);
    }

    updateCheckboxVal(_ui->checkBoxChannelA, ch_a);
    updateCheckboxVal(_ui->checkBoxChannelB, ch_b);
    updateCheckboxVal(_ui->checkBoxChannelC, ch_c);

    switch (static_cast<PFCstate>(_pfc_settings->status))
    {
        case PFCstate::PFC_STATE_INIT:
            _ui->valueWorkState->setText(QString::fromStdString(STRING_PFC_STATE_INIT));
            break;
        case PFCstate::PFC_STATE_STOP:
            _ui->valueWorkState->setText(QString::fromStdString(STRING_PFC_STATE_STOP));
            break;
        case PFCstate::PFC_STATE_SYNC:
            _ui->valueWorkState->setText(QString::fromStdString(STRING_PFC_STATE_SYNC));
            break;
        case PFCstate::PFC_STATE_PRECHARGE_PREPARE:
            _ui->valueWorkState->setText(QString::fromStdString(STRING_PFC_STATE_PRECHARGE_PREPARE));
            break;
        case PFCstate::PFC_STATE_PRECHARGE:
            _ui->valueWorkState->setText(QString::fromStdString(STRING_PFC_STATE_PRECHARGE));
            break;
        case PFCstate::PFC_STATE_MAIN:
            _ui->valueWorkState->setText(QString::fromStdString(STRING_PFC_STATE_MAIN));
            break;
        case PFCstate::PFC_STATE_PRECHARGE_DISABLE:
            _ui->valueWorkState->setText(QString::fromStdString(STRING_PFC_STATE_PRECHARGE_DISABLE));
            break;
        case PFCstate::PFC_STATE_WORK:
            _ui->valueWorkState->setText(QString::fromStdString(STRING_PFC_STATE_WORK));
            break;
        case PFCstate::PFC_STATE_CHARGE:
            _ui->valueWorkState->setText(QString::fromStdString(STRING_PFC_STATE_CHARGE));
            break;
        case PFCstate::PFC_STATE_TEST:
            _ui->valueWorkState->setText(QString::fromStdString(STRING_PFC_STATE_TEST));
            break;
        case PFCstate::PFC_STATE_STOPPING:
            _ui->valueWorkState->setText(QString::fromStdString(STRING_PFC_STATE_STOPPING));
            break;
        case PFCstate::PFC_STATE_FAULTBLOCK:
            _ui->valueWorkState->setText(QString::fromStdString(STRING_PFC_STATE_FAULTBLOCK));
            break;
        default:
            _ui->valueWorkState->setText(QString::fromStdString(STRING_PFC_STATE_UNKNOWN));
            break;
    }
}

void PageMain::setVersionInfo(
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
    _ui->valueVersion->setText(QString().sprintf("PFC: %d.%d.%d.%d \n Build %02d.%02d.%04d %02d:%02d:%02d",
                                             major, minor, micro, build,
                                             day, month, year, hour, minute, second));
}
