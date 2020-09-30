#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "settingsdialog.h"
#include <QMessageBox>
#include <QMessageBox>
#include <QDateTime>
#include <QGraphicsDropShadowEffect>
#include <QListWidgetItem>

QVector<double> xval(HARMONICS_NUM),yval(HARMONICS_NUM),ayval(HARMONICS_NUM);

void MainWindow::page_InputPhaseCorrection_Init(){
    ui->eqplot->addGraph();
    ui->eqplot->setDragInfo(0, 20,6.28,-6.28); // data of graph(0) is dragable, 20px margin
    for(int i=0;i<HARMONICS_NUM;i++){
        yval[i]=0;
        xval[i]=i;
    }
    ui->eqplot->graph(0)->setData(xval,yval);
    ui->eqplot->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 5));
    ui->eqplot->xAxis->setRange(0, HARMONICS_NUM);

    ui->eqplot->xAxis->setLabel("Номер гармоники");
    ui->eqplot->yAxis->setLabel("Угол сдвига фазы, рад.");

    ui->eqplot->rescaleAxes(true);

    QSharedPointer<QCPAxisTickerFixed> fixedTicker(new QCPAxisTickerFixed);
    ui->eqplot->xAxis->setTicker(fixedTicker);
    fixedTicker->setTickStep(2.0);
    fixedTicker->setTickOrigin(1);
    fixedTicker->setScaleStrategy(QCPAxisTickerFixed::ssNone); // and no scaling of the tickstep (like multiples or powers) is allowed
    ui->eqplot->xAxis->setSubTicks(false);

    connect(ui->eqplot,SIGNAL(EditingFinished(QVector<float>)),
            this,SLOT(HarmonicsPhaseShift_EditFinished(QVector<float>)));
    connect(ui->eqplot,SIGNAL(DataChanged(QVector<float>)),
            this,SLOT(HarmonicsPhaseShift_DataChanged(QVector<float>)));
    connect(ui->eqplot,SIGNAL(EditingStarted()),
            this,SLOT(HarmonicsPhaseShift_EditStarted()));

    isPhaseShiftEditing=false;
    //--------------------------------
    ui->eqplot_ampl->addGraph();
    ui->eqplot_ampl->setDragInfo(0, 20,20,-20); // data of graph(0) is dragable, 20px margin
    for(int i=0;i<HARMONICS_NUM;i++){
        ayval[i]=0;
        xval[i]=i;
    }
    ui->eqplot_ampl->graph(0)->setData(xval,ayval);
    ui->eqplot_ampl->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 5));
    ui->eqplot_ampl->xAxis->setRange(0, HARMONICS_NUM);

    ui->eqplot_ampl->xAxis->setLabel("Номер гармоники");
    ui->eqplot_ampl->yAxis->setLabel("Амплитуда");

    ui->eqplot_ampl->rescaleAxes(true);

    ui->eqplot_ampl->xAxis->setTicker(fixedTicker);
    ui->eqplot_ampl->xAxis->setSubTicks(false);

    connect(ui->eqplot_ampl,SIGNAL(EditingFinished(QVector<float>)),
            this,SLOT(HarmonicsAmplShift_EditFinished(QVector<float>)));
    connect(ui->eqplot_ampl,SIGNAL(DataChanged(QVector<float>)),
            this,SLOT(HarmonicsAmplShift_DataChanged(QVector<float>)));
    connect(ui->eqplot_ampl,SIGNAL(EditingStarted()),
            this,SLOT(HarmonicsAmplShift_EditStarted()));

    isAmplShiftEditing=false;
}
void MainWindow::HarmonicsPhaseShift_EditFinished(QVector<float> data ){
    HarmonicsPhaseShift_DataChanged(data);
    isPhaseShiftEditing=false;
}
void MainWindow::HarmonicsPhaseShift_DataChanged( QVector<float> data ){
    ui->eqplot->yAxis->setRange(
                *std::max_element(data.constBegin(),data.constEnd())+0.5,
                *std::min_element(data.constBegin(),data.constEnd())-0.5);
    ui->eqplot->replot();
    writeSettingsHarmonicsPhaseShift(data);
}
void MainWindow::setSettingsHarmonicsPhaseShift(
        QVector<float> phaseShift){
    for(int i=0;i<HARMONICS_NUM;i++){
        yval[i]=phaseShift[i];
        xval[i]=i;
    }
    ui->eqplot->graph(0)->setData(xval,yval);
    ui->eqplot->yAxis->setRange(
                *std::max_element(yval.constBegin(),yval.constEnd())+0.5,
                *std::min_element(yval.constBegin(),yval.constEnd())-0.5);
    ui->eqplot->replot();
}
void MainWindow::HarmonicsPhaseShift_EditStarted(){
    isPhaseShiftEditing=true;
}
//-------------------------
void MainWindow::HarmonicsAmplShift_EditFinished(QVector<float> data ){
    HarmonicsAmplShift_DataChanged(data);
    isAmplShiftEditing=false;
}
void MainWindow::HarmonicsAmplShift_DataChanged( QVector<float> data ){
    ui->eqplot_ampl->yAxis->setRange(
                *std::max_element(data.constBegin(),data.constEnd())+0.5,
                *std::min_element(data.constBegin(),data.constEnd())-0.5);
    ui->eqplot_ampl->replot();
    writeSettingsHarmonicsAmplShift(data);
}
void MainWindow::setSettingsHarmonicsAmplShift(
        QVector<float> amplShift){
    for(int i=0;i<HARMONICS_NUM;i++){
        ayval[i]=amplShift[i];
        xval[i]=i;
    }
    ui->eqplot_ampl->graph(0)->setData(xval,ayval);
    ui->eqplot_ampl->yAxis->setRange(
                *std::max_element(ayval.constBegin(),ayval.constEnd())+0.5,
                *std::min_element(ayval.constBegin(),ayval.constEnd())-0.5);
    ui->eqplot_ampl->replot();
}
void MainWindow::HarmonicsAmplShift_EditStarted(){
    isAmplShiftEditing=true;
}

