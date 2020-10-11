/**
 * @file mainwindow.c
 * @author Stanislav Karpikov
 * @brief Main application window management
 */

/** @addtogroup app_main
 * @{
 */

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
#include "htmldelegate.h"
#include "device_definition.h"

#include <sstream>      /* std::stringstream */
#include <iomanip>      /* std::setfill, std::setw */

/*--------------------------------------------------------------
                       NAMESPACES
--------------------------------------------------------------*/

using namespace PFCconfig;
using namespace PFCconfig::ADC;
using namespace PFCconfig::Interface;
using namespace PFCconfig::Events;

/*--------------------------------------------------------------
                       DEFINES
--------------------------------------------------------------*/

void SETFILTER(QEvent* event, QObject* object, QWidget* UI, QTimer* OBJ, std::chrono::milliseconds TIMER)
{
    if( object == UI ){
        if( event->type() == QEvent::Show ){
            std::chrono::milliseconds timeout = TIMER+
            ((static_cast<std::chrono::milliseconds>(qrand())%(TIMER/4))-(TIMER/4));

            OBJ->start(timeout);
        }
        if( event->type() == QEvent::Hide ){
            OBJ->stop();
        }
    }
}

std::string SCOL(std::string STR, std::string COL)
{
    std::string str;
    str = "<font color=" + COL + ">" + STR + "</font>";
    return str;
}

#define FCOEFF 0.9f
#define FILTERADD(A,B) A=A*FCOEFF+B*(1-FCOEFF)

