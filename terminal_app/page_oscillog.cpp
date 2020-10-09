#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "settingsdialog.h"
#include <QMessageBox>
#include <QMessageBox>
#include <QDateTime>
#include <QGraphicsDropShadowEffect>
#include <QListWidgetItem>
#include <QMap>

QMap<uint32_t,uint32_t> OSCILLOG_ARR;

void MainWindow::pageOscillogInit(){
    OSCILLOG_ARR[OSC_U_A]=OSCILLOG_U_A;
    OSCILLOG_ARR[OSC_U_B]=OSCILLOG_U_B;
    OSCILLOG_ARR[OSC_U_C]=OSCILLOG_U_C;
    OSCILLOG_ARR[OSC_I_A]=OSCILLOG_I_A;
    OSCILLOG_ARR[OSC_I_B]=OSCILLOG_I_B;
    OSCILLOG_ARR[OSC_I_C]=OSCILLOG_I_C;
    OSCILLOG_ARR[OSC_UD]=OSCILLOG_UD;
    OSCILLOG_ARR[OSC_COMP_A]=OSCILLOG_COMP_A;
    OSCILLOG_ARR[OSC_COMP_B]=OSCILLOG_COMP_B;
    OSCILLOG_ARR[OSC_COMP_C]=OSCILLOG_COMP_C;

    #define U_AXIS ui->OscillogPlot->xAxis, ui->OscillogPlot->yAxis2
    #define I_AXIS ui->OscillogPlot->xAxis, ui->OscillogPlot->yAxis

    ui->OscillogPlot->addGraph(I_AXIS)->setName("Ia"); // Ia
    ui->OscillogPlot->addGraph(I_AXIS)->setName("Ib"); // Ib
    ui->OscillogPlot->addGraph(I_AXIS)->setName("Ic"); // Ic

    ui->OscillogPlot->addGraph(U_AXIS)->setName("Ua"); // Ua
    ui->OscillogPlot->addGraph(U_AXIS)->setName("Ub"); // Ub
    ui->OscillogPlot->addGraph(U_AXIS)->setName("Uc"); // Uc

    ui->OscillogPlot->addGraph(U_AXIS)->setName("U"); // U1

    ui->OscillogPlot->addGraph(I_AXIS)->setName("Icompa"); // Icompa
    ui->OscillogPlot->addGraph(I_AXIS)->setName("Icompb"); // Icompb
    ui->OscillogPlot->addGraph(I_AXIS)->setName("Icompc"); // Icompc

    ui->OscillogPlot->graph(OSCILLOG_U_A)->setPen(QPen(Qt::red));
    ui->OscillogPlot->graph(OSCILLOG_U_B)->setPen(QPen(Qt::blue));
    ui->OscillogPlot->graph(OSCILLOG_U_C)->setPen(QPen(Qt::green));

    ui->OscillogPlot->graph(OSCILLOG_I_A)->setPen(QPen(QColor(Qt::darkRed).lighter(200)));
    ui->OscillogPlot->graph(OSCILLOG_I_B)->setPen(QPen(QColor(Qt::darkBlue).lighter(200)));
    ui->OscillogPlot->graph(OSCILLOG_I_C)->setPen(QPen(QColor(Qt::darkGreen).lighter(200)));
    ui->OscillogPlot->graph(OSCILLOG_I_A)->setBrush(QBrush(QColor(139,0,0,100)));
    ui->OscillogPlot->graph(OSCILLOG_I_B)->setBrush(QBrush(QColor(0,0,139,100)));
    ui->OscillogPlot->graph(OSCILLOG_I_C)->setBrush(QBrush(QColor(0,139,0,100)));

    ui->OscillogPlot->graph(OSCILLOG_UD)->setPen(QPen(qRgb(139,0,139)));

    ui->OscillogPlot->graph(OSCILLOG_COMP_A)->setPen(QPen(qRgb(255,105,180)));
    ui->OscillogPlot->graph(OSCILLOG_COMP_B)->setPen(QPen(qRgb(123,104,238)));
    ui->OscillogPlot->graph(OSCILLOG_COMP_C)->setPen(QPen(qRgb(95,158,160)));

    ui->OscillogPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);

    ui->OscillogPlot->yAxis2->setVisible(true);
    // give the axes some labels:
    ui->OscillogPlot->xAxis->setLabel("мс");
    ui->OscillogPlot->yAxis->setLabel("А");
    ui->OscillogPlot->yAxis2->setLabel("В");
    // make left and bottom axes transfer their ranges to right and top axes:
    connect(ui->OscillogPlot->xAxis, SIGNAL(rangeChanged(QCPRange)), ui->OscillogPlot->xAxis2, SLOT(setRange(QCPRange)));

    ui->OscillogPlot->graph(0)->rescaleAxes(true);

    oscillog_xval.resize(128);

    for(int i=0;i<OSCILLOG_SIZE;i++){
        oscillog_data[i].resize(128);
    }
    for(int i=0;i<128;i++){
        oscillog_xval[i]=(1.0/50.0/128.0)*(float)i*1e3;
    }

    ui->OscillogPlot->xAxis->setRange(0, 20.0/*-20.0/128.0*/);
    ui->OscillogPlot->yAxis2->setRange(-220*sqrt(2)*1.5, 220*sqrt(2)*1.5);
    ui->OscillogPlot->yAxis->setRange(-1.2,1.2);
    ui->OscillogPlot->replot();
    //--------------------------------------------
    connect(ui->OscillogPlot->xAxis, SIGNAL(rangeChanged(QCPRange,QCPRange)),
            this,   SLOT(xAxisRangeChanged(QCPRange,QCPRange)));
}
void MainWindow::xAxisRangeChanged( QCPRange newRange,QCPRange oldRange ){
    Q_UNUSED(newRange);
    Q_UNUSED(oldRange);
    if (ui->OscillogPlot->xAxis->range().lower <= 0){
      ui->OscillogPlot->xAxis->setRangeLower(0);
    }
    if (ui->OscillogPlot->xAxis->range().upper >= 20){
      ui->OscillogPlot->xAxis->setRangeUpper(20);
    }
}
void MainWindow::mouseWheel(QWheelEvent *event) {
    double factor;
    double wheelSteps = event->delta()/120.0; // a single step delta is +/-120 usually
    factor = qPow(ui->OscillogPlot->axisRect()->rangeZoomFactor(Qt::Vertical), wheelSteps);
    ui->OscillogPlot->yAxis2->scaleRange(factor, ui->OscillogPlot->yAxis2->pixelToCoord(event->pos().y()));
}
void MainWindow::onPushButtonClicked(){
    ui->OscillogPlot->rescaleAxes(true);
}
void MainWindow::setOscillog(uint16_t channel, std::vector<double> data){
    float K1=((float)ui->horizontalSlider_filter->value())/1000.0;
    //qDebug()<<data;
    if(channel<=OSC_COMP_C){
        if(!OSCILLOG_ARR.contains(channel)){
            qDebug()<<"Channel error 1";
            return;
        }
        for(int i=0;i<oscillog_data[OSCILLOG_ARR[channel]].size();i++){
            oscillog_data[OSCILLOG_ARR[channel]][i]=
                    oscillog_data[OSCILLOG_ARR[channel]][i]*K1+data[i]*(1-K1);
        }
        ui->OscillogPlot->graph(OSCILLOG_ARR[channel])->setData(
                    QVector<double>::fromStdVector(oscillog_xval),
                    oscillog_data[OSCILLOG_ARR[channel]]);
        ui->OscillogPlot->replot();
    }else{
        qDebug()<<"Channel error 2";
    }
}
