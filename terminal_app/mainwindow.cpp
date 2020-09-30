/****************************************************************************
**
** Copyright (C) 2012 Denis Shienkov <denis.shienkov@gmail.com>
** Copyright (C) 2012 Laszlo Papp <lpapp@kde.org>
** Contact: http://www.qt.io/licensing/
**
** This file is part of the QtSerialPort module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL21$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 or version 3 as published by the Free
** Software Foundation and appearing in the file LICENSE.LGPLv21 and
** LICENSE.LGPLv3 included in the packaging of this file. Please review the
** following information to ensure the GNU Lesser General Public License
** requirements will be met: https://www.gnu.org/licenses/lgpl.html and
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** As a special exception, The Qt Company gives you certain additional
** rights. These rights are described in The Qt Company LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "settingsdialog.h"
#include <QMessageBox>
#include <QMessageBox>
#include <QDateTime>
#include <QGraphicsDropShadowEffect>
#include <QListWidgetItem>
#include "htmldelegate.h"

MainWindow *w;
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    settings = new SettingsDialog;

    ui->actionConnect->setEnabled(true);
    ui->actionDisconnect->setEnabled(false);
    ui->actionQuit->setEnabled(true);
    ui->actionConfigure->setEnabled(true);

    initActionsConnections();

    QMetaObject::invokeMethod(settings, "show",
                              Qt::QueuedConnection);
    //----------------------------------
    kkm = new KKM();
    //----------------------------------
    connect(kkm->_interface,SIGNAL(Connected()),this,SLOT(KKMconnected()));
    connect(kkm->_interface,SIGNAL(Disconnected()),this,SLOT(KKMdisconnected()));

    qRegisterMetaType<QSerialPort::DataBits>("QSerialPort::DataBits");
    qRegisterMetaType<QSerialPort::Parity>("QSerialPort::Parity");
    qRegisterMetaType<QSerialPort::StopBits>("QSerialPort::StopBits");
    qRegisterMetaType<QSerialPort::FlowControl>("QSerialPort::FlowControl");
    qRegisterMetaType<std::vector<uint8_t>>("std::vector<uint8_t>");
    qRegisterMetaType<PackageCommand*>("PackageCommand*");
    //----------------------------------
    ui->groupBox_State->installEventFilter(this);
    ui->groupBox_net->installEventFilter(this);
    ui->OscillogPlot->installEventFilter(this);
    ui->pageCalibrations->installEventFilter(this);
    ui->pageCapacitors->installEventFilter(this);
    ui->pageProtection->installEventFilter(this);
    ui->pageFilters->installEventFilter(this);    
    //----------------------------------
    connect(kkm,SIGNAL(setConnection(bool)),
            this,SLOT(setConnection(bool)));
    connect(kkm,&KKM::setOscillog,
            this,&MainWindow::setOscillog);
    connect(kkm,&KKM::setNetVoltage,
            this,&MainWindow::setNetVoltage);
    connect(kkm,&KKM::setSwitchOnOff,
            this,&MainWindow::setSwitchOnOff);
    connect(kkm,&KKM::setNetVoltageRAW,
            this,&MainWindow::setNetVoltageRAW);
    connect(kkm,&KKM::setNetParams,
            this,&MainWindow::setNetParams);
    connect(kkm,&KKM::setVersionInfo,
            this,&MainWindow::setVersionInfo);
    connect(kkm,&KKM::setWorkState,
            this,&MainWindow::setWorkState);
    connect(kkm,&KKM::setEvents,
            this,&MainWindow::setEvents);
    connect(kkm,SIGNAL(Message(quint8,quint8,quint8,QString)),
            this, SLOT(Message(quint8,quint8,quint8,QString)));

    connect(kkm,&KKM::setSettingsCalibrations,
            this,&MainWindow::setSettingsCalibrations);
    connect(kkm,&KKM::setSettingsProtection,
            this,&MainWindow::setSettingsProtection);
    connect(kkm,&KKM::setSettingsCapacitors,
            this,&MainWindow::setSettingsCapacitors);
    connect(kkm,&KKM::setSettingsFilters,
            this,&MainWindow::setSettingsFilters);

    connect(kkm,&KKM::ansSettingsCalibrations,
            this,&MainWindow::ansSettingsCalibrations);
    connect(kkm,&KKM::ansSettingsProtection,
            this,&MainWindow::ansSettingsProtection);
    connect(kkm,&KKM::ansSettingsCapacitors,
            this,&MainWindow::ansSettingsCapacitors);   
    connect(kkm,&KKM::ansSettingsFilters,
            this,&MainWindow::ansSettingsFilters);
    //------------
    connect(this,SIGNAL(updateNetVoltage()),
            kkm,SLOT(updateNetVoltage()));
    connect(this,SIGNAL(updateNetVoltageRAW()),
            kkm,SLOT(updateNetVoltageRAW()));
    connect(this,SIGNAL(updateNetParams()),
            kkm,SLOT(updateNetParams()));
    connect(this,SIGNAL(updateOscillog(uint16_t)),
            kkm,SLOT(updateOscillog(uint16_t)));
    connect(this,&MainWindow::updateWorkState,
            kkm,&KKM::updateWorkState);
    connect(this,SIGNAL(updateVersionInfo()),
            kkm,SLOT(updateVersionInfo()));
    connect(this,&MainWindow::updateEvents,
            kkm,&KKM::updateEvents);

    connect(this,&MainWindow::updateSettingsCalibrations,
            kkm,&KKM::updateSettingsCalibrations);
    connect(this,&MainWindow::updateSettingsProtection,
            kkm,&KKM::updateSettingsProtection);
    connect(this,&MainWindow::updateSettingsCapacitors,
            kkm,&KKM::updateSettingsCapacitors);
    connect(this,&MainWindow::updateSettingsFilters,
            kkm,&KKM::updateSettingsFilters);

    connect(this,&MainWindow::writeSettingsCalibrations,
            kkm,&KKM::writeSettingsCalibrations);
    connect(this,&MainWindow::writeSettingsProtection,
            kkm,&KKM::writeSettingsProtection);
    connect(this,&MainWindow::writeSettingsCapacitors,
            kkm,&KKM::writeSettingsCapacitors);
    connect(this,&MainWindow::writeSettingsFilters,
            kkm,&KKM::writeSettingsFilters);

    connect(this,&MainWindow::writeSwitchOnOff,
            kkm,&KKM::writeSwitchOnOff);
    //-------------------------------------------
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
    connect(&timer_Events,SIGNAL(timeout()),
            this,SLOT(timerEvents()));
    //--------------------------------------------
    page_Main_Init();
    page_Oscillog_Init();
    page_SettingsCalibrations_Init();
    page_SettingsCapacitors_Init();
    page_SettingsProtection_Init();
    page_SettingsFilters_Init();

    timer_Events.start(1000);

    ui->listWidget->setItemDelegate(new HtmlDelegate);
}

void MainWindow::timerupdateNetVoltage(){
    if(_connected)updateNetVoltage();
}
void MainWindow::timerupdateNetVoltageRaw(){
    if(_connected)updateNetVoltageRAW();
}
void MainWindow::timerSettingsCapacitors(){
    if(_connected)updateSettingsCapacitors();
}
void MainWindow::timerSettingsProtection(){
    if(_connected)updateSettingsProtection();
}
void MainWindow::timerSettingsFilters(){
    if(_connected)updateSettingsFilters();
}
void MainWindow::timerSettingsCalibrations(){
    if(_connected)updateSettingsCalibrations();
}

void MainWindow::timerEvents(){
    if(_connected)updateEvents(lastIndexEvents);
}

void MainWindow::timerWorkState(){
    if(_connected)updateWorkState(QDateTime::currentMSecsSinceEpoch());
}
void MainWindow::timerVersion(){
    updateVersionInfo();
}
void MainWindow::timerNetParams(){
    if(_connected)updateNetParams();
}

void MainWindow::timerOscillog(){
    if(!_connected)return;
    if(ui->checkBox_osc_I_a->isChecked())updateOscillog(OSC_I_A);
    if(ui->checkBox_osc_I_b->isChecked())updateOscillog(OSC_I_B);
    if(ui->checkBox_osc_I_c->isChecked())updateOscillog(OSC_I_C);

    if(ui->checkBox_osc_Ua->isChecked())updateOscillog(OSC_U_A);
    if(ui->checkBox_osc_Ub->isChecked())updateOscillog(OSC_U_B);
    if(ui->checkBox_osc_Uc->isChecked())updateOscillog(OSC_U_C);

    if(ui->checkBox_osc_Ud->isChecked())updateOscillog(OSC_UD);

    if(ui->checkBox_osc_Icomp_a->isChecked())updateOscillog(OSC_COMP_A);
    if(ui->checkBox_osc_Icomp_b->isChecked())updateOscillog(OSC_COMP_B);
    if(ui->checkBox_osc_Icomp_c->isChecked())updateOscillog(OSC_COMP_C);
//-------------------
    ui->OscillogPlot->graph(OSCILLOG_I_A)->setVisible(ui->checkBox_osc_I_a->isChecked());
    ui->OscillogPlot->graph(OSCILLOG_I_B)->setVisible(ui->checkBox_osc_I_b->isChecked());
    ui->OscillogPlot->graph(OSCILLOG_I_C)->setVisible(ui->checkBox_osc_I_c->isChecked());

    ui->OscillogPlot->graph(OSCILLOG_U_A)->setVisible(ui->checkBox_osc_Ua->isChecked());
    ui->OscillogPlot->graph(OSCILLOG_U_B)->setVisible(ui->checkBox_osc_Ub->isChecked());
    ui->OscillogPlot->graph(OSCILLOG_U_C)->setVisible(ui->checkBox_osc_Uc->isChecked());

    ui->OscillogPlot->graph(OSCILLOG_UD)->setVisible(ui->checkBox_osc_Ud->isChecked());

    ui->OscillogPlot->graph(OSCILLOG_COMP_A)->setVisible(ui->checkBox_osc_Icomp_a->isChecked());
    ui->OscillogPlot->graph(OSCILLOG_COMP_B)->setVisible(ui->checkBox_osc_Icomp_b->isChecked());
    ui->OscillogPlot->graph(OSCILLOG_COMP_C)->setVisible(ui->checkBox_osc_Icomp_c->isChecked());
}

#define SETFILTER(UI,OBJ,TIMER) \
    if( object == UI ){\
        if( event->type() == QEvent::Show ){\
            OBJ.start(TIMER+((qrand()%(TIMER/4))-(TIMER/4)));\
        }\
        if( event->type() == QEvent::Hide ){\
            OBJ.stop();\
        }\
    }
bool MainWindow::eventFilter( QObject *object, QEvent *event ){
    SETFILTER(ui->groupBox_net,timer_MainParams,300);
    SETFILTER(ui->groupBox_net,timer_Vol,300);
    SETFILTER(ui->groupBox_net,timer_Raw,300);
    SETFILTER(ui->groupBox_net,timer_State,300);
    SETFILTER(ui->groupBox_State,timer_Version,3000);
    SETFILTER(ui->OscillogPlot,timer_Oscillog,54);
    SETFILTER(ui->pageCalibrations,timer_SettingsCalibrations,300);
    SETFILTER(ui->pageCapacitors,timer_SettingsCapacitors,300);
    SETFILTER(ui->pageProtection,timer_SettingsProtection,300);
    SETFILTER(ui->pageFilters,timer_SettingsFilters,300);
    return QMainWindow::eventFilter( object, event );
}
void MainWindow::KKMconnected(){
    ui->actionConnect->setEnabled(false);
    ui->actionDisconnect->setEnabled(true);
    ui->actionConfigure->setEnabled(false);
}
void MainWindow::KKMdisconnected(){
    ui->actionConnect->setEnabled(true);
    ui->actionDisconnect->setEnabled(false);
    ui->actionConfigure->setEnabled(true);
}
#define SCOL(STR,COL) "<font color=" COL ">" STR "</font>"
#define DARK_GREY "#808080"
#define DARK_RED "#800000"
#define LIGHT_GREY "#c0c0c0"
#define DARK_GREEN "#008000"
void MainWindow::setEvents(QVector<struct sEventRecord> ev){
    foreach(struct sEventRecord event, ev){
        if(event.unixTime_s_ms>=lastIndexEvents){
            lastIndexEvents = event.unixTime_s_ms+1;
        }
        if(lastIndexEvents>4133894400000){
            lastIndexEvents=0;
        }
        char Phases[3]={'A','B','C'};
        char* ADCchannels[]={
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
        QString str;
        QTextStream stream(&str);
        stream.setRealNumberNotation(QTextStream::FixedNotation);
        stream.setRealNumberPrecision(2);
        uint32_t subtype=(event.type>>16)&0xFFFF;

        QDateTime timestamp;
        timestamp.setTime_t(event.unixTime_s_ms/1000);
        stream << "<font color=" LIGHT_GREY ">";
        stream << timestamp.toString("dd.MM.yyyy hh:mm:ss");
        stream << ".";
        stream << QString().sprintf("%03d",event.unixTime_s_ms%1000);
        stream <<"</font>";

        switch(event.type&0xFFFF){
            case EVENT_TYPE_POWER:
            stream << QString(SCOL(" - Питание - ",DARK_GREEN));
            switch(subtype){
                case SUB_EVENT_TYPE_POWER_ON:
                    stream << QString("Включен");
                break;
                default:
                stream << QString(SCOL(" - Неизвестное событие! ", DARK_RED));
                break;
            }
            break;
            case EVENT_TYPE_CHANGESTATE:
            stream << QString(SCOL(" - Состояние - ",DARK_GREEN));
            switch(subtype){
                case KKM_STATE_INIT: //предзаряд
                    stream << QString("Инициализация");
                break;
                case KKM_STATE_STOP: //не работает
                    stream << QString("Остановлен");
                break;
                case KKM_STATE_SYNC: //синхронизация с сетью
                    stream << QString("Синхронизация");
                break;
                case KKM_STATE_PRECHARGE_PREPARE: //синхронизация с сетью
                    stream << QString("Подг.предзаряда");
                break;
                case KKM_STATE_PRECHARGE: //предзаряд
                    stream << QString("Предзаряд");
                break;                
                case KKM_STATE_MAIN:
                    stream << QString("Контактор");
                break;
                case KKM_STATE_PRECHARGE_DISABLE:
                    stream << QString("Выкл.предзаряда");
                break;
                case KKM_STATE_WORK: //работает
                    stream << QString("Работа");
                break;
                case KKM_STATE_CHARGE: //работает
                    stream << QString("Заряд");
                break;
                case KKM_STATE_TEST: //тестирование сети
                    stream << QString("Тест");
                break;
                case KKM_STATE_STOPPING: //ошибка
                    stream << QString("Остановка..");
                break;
                case KKM_STATE_FAULTBLOCK: //ошибка
                    stream << QString(SCOL(" Авария ", DARK_RED));
                break;
            }
            break;
            case EVENT_TYPE_PROTECTION:
            stream << QString(SCOL(" - Защиты ",DARK_RED));
            switch(subtype){
                case SUB_EVENT_TYPE_PROTECTION_UD_MIN:
                    stream << QString(" - Слишком низкое напряжение конденсатора во время работы ");
                    stream << "(" << event.info+1 << ") : "<< event.value << " B";
                break;
                case SUB_EVENT_TYPE_PROTECTION_UD_MAX:
                    stream << QString(" - Превышение напряжения конденсатора ");
                    stream << "(" << event.info+1 << ") : "<< event.value << " B";
                break;
                case SUB_EVENT_TYPE_PROTECTION_TEMPERATURE:
                    stream << QString(" - Превышение температуры ");
                    stream << event.value << QString(" °С");
                break;
                case SUB_EVENT_TYPE_PROTECTION_U_MIN:
                    stream << QString(" - Слишком низкое напряжение фазы ");
                    stream << Phases[event.info] << ": "<< event.value << " B";
                break;
                case SUB_EVENT_TYPE_PROTECTION_U_MAX:
                    stream << QString(" - Превышение напряжения фазы ");
                    stream << Phases[event.info] << ": "<< event.value << " B";
                break;
                case SUB_EVENT_TYPE_PROTECTION_F_MIN:
                    stream << QString(" - Слишком низкая частота сети ");
                    stream << event.value << QString("Гц");
                break;
                case SUB_EVENT_TYPE_PROTECTION_F_MAX:
                    stream << QString(" - Слишком высокая частота сети ");
                    stream << event.value << QString("Гц");
                break;
                case SUB_EVENT_TYPE_PROTECTION_IAFG_MAX_RMS:
                    stream << QString(" - Превышение среднеквадратического тока АДФ по фазе ");
                    stream << Phases[event.info] << ": "<< event.value << " A";
                break;
                case SUB_EVENT_TYPE_PROTECTION_IAFG_MAX_PEAK:
                    stream << QString(" - Превышение пикового тока АДФ по фазе ");
                    stream << Phases[event.info] << ": "<< event.value << " A";
                break;
                case SUB_EVENT_TYPE_PROTECTION_PHASES:
                    stream << QString(" - Неверное отношение фаз напряжения ");
                break;
                case SUB_EVENT_TYPE_PROTECTION_ADC_OVERLOAD:
                    stream << QString(" - Переполнение АЦП на канале ");
                    stream << ADCchannels[event.info] << ": "<< event.value;
                break;
                case SUB_EVENT_TYPE_PROTECTION_BAD_SYNC:
                    stream << QString(" - Проблемы синхронизации (качающаяся частота?) ");
                break;
                case SUB_EVENT_TYPE_PROTECTION_IGBT:
                    stream << QString(" - ошибка ключа IGBT ");
                    stream << event.info ;
                break;
                default:
                    stream << QString(SCOL(" - Неизвестное событие! ", DARK_RED));
                break;
            }
            break;
            case EVENT_TYPE_EVENT:
            stream << QString(" - Событие ");
            break;
            default:
            stream << QString(SCOL(" - Неизвестное событие! ", DARK_RED));
            break;
        }

        Message(MESSAGE_TYPE_STATE,MESSAGE_NORMAL,MESSAGE_TARGET_ALL,
                str);
    }
}

void MainWindow::Message(quint8 type, quint8 level, quint8 target, QString message){
        QString prefix;
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
        doc.setHtml( prefix );
        QString ss(doc.toPlainText());
        if(target&MESSAGE_TARGET_DEBUG){
            qDebug()<<ss;
        }
        if(target&MESSAGE_TARGET_STATUS){
            ui->statusBar->showMessage(ss,5000);
        }
        if(target&MESSAGE_TARGET_HISTORY){
            QString s ("<font color=" LIGHT_GREY ">");
            s.append(QDateTime::currentDateTime ().toString("dd.MM.yyyy hh:mm:ss:"));
            s.append("</font>");
            s+=prefix;
            QListWidgetItem *i=new QListWidgetItem(s, ui->listWidget);
            Q_UNUSED(i);
        }
}
MainWindow::~MainWindow(){
    delete settings;
    delete ui;
}
void MainWindow::openSerialPort(){
    SettingsDialog::Settings p = settings->settings();

    QMetaObject::invokeMethod(kkm->_interface, "ConnectTo",
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
    QMetaObject::invokeMethod(kkm->_interface, "Disconnect");
}
void MainWindow::about(){
    QMessageBox::about(this, tr("About Simple Terminal"),
                       tr("The <b>Simple Terminal</b> example demonstrates how to "
                          "use the Qt Serial Port module in modern GUI applications "
                          "using Qt, with a menu bar, toolbars, and a status bar."));
}
void MainWindow::initActionsConnections()
{
    connect(ui->actionConnect, SIGNAL(triggered()), this, SLOT(openSerialPort()));
    connect(ui->actionDisconnect, SIGNAL(triggered()), this, SLOT(closeSerialPort()));
    connect(ui->actionQuit, SIGNAL(triggered()), this, SLOT(close()));
    connect(ui->actionConfigure, SIGNAL(triggered()), settings, SLOT(show()));
    connect(ui->actionAbout, SIGNAL(triggered()), this, SLOT(about()));
    connect(ui->actionAboutQt, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
    connect(settings, SIGNAL(Appl()), this, SLOT(openSerialPort()));
}
//========================================================================
//========================================================================
//========================================================================

//========================================================================
#define FCOEFF 0.9
#define FILTERADD(A,B) A=A*FCOEFF+B*(1-FCOEFF)
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
    FILTERADD(KKM_var.ADC.ADC_U_A,ADC_U_A);
    FILTERADD(KKM_var.ADC.ADC_U_B,ADC_U_B);
    FILTERADD(KKM_var.ADC.ADC_U_C,ADC_U_C);

    FILTERADD(KKM_var.ADC.ADC_I_A,ADC_I_A);
    FILTERADD(KKM_var.ADC.ADC_I_B,ADC_I_B);
    FILTERADD(KKM_var.ADC.ADC_I_C,ADC_I_C);

    FILTERADD(KKM_var.ADC.ADC_MATH_A,ADC_MATH_A);
    FILTERADD(KKM_var.ADC.ADC_MATH_B,ADC_MATH_B);
    FILTERADD(KKM_var.ADC.ADC_MATH_C,ADC_MATH_C);

    KKM_var.ADC.ADC_UD=ADC_UD;

    FILTERADD(KKM_var.ADC.ADC_I_TEMP1,ADC_I_TEMP1);
    FILTERADD(KKM_var.ADC.ADC_I_TEMP2,ADC_I_TEMP2);

    ui->label_Ua->setText(QString().sprintf("% 5.0f В",KKM_var.ADC.ADC_MATH_A));
    ui->label_Ub->setText(QString().sprintf("% 5.0f В",KKM_var.ADC.ADC_MATH_B));
    ui->label_Uc->setText(QString().sprintf("% 5.0f В",KKM_var.ADC.ADC_MATH_C));

    ui->label_I_A->setText(QString().sprintf("% 5.1f А",KKM_var.ADC.ADC_I_A));
    ui->label_I_B->setText(QString().sprintf("% 5.1f А",KKM_var.ADC.ADC_I_B));
    ui->label_I_C->setText(QString().sprintf("% 5.1f А",KKM_var.ADC.ADC_I_C));

    ui->label_temperature1->setText(QString().sprintf("% 3.0f °C",KKM_var.ADC.ADC_I_TEMP1));
    ui->label_temperature2->setText(QString().sprintf("% 3.0f °C",KKM_var.ADC.ADC_I_TEMP2));
}
//========================================================================
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
    FILTERADD(KKM_var.ADC_RAW.ADC_U_A,ADC_U_A);
    FILTERADD(KKM_var.ADC_RAW.ADC_U_B,ADC_U_B);
    FILTERADD(KKM_var.ADC_RAW.ADC_U_C,ADC_U_C);

    FILTERADD(KKM_var.ADC_RAW.ADC_I_A,ADC_I_A);
    FILTERADD(KKM_var.ADC_RAW.ADC_I_B,ADC_I_B);
    FILTERADD(KKM_var.ADC_RAW.ADC_I_C,ADC_I_C);

    FILTERADD(KKM_var.ADC_RAW.ADC_UD,ADC_UD);

    FILTERADD(KKM_var.ADC_RAW.ADC_I_TEMP1,ADC_I_TEMP1);
    FILTERADD(KKM_var.ADC_RAW.ADC_I_TEMP2,ADC_I_TEMP2);
}
//========================================================================
void MainWindow::setNetParams(
                float period_fact,
                float U0Hz_A,//Постоянная составляющая
                float U0Hz_B,
                float U0Hz_C,
                float I0Hz_A,//Постоянная составляющая
                float I0Hz_B,
                float I0Hz_C,
                float thdu_A,
                float thdu_B,
                float thdu_C,
                float U_phase_A,
                float U_phase_B,
                float U_phase_C){
    KKM_var.NET_PARAMS.period_fact=period_fact;

    KKM_var.NET_PARAMS.U0Hz_A=U0Hz_A;//Постоянная составляющая
    KKM_var.NET_PARAMS.U0Hz_B=U0Hz_B;
    KKM_var.NET_PARAMS.U0Hz_C=U0Hz_C;
    KKM_var.NET_PARAMS.I0Hz_A=I0Hz_A;//Постоянная составляющая
    KKM_var.NET_PARAMS.I0Hz_B=I0Hz_B;
    KKM_var.NET_PARAMS.I0Hz_C=I0Hz_C;

    KKM_var.NET_PARAMS.thdu_A=thdu_A;
    KKM_var.NET_PARAMS.thdu_B=thdu_B;
    KKM_var.NET_PARAMS.thdu_C=thdu_C;

    KKM_var.NET_PARAMS.U_phase_A=U_phase_A*360.0/3.1416;
    KKM_var.NET_PARAMS.U_phase_B=U_phase_B*360.0/3.1416;
    KKM_var.NET_PARAMS.U_phase_C=U_phase_C*360.0/3.1416;

#define PRINT_PARAM(PAR,STR) \
    ui->label_##PAR ->setText(QString().sprintf(STR,KKM_var.NET_PARAMS.PAR));

    PRINT_PARAM(U_phase_B,"% 5.1f°");
    PRINT_PARAM(U_phase_C,"% 5.1f°");

    PRINT_PARAM(thdu_A,"% 5.2f");
    PRINT_PARAM(thdu_B,"% 5.2f");
    PRINT_PARAM(thdu_C,"% 5.2f");

    ui->label_freq->setText(QString().sprintf("% 6.3f Гц",1/(KKM_var.NET_PARAMS.period_fact/1000000.0)));
}
//========================================================================
void MainWindow::ansSettingsCalibrations(bool writed){
    Q_UNUSED(writed);
}
//========================================================================
void MainWindow::ansSettingsProtection(bool writed){
    Q_UNUSED(writed);
}
//========================================================================
void MainWindow::ansSettingsCapacitors(bool writed){
    Q_UNUSED(writed);
}
//========================================================================
void MainWindow::ansSettingsFilters(bool writed){
    Q_UNUSED(writed);
}
//========================================================================
//========================================================================
//========================================================================
enum{
    COMMAND_WORK_ON=1,
    COMMAND_WORK_OFF,
    COMMAND_CHARGE_ON,
    COMMAND_CHARGE_OFF,
    COMMAND_CHANNEL0_DATA,
    COMMAND_CHANNEL1_DATA,
    COMMAND_CHANNEL2_DATA,
    COMMAND_SETTINGS_SAVE
};
void MainWindow::on_pushButton_STOP_clicked(){
    writeSwitchOnOff(COMMAND_WORK_OFF,0);
}
void MainWindow::on_pushButton_Start_clicked(){
    writeSwitchOnOff(COMMAND_WORK_ON,0);
}
void MainWindow::setSwitchOnOff(uint32_t result){
    Q_UNUSED(result);
}
void MainWindow::on_pushButton_Save_clicked()
{
    writeSwitchOnOff(COMMAND_SETTINGS_SAVE,0);
}
void MainWindow::on_actionClear_triggered(){
    ui->listWidget->clear();
}
void MainWindow::on_pushButton_2_clicked(){
      ui->listWidget->clear();
}
void MainWindow::on_checkBox_channelA_toggled(bool checked)
{
 writeSwitchOnOff(COMMAND_CHANNEL0_DATA,checked);
}

void MainWindow::on_checkBox_channelB_toggled(bool checked)
{
 writeSwitchOnOff(COMMAND_CHANNEL1_DATA,checked);
}

void MainWindow::on_checkBox_channelC_toggled(bool checked)
{
 writeSwitchOnOff(COMMAND_CHANNEL2_DATA,checked);
}

void MainWindow::on_pushButton_CHARGE_ON_clicked()
{
    qDebug()<<"ON";
        writeSwitchOnOff(COMMAND_CHARGE_ON,0);
}

void MainWindow::on_pushButton_CHARGE_OFF_clicked()
{ qDebug()<<"OFF";
        writeSwitchOnOff(COMMAND_CHARGE_OFF,0);
}