/*--------------------------------------------------------------
                       CLASS FUNCTIONS
--------------------------------------------------------------*/

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    pfc(new PFC),
    oscillog_xval(),
    harmonics_xval(),
    oscillog_data(static_cast<int>(OscillogChannels::OSCILLOG_SIZE)),
    buttons_edit(),
    last_index_events(0),
    pfc_settings(),
    OSCILLOG_ARR(),
    port_settings(new SettingsDialog)
{
    ui->setupUi(this);

    ui->actionConnect->setEnabled(true);
    ui->actionDisconnect->setEnabled(false);
    ui->actionQuit->setEnabled(true);
    ui->actionConfigure->setEnabled(true);

    initInterfaceConnections();

    QMetaObject::invokeMethod(port_settings, "show", Qt::QueuedConnection);

    connect(pfc->_interface,SIGNAL(connected()),this,SLOT(deviceConnected()));
    connect(pfc->_interface,SIGNAL(disconnected()),this,SLOT(deviceDisconnected()));

    qRegisterMetaType<QSerialPort::DataBits>("QSerialPort::DataBits");
    qRegisterMetaType<QSerialPort::Parity>("QSerialPort::Parity");
    qRegisterMetaType<QSerialPort::StopBits>("QSerialPort::StopBits");
    qRegisterMetaType<QSerialPort::FlowControl>("QSerialPort::FlowControl");
    qRegisterMetaType<std::vector<uint8_t>>("std::vector<uint8_t>");
    qRegisterMetaType<PackageCommand*>("PackageCommand*");

    ui->groupBox_State->installEventFilter(this);
    ui->groupBox_net->installEventFilter(this);
    ui->OscillogPlot->installEventFilter(this);
    ui->pageCalibrations->installEventFilter(this);
    ui->pageCapacitors->installEventFilter(this);
    ui->pageProtection->installEventFilter(this);
    ui->pageFilters->installEventFilter(this);    

    connect(pfc,SIGNAL(setConnection(bool)),this,SLOT(setConnection(bool)));
    connect(pfc,&PFC::setOscillog,this,&MainWindow::setOscillog);
    connect(pfc,&PFC::setNetVoltage,this,&MainWindow::setNetVoltage);
    connect(pfc,&PFC::setSwitchOnOff, this,&MainWindow::setSwitchOnOff);
    connect(pfc,&PFC::setNetVoltageRAW,this,&MainWindow::setNetVoltageRAW);
    connect(pfc,&PFC::setNetParams,this,&MainWindow::setNetParams);
    connect(pfc,&PFC::setVersionInfo,this,&MainWindow::setVersionInfo);
    connect(pfc,&PFC::setWorkState,this,&MainWindow::setWorkState);
    connect(pfc,&PFC::setEvents,this,&MainWindow::setEvents);

    connect(pfc,SIGNAL(Message(quint8,quint8,quint8,std::string)),
            this, SLOT(Message(quint8,quint8,quint8,std::string)));

    connect(pfc,&PFC::setSettingsCalibrations,
            this,&MainWindow::setSettingsCalibrations);
    connect(pfc,&PFC::setSettingsProtection,
            this,&MainWindow::setSettingsProtection);
    connect(pfc,&PFC::setSettingsCapacitors,
            this,&MainWindow::setSettingsCapacitors);
    connect(pfc,&PFC::setSettingsFilters,
            this,&MainWindow::setSettingsFilters);

    connect(pfc,&PFC::ansSettingsCalibrations,
            this,&MainWindow::ansSettingsCalibrations);
    connect(pfc,&PFC::ansSettingsProtection,
            this,&MainWindow::ansSettingsProtection);
    connect(pfc,&PFC::ansSettingsCapacitors,
            this,&MainWindow::ansSettingsCapacitors);   
    connect(pfc,&PFC::ansSettingsFilters,
            this,&MainWindow::ansSettingsFilters);

    connect(this,SIGNAL(updateNetVoltage()),
            pfc,SLOT(updateNetVoltage()));
    connect(this,SIGNAL(updateNetVoltageRAW()),
            pfc,SLOT(updateNetVoltageRAW()));
    connect(this,SIGNAL(updateNetParams()),
            pfc,SLOT(updateNetParams()));
    connect(this,SIGNAL(updateOscillog(PFCOscillogCnannel)),
            pfc,SLOT(updateOscillog(PFCOscillogCnannel)));
    connect(this,&MainWindow::updateWorkState,
            pfc,&PFC::updateWorkState);
    connect(this,SIGNAL(updateVersionInfo()),
            pfc,SLOT(updateVersionInfo()));
    connect(this,&MainWindow::updateEvents,
            pfc,&PFC::updateEvents);

    connect(this,&MainWindow::updateSettingsCalibrations,
            pfc,&PFC::updateSettingsCalibrations);
    connect(this,&MainWindow::updateSettingsProtection,
            pfc,&PFC::updateSettingsProtection);
    connect(this,&MainWindow::updateSettingsCapacitors,
            pfc,&PFC::updateSettingsCapacitors);
    connect(this,&MainWindow::updateSettingsFilters,
            pfc,&PFC::updateSettingsFilters);

    connect(this,&MainWindow::writeSettingsCalibrations,
            pfc,&PFC::writeSettingsCalibrations);
    connect(this,&MainWindow::writeSettingsProtection,
            pfc,&PFC::writeSettingsProtection);
    connect(this,&MainWindow::writeSettingsCapacitors,
            pfc,&PFC::writeSettingsCapacitors);
    connect(this,&MainWindow::writeSettingsFilters,
            pfc,&PFC::writeSettingsFilters);

    connect(this,&MainWindow::writeSwitchOnOff,
            pfc,&PFC::writeSwitchOnOff);

    connect(&timer_Vol,SIGNAL(timeout()),
            this,SLOT(timerupdateNetVoltage()));
    connect(&timer_Raw,SIGNAL(timeout()),
            this,SLOT(timerupdateNetVoltageRaw()));
    connect(&timer_Oscillog,SIGNAL(timeout()),
            this,SLOT(timerOscillog()));
    connect(&timer_SettingsCalibrations,SIGNAL(timeout()),
            this,SLOT(timerSettingsCalibrations()));
    connect(&timer_SettingsCapacitors,SIGNAL(timeout()),
            this,SLOT(timerSettingsCapacitors()));
    connect(&timer_SettingsProtection,SIGNAL(timeout()),
            this,SLOT(timerSettingsProtection()));
    connect(&timer_SettingsFilters,SIGNAL(timeout()),
            this,SLOT(timerSettingsFilters()));
    connect(&timer_MainParams,SIGNAL(timeout()),
            this,SLOT(timerNetParams()));
    connect(&timer_State,SIGNAL(timeout()),
            this,SLOT(timerWorkState()));
    connect(&timer_Version,SIGNAL(timeout()),
            this,SLOT(timerVersion()));
    connect(&timer_events,SIGNAL(timeout()),
            this,SLOT(timerEvents()));

    /* Init window pages */
    pageMainInit();
    pageOscillogInit();
    pageSettingsCalibrationsInit();
    pageSettingsCapacitorsInit();
    pageSettingsProtectionInit();
    pageSettingsFiltersInit();

    timer_events.start(EVENTS_TIMER_TIMEOUT);

    ui->listWidget->setItemDelegate(new HtmlDelegate);
}

void MainWindow::timerupdateNetVoltage()
{
    if(_connected)updateNetVoltage();
}
void MainWindow::timerupdateNetVoltageRaw()
{
    if(_connected)updateNetVoltageRAW();
}
void MainWindow::timerSettingsCapacitors()
{
    if(_connected)updateSettingsCapacitors();
}
void MainWindow::timerSettingsProtection()
{
    if(_connected)updateSettingsProtection();
}
void MainWindow::timerSettingsFilters()
{
    if(_connected)updateSettingsFilters();
}
void MainWindow::timerSettingsCalibrations()
{
    if(_connected)updateSettingsCalibrations();
}

void MainWindow::timerEvents()
{
    if(_connected)updateEvents(last_index_events);
}

void MainWindow::timerWorkState()
{
    if(_connected)updateWorkState(static_cast<uint64_t>(QDateTime::currentMSecsSinceEpoch()));
}
void MainWindow::timerVersion()
{
    updateVersionInfo();
}
void MainWindow::timerNetParams()
{
    if(_connected)updateNetParams();
}

void MainWindow::timerOscillog()
{
    if(!_connected)return;

    if(ui->checkBox_osc_I_a->isChecked())updateOscillog(PFCOscillogCnannel::OSC_I_A);
    if(ui->checkBox_osc_I_b->isChecked())updateOscillog(PFCOscillogCnannel::OSC_I_B);
    if(ui->checkBox_osc_I_c->isChecked())updateOscillog(PFCOscillogCnannel::OSC_I_C);

    if(ui->checkBox_osc_Ua->isChecked())updateOscillog(PFCOscillogCnannel::OSC_U_A);
    if(ui->checkBox_osc_Ub->isChecked())updateOscillog(PFCOscillogCnannel::OSC_U_B);
    if(ui->checkBox_osc_Uc->isChecked())updateOscillog(PFCOscillogCnannel::OSC_U_C);

    if(ui->checkBox_osc_Ud->isChecked())updateOscillog(PFCOscillogCnannel::OSC_UD);

    if(ui->checkBox_osc_Icomp_a->isChecked())updateOscillog(PFCOscillogCnannel::OSC_COMP_A);
    if(ui->checkBox_osc_Icomp_b->isChecked())updateOscillog(PFCOscillogCnannel::OSC_COMP_B);
    if(ui->checkBox_osc_Icomp_c->isChecked())updateOscillog(PFCOscillogCnannel::OSC_COMP_C);

    ui->OscillogPlot->graph(static_cast<int>(OscillogChannels::OSCILLOG_I_A))->setVisible(ui->checkBox_osc_I_a->isChecked());
    ui->OscillogPlot->graph(static_cast<int>(OscillogChannels::OSCILLOG_I_B))->setVisible(ui->checkBox_osc_I_b->isChecked());
    ui->OscillogPlot->graph(static_cast<int>(OscillogChannels::OSCILLOG_I_C))->setVisible(ui->checkBox_osc_I_c->isChecked());

    ui->OscillogPlot->graph(static_cast<int>(OscillogChannels::OSCILLOG_U_A))->setVisible(ui->checkBox_osc_Ua->isChecked());
    ui->OscillogPlot->graph(static_cast<int>(OscillogChannels::OSCILLOG_U_B))->setVisible(ui->checkBox_osc_Ub->isChecked());
    ui->OscillogPlot->graph(static_cast<int>(OscillogChannels::OSCILLOG_U_C))->setVisible(ui->checkBox_osc_Uc->isChecked());

    ui->OscillogPlot->graph(static_cast<int>(OscillogChannels::OSCILLOG_UD))->setVisible(ui->checkBox_osc_Ud->isChecked());

    ui->OscillogPlot->graph(static_cast<int>(OscillogChannels::OSCILLOG_COMP_A))->setVisible(ui->checkBox_osc_Icomp_a->isChecked());
    ui->OscillogPlot->graph(static_cast<int>(OscillogChannels::OSCILLOG_COMP_B))->setVisible(ui->checkBox_osc_Icomp_b->isChecked());
    ui->OscillogPlot->graph(static_cast<int>(OscillogChannels::OSCILLOG_COMP_C))->setVisible(ui->checkBox_osc_Icomp_c->isChecked());
}

bool MainWindow::eventFilter( QObject *object, QEvent *event )
{
    SETFILTER(event, object, ui->groupBox_net, &timer_MainParams, TIMEOUT_UPDATE_MAIN_PARAMS);
    SETFILTER(event, object, ui->groupBox_net, &timer_Vol,TIMEOUT_UPDATE_VOLTAGES);
    SETFILTER(event, object, ui->groupBox_net, &timer_Raw,TIMEOUT_UPDATE_ADC_RAW);
    SETFILTER(event, object, ui->groupBox_net, &timer_State,TIMEOUT_UPDATE_STATE);
    SETFILTER(event, object, ui->groupBox_State, &timer_Version,TIMEOUT_UPDATE_VERSION);
    SETFILTER(event, object, ui->OscillogPlot, &timer_Oscillog,TIMEOUT_UPDATE_OSCILLOG);
    SETFILTER(event, object, ui->pageCalibrations, &timer_SettingsCalibrations,TIMEOUT_UPDATE_SETTINGS_CALIBRATIONS);
    SETFILTER(event, object, ui->pageCapacitors, &timer_SettingsCapacitors,TIMEOUT_UPDATE_SETTINGS_CAPACITORS);
    SETFILTER(event, object, ui->pageProtection, &timer_SettingsProtection,TIMEOUT_UPDATE_SETTINGS_PROTECTION);
    SETFILTER(event, object, ui->pageFilters, &timer_SettingsFilters,TIMEOUT_UPDATE_SETTINGS_FILTERS);
    return QMainWindow::eventFilter( object, event );
}

void MainWindow::deviceConnected()
{
    ui->actionConnect->setEnabled(false);
    ui->actionDisconnect->setEnabled(true);
    ui->actionConfigure->setEnabled(false);
}

void MainWindow::deviceDisconnected()
{
    ui->actionConnect->setEnabled(true);
    ui->actionDisconnect->setEnabled(false);
    ui->actionConfigure->setEnabled(true);
}

void MainWindow::setEvents(std::list<EventRecord> ev)
{
    foreach(EventRecord event, ev){
        if(event.unix_time_s_ms>=last_index_events)
        {
            last_index_events = event.unix_time_s_ms+1;
        }
        if(last_index_events>TIME_MAX_VALUE)
        {
            last_index_events=0;
        }
        char Phases[3]={'A','B','C'};
        std::string ADCchannels[]={
            "ADC_UD",
            "ADC_U_A",
            "ADC_U_B",
            "ADC_U_C",
            "ADC_I_A",
            "ADC_I_B",
            "ADC_I_C",
            "ADC_I_ET",
            "ADC_I_TEMP1",
            "ADC_I_TEMP2",
            "ADC_EMS_A",
            "ADC_EMS_B",
            "ADC_EMS_C",
            "ADC_EMS_I",
        };
        std::stringstream message_stream;
        message_stream << std::fixed << 2;
        uint32_t subtype=(event.type>>16)&0xFFFF;

        QDateTime timestamp;
        timestamp.setTime_t(static_cast<uint>(event.unix_time_s_ms/1000));

        std::stringstream date_stream;
        date_stream << timestamp.toString("dd.MM.yyyy hh:mm:ss").toStdString();
        date_stream << ".";
        date_stream << std::setfill('0') << std::setw(4);
        date_stream << event.unix_time_s_ms % 1000;

        message_stream << SCOL(date_stream.str(), LIGHT_GREY);

        switch(static_cast<event_type_t>(event.type&0xFFFF)){
            case event_type_t::EVENT_TYPE_POWER:
                message_stream << SCOL(" - Питание - ",DARK_GREEN);
                switch(subtype){
                    case SUB_EVENT_TYPE_POWER_ON:
                        message_stream << "Включен";
                    break;
                    default:
                        message_stream << SCOL(" - Неизвестное событие! ", DARK_RED);
                    break;
                }
            break;
            case event_type_t::EVENT_TYPE_CHANGESTATE:
            message_stream << SCOL(" - Состояние - ",DARK_GREEN);
            switch(static_cast<PFCstate>(subtype)){
                case PFCstate::PFC_STATE_INIT:
                    message_stream << "Инициализация";
                break;
                case PFCstate::PFC_STATE_STOP:
                    message_stream << "Остановлен";
                break;
                case PFCstate::PFC_STATE_SYNC:
                    message_stream << "Синхронизация";
                break;
                case PFCstate::PFC_STATE_PRECHARGE_PREPARE:
                    message_stream << "Подг.предзаряда";
                break;
                case PFCstate::PFC_STATE_PRECHARGE:
                    message_stream << "Предзаряд";
                break;                
                case PFCstate::PFC_STATE_MAIN:
                    message_stream << "Контактор";
                break;
                case PFCstate::PFC_STATE_PRECHARGE_DISABLE:
                    message_stream << "Выкл.предзаряда";
                break;
                case PFCstate::PFC_STATE_WORK:
                    message_stream << "Работа";
                break;
                case PFCstate::PFC_STATE_CHARGE:
                    message_stream << "Заряд";
                break;
                case PFCstate::PFC_STATE_TEST:
                    message_stream << "Тест";
                break;
                case PFCstate::PFC_STATE_STOPPING:
                    message_stream << "Остановка..";
                break;
                case PFCstate::PFC_STATE_FAULTBLOCK:
                    message_stream << SCOL(" Авария ", DARK_RED);
                break;
                default:
                    message_stream << "Unknown state";
                break;
            }
            break;
            case event_type_t::EVENT_TYPE_PROTECTION:
            message_stream << SCOL(" - Защиты ",DARK_RED);
            switch(subtype){
                case SUB_EVENT_TYPE_PROTECTION_UCAP_MIN:
                    message_stream << " - Слишком низкое напряжение конденсатора во время работы ";
                    message_stream << "(" << event.info+1 << ") : "<< event.value << " B";
                break;
                case SUB_EVENT_TYPE_PROTECTION_UCAP_MAX:
                    message_stream << " - Превышение напряжения конденсатора ";
                    message_stream << "(" << event.info+1 << ") : "<< event.value << " B";
                break;
                case SUB_EVENT_TYPE_PROTECTION_TEMPERATURE:
                    message_stream << " - Превышение температуры ";
                    message_stream << event.value << " °С";
                break;
                case SUB_EVENT_TYPE_PROTECTION_U_MIN:
                    message_stream << " - Слишком низкое напряжение фазы ";
                    message_stream << Phases[event.info] << ": "<< event.value << " B";
                break;
                case SUB_EVENT_TYPE_PROTECTION_U_MAX:
                    message_stream << " - Превышение напряжения фазы ";
                    message_stream << Phases[event.info] << ": "<< event.value << " B";
                break;
                case SUB_EVENT_TYPE_PROTECTION_F_MIN:
                    message_stream << " - Слишком низкая частота сети ";
                    message_stream << event.value << "Гц";
                break;
                case SUB_EVENT_TYPE_PROTECTION_F_MAX:
                    message_stream << " - Слишком высокая частота сети ";
                    message_stream << event.value << "Гц";
                break;
                case SUB_EVENT_TYPE_PROTECTION_IAFG_MAX_RMS:
                    message_stream << " - Превышение среднеквадратического тока АДФ по фазе ";
                    message_stream << Phases[event.info] << ": "<< event.value << " A";
                break;
                case SUB_EVENT_TYPE_PROTECTION_IAFG_MAX_PEAK:
                    message_stream << " - Превышение пикового тока АДФ по фазе ";
                    message_stream << Phases[event.info] << ": "<< event.value << " A";
                break;
                case SUB_EVENT_TYPE_PROTECTION_PHASES:
                    message_stream << " - Неверное отношение фаз напряжения ";
                break;
                case SUB_EVENT_TYPE_PROTECTION_ADC_OVERLOAD:
                    message_stream << " - Переполнение АЦП на канале ";
                    message_stream << ADCchannels[event.info] << ": "<< event.value;
                break;
                case SUB_EVENT_TYPE_PROTECTION_BAD_SYNC:
                    message_stream << " - Проблемы синхронизации (качающаяся частота?) ";
                break;
                case SUB_EVENT_TYPE_PROTECTION_IGBT:
                    message_stream << " - ошибка ключа IGBT ";
                    message_stream << event.info ;
                break;
                default:
                    message_stream << SCOL(" - Неизвестное событие! ", DARK_RED);
                break;
            }
            break;
            case event_type_t::EVENT_TYPE_EVENT:
                message_stream << " - Событие ";
            break;
        }

        Message(MESSAGE_TYPE_STATE,MESSAGE_NORMAL,MESSAGE_TARGET_ALL,
                message_stream.str());
    }
}

void MainWindow::Message(quint8 type, quint8 level, quint8 target, std::string message)
{
        std::string prefix;
        QColor color=Qt::black;
        switch(type){
            case MESSAGE_TYPE_GENERAL:
                prefix=SCOL("[GENERAL] ",DARK_GREY);
            break;
            case MESSAGE_TYPE_CONNECTION:
                prefix=SCOL("[CONNECTION] ","#999966");
            break;
            case MESSAGE_TYPE_GLOBALFAULT:
                prefix=SCOL("[FAULT] ",DARK_RED);
            break;
            case MESSAGE_TYPE_GLOBALWARNING:
                prefix=SCOL("[WARNING] ",DARK_RED);
            break;
            case MESSAGE_TYPE_STATE:
                prefix=SCOL("[STATE] ",DARK_GREEN);
            break;
            case MESSAGE_TYPE_CAPACITORS:
                prefix=SCOL("[CAPACITORS] ",DARK_GREEN);
            break;
            case MESSAGE_TYPE_NETWORK:
                prefix=SCOL("[NETWORK] ",DARK_GREEN);
            break;
            default:break;
        }
        switch(level){
            case MESSAGE_NORMAL:break;
            case MESSAGE_WARNING:color=Qt::darkYellow;break;
            case MESSAGE_ERROR:color=Qt::darkRed;break;
        }
        prefix.append(message);
        QTextDocument doc;
        doc.setHtml( QString::fromStdString(prefix) );
        QString ss(doc.toPlainText());
        if(target&MESSAGE_TARGET_DEBUG){
            qDebug()<<ss;
        }
        if(target&MESSAGE_TARGET_STATUS){
            ui->statusBar->showMessage(ss,5000);
        }
        if(target&MESSAGE_TARGET_HISTORY){
            std::string s ("<font color=" + LIGHT_GREY + ">");
            s.append(QDateTime::currentDateTime ().toString("dd.MM.yyyy hh:mm:ss:").toStdString());
            s.append("</font>");
            s+=prefix;
            QListWidgetItem *i=new QListWidgetItem(QString::fromStdString(s), ui->listWidget);
            Q_UNUSED(i)
        }
}
MainWindow::~MainWindow(){
    delete port_settings;
    delete ui;
}
void MainWindow::openSerialPort(){
    SettingsDialog::Settings p = port_settings->settings();

    /* Connect to the default port */
    QMetaObject::invokeMethod(pfc->_interface, "ConnectTo",
                             Qt::QueuedConnection,
                            Q_ARG(QString, p.name),
                            Q_ARG(qint32,  p.baudRate),
                            Q_ARG(QSerialPort::DataBits,       p.dataBits),
                            Q_ARG(QSerialPort::Parity,         p.parity),
                            Q_ARG(QSerialPort::StopBits,       p.stopBits),
                            Q_ARG(QSerialPort::FlowControl,    p.flowControl),
                            Q_ARG(bool,     p.localEchoEnabled),
                            Q_ARG(quint32,  1000),
                            Q_ARG(quint32,  3));
}
void MainWindow::closeSerialPort(){
    QMetaObject::invokeMethod(pfc->_interface, "Disconnect");
}
void MainWindow::about(){
    QMessageBox::about(this, tr("Program info"),
                       tr("The <b>Simple Terminal</b> example demonstrates how to "
                          "use the Qt Serial Port module in modern GUI applications "
                          "using Qt, with a menu bar, toolbars, and a status bar."));
}

void MainWindow::initInterfaceConnections()
{
    connect(ui->actionConnect, SIGNAL(triggered()), this, SLOT(openSerialPort()));
    connect(ui->actionDisconnect, SIGNAL(triggered()), this, SLOT(closeSerialPort()));
    connect(ui->actionQuit, SIGNAL(triggered()), this, SLOT(close()));
    connect(ui->actionConfigure, SIGNAL(triggered()), port_settings, SLOT(show()));
    connect(ui->actionAbout, SIGNAL(triggered()), this, SLOT(about()));
    connect(ui->actionAboutQt, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
    connect(port_settings, SIGNAL(Appl()), this, SLOT(openSerialPort()));
}


void MainWindow::setNetVoltage(	float ADC_UD,
                                float ADC_U_A,
                                float ADC_U_B,
                                float ADC_U_C,
                                float ADC_I_A,
                                float ADC_I_B,
                                float ADC_I_C,
                                float ADC_I_ET,
                                float ADC_I_TEMP1,
                                float ADC_I_TEMP2,
                                float ADC_EMS_A,
                                float ADC_EMS_B,
                                float ADC_EMS_C,
                                float ADC_EMS_I,
                                float ADC_MATH_A,
                                float ADC_MATH_B,
                                float ADC_MATH_C){
    Q_UNUSED(ADC_I_ET)
    Q_UNUSED(ADC_EMS_A)
    Q_UNUSED(ADC_EMS_B)
    Q_UNUSED(ADC_EMS_C)
    Q_UNUSED(ADC_EMS_I)
    FILTERADD(pfc_settings.ADC.ADC_U_A,ADC_U_A);
    FILTERADD(pfc_settings.ADC.ADC_U_B,ADC_U_B);
    FILTERADD(pfc_settings.ADC.ADC_U_C,ADC_U_C);

    FILTERADD(pfc_settings.ADC.ADC_I_A,ADC_I_A);
    FILTERADD(pfc_settings.ADC.ADC_I_B,ADC_I_B);
    FILTERADD(pfc_settings.ADC.ADC_I_C,ADC_I_C);

    FILTERADD(pfc_settings.ADC.ADC_MATH_A,ADC_MATH_A);
    FILTERADD(pfc_settings.ADC.ADC_MATH_B,ADC_MATH_B);
    FILTERADD(pfc_settings.ADC.ADC_MATH_C,ADC_MATH_C);

    pfc_settings.ADC.ADC_UD=ADC_UD;

    FILTERADD(pfc_settings.ADC.ADC_I_TEMP1,ADC_I_TEMP1);
    FILTERADD(pfc_settings.ADC.ADC_I_TEMP2,ADC_I_TEMP2);

    ui->label_Ua->setText(QString().sprintf("% 5.0f В",static_cast<double>(pfc_settings.ADC.ADC_MATH_A)));
    ui->label_Ub->setText(QString().sprintf("% 5.0f В",static_cast<double>(pfc_settings.ADC.ADC_MATH_B)));
    ui->label_Uc->setText(QString().sprintf("% 5.0f В",static_cast<double>(pfc_settings.ADC.ADC_MATH_C)));

    ui->label_I_A->setText(QString().sprintf("% 5.1f А",static_cast<double>(pfc_settings.ADC.ADC_I_A)));
    ui->label_I_B->setText(QString().sprintf("% 5.1f А",static_cast<double>(pfc_settings.ADC.ADC_I_B)));
    ui->label_I_C->setText(QString().sprintf("% 5.1f А",static_cast<double>(pfc_settings.ADC.ADC_I_C)));

    ui->label_temperature1->setText(QString().sprintf("% 3.0f °C",static_cast<double>(pfc_settings.ADC.ADC_I_TEMP1)));
    ui->label_temperature2->setText(QString().sprintf("% 3.0f °C",static_cast<double>(pfc_settings.ADC.ADC_I_TEMP2)));
}

void MainWindow::setNetVoltageRAW(	float ADC_UD,
                                    float ADC_U_A,
                                    float ADC_U_B,
                                    float ADC_U_C,
                                    float ADC_I_A,
                                    float ADC_I_B,
                                    float ADC_I_C,
                                    float ADC_I_ET,
                                    float ADC_I_TEMP1,
                                    float ADC_I_TEMP2,
                                    float ADC_EMS_A,
                                    float ADC_EMS_B,
                                    float ADC_EMS_C,
                                    float ADC_EMS_I){
    Q_UNUSED(ADC_I_ET)
    Q_UNUSED(ADC_EMS_A)
    Q_UNUSED(ADC_EMS_B)
    Q_UNUSED(ADC_EMS_C)
    Q_UNUSED(ADC_EMS_I)
    FILTERADD(pfc_settings.ADC_RAW.ADC_U_A,ADC_U_A);
    FILTERADD(pfc_settings.ADC_RAW.ADC_U_B,ADC_U_B);
    FILTERADD(pfc_settings.ADC_RAW.ADC_U_C,ADC_U_C);

    FILTERADD(pfc_settings.ADC_RAW.ADC_I_A,ADC_I_A);
    FILTERADD(pfc_settings.ADC_RAW.ADC_I_B,ADC_I_B);
    FILTERADD(pfc_settings.ADC_RAW.ADC_I_C,ADC_I_C);

    FILTERADD(pfc_settings.ADC_RAW.ADC_UD,ADC_UD);

    FILTERADD(pfc_settings.ADC_RAW.ADC_I_TEMP1,ADC_I_TEMP1);
    FILTERADD(pfc_settings.ADC_RAW.ADC_I_TEMP2,ADC_I_TEMP2);
}

void MainWindow::setNetParams(
                float period_fact,
                float U0Hz_A,
                float U0Hz_B,
                float U0Hz_C,
                float I0Hz_A,
                float I0Hz_B,
                float I0Hz_C,
                float thdu_A,
                float thdu_B,
                float thdu_C,
                float U_phase_A,
                float U_phase_B,
                float U_phase_C){
    pfc_settings.NET_PARAMS.period_fact=period_fact;

    pfc_settings.NET_PARAMS.U0Hz_A=U0Hz_A;
    pfc_settings.NET_PARAMS.U0Hz_B=U0Hz_B;
    pfc_settings.NET_PARAMS.U0Hz_C=U0Hz_C;
    pfc_settings.NET_PARAMS.I0Hz_A=I0Hz_A;
    pfc_settings.NET_PARAMS.I0Hz_B=I0Hz_B;
    pfc_settings.NET_PARAMS.I0Hz_C=I0Hz_C;

    pfc_settings.NET_PARAMS.thdu_A=thdu_A;
    pfc_settings.NET_PARAMS.thdu_B=thdu_B;
    pfc_settings.NET_PARAMS.thdu_C=thdu_C;

    pfc_settings.NET_PARAMS.U_phase_A=U_phase_A*360.0f/3.1416f;
    pfc_settings.NET_PARAMS.U_phase_B=U_phase_B*360.0f/3.1416f;
    pfc_settings.NET_PARAMS.U_phase_C=U_phase_C*360.0f/3.1416f;

    ui->label_U_phase_B ->setText(QString().sprintf("% 5.1f°", static_cast<double>(pfc_settings.NET_PARAMS.U_phase_B)));
    ui->label_U_phase_C ->setText(QString().sprintf("% 5.1f°", static_cast<double>(pfc_settings.NET_PARAMS.U_phase_C)));

    ui->label_thdu_A ->setText(QString().sprintf("% 5.2f°", static_cast<double>(pfc_settings.NET_PARAMS.thdu_A)));
    ui->label_thdu_B ->setText(QString().sprintf("% 5.2f°", static_cast<double>(pfc_settings.NET_PARAMS.thdu_B)));
    ui->label_thdu_C ->setText(QString().sprintf("% 5.2f°", static_cast<double>(pfc_settings.NET_PARAMS.thdu_C)));

    ui->label_freq->setText(QString().sprintf("% 6.3f Гц",static_cast<double>(1.0f/(pfc_settings.NET_PARAMS.period_fact/1000000.0f))));
}

void MainWindow::ansSettingsCalibrations(bool writed)
{
    Q_UNUSED(writed)
}

void MainWindow::ansSettingsProtection(bool writed)
{
    Q_UNUSED(writed)
}

void MainWindow::ansSettingsCapacitors(bool writed)
{
    Q_UNUSED(writed)
}

void MainWindow::ansSettingsFilters(bool writed)
{
    Q_UNUSED(writed)
}


void MainWindow::on_pushButton_STOP_clicked(){
    writeSwitchOnOff(pfc_commands_t::COMMAND_WORK_OFF,0);
}
void MainWindow::on_pushButton_Start_clicked(){
    writeSwitchOnOff(pfc_commands_t::COMMAND_WORK_ON,0);
}
void MainWindow::setSwitchOnOff(uint32_t result){
    Q_UNUSED(result)
}
void MainWindow::on_pushButton_Save_clicked()
{
    writeSwitchOnOff(pfc_commands_t::COMMAND_SETTINGS_SAVE,0);
}
void MainWindow::on_actionClear_triggered(){
    ui->listWidget->clear();
}
void MainWindow::on_pushButton_2_clicked(){
    ui->listWidget->clear();
}
void MainWindow::on_checkBox_channelA_toggled(bool checked)
{
 writeSwitchOnOff(pfc_commands_t::COMMAND_CHANNEL0_DATA,checked);
}

void MainWindow::on_checkBox_channelB_toggled(bool checked)
{
 writeSwitchOnOff(pfc_commands_t::COMMAND_CHANNEL1_DATA,checked);
}

void MainWindow::on_checkBox_channelC_toggled(bool checked)
{
 writeSwitchOnOff(pfc_commands_t::COMMAND_CHANNEL2_DATA,checked);
}

void MainWindow::on_pushButton_CHARGE_ON_clicked()
{
        writeSwitchOnOff(pfc_commands_t::COMMAND_CHARGE_ON,0);
}

void MainWindow::on_pushButton_CHARGE_OFF_clicked()
{
        writeSwitchOnOff(pfc_commands_t::COMMAND_CHARGE_OFF,0);
}

/** @} */
