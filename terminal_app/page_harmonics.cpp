#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "settingsdialog.h"
#include <QMessageBox>
#include <QMessageBox>
#include <QDateTime>
#include <QGraphicsDropShadowEffect>
#include <QListWidgetItem>

void MainWindow::page_Harmonics_Init()
{
    // create empty bar chart objects:
    QCPBarsGroup *group1 = new QCPBarsGroup(ui->Harmonics_plot);
    loadCurrentHarmonics = new QCPBars *[3];
    compCurrentHarmonics = new QCPBars *[3];
    for (int i = 0; i < 3; i++)
    {
        *(loadCurrentHarmonics + i) = new QCPBars(ui->Harmonics_plot->xAxis, ui->Harmonics_plot->yAxis);
        *(compCurrentHarmonics + i) = new QCPBars(ui->Harmonics_plot->xAxis, ui->Harmonics_plot->yAxis);
    }
    QColor colorsA[] = {Qt::red, Qt::blue, Qt::darkGreen};
    //QColor colorsB[]={Qt::magenta,Qt::cyan,Qt::yellow};
    for (int i = 0; i < 3; i++)
    {
        loadCurrentHarmonics[i]->setAntialiased(false);
        loadCurrentHarmonics[i]->setStackingGap(0);
        loadCurrentHarmonics[i]->setPen(QPen(QColor(colorsA[i]), 1));
        loadCurrentHarmonics[i]->setBrush(QColor(colorsA[i]).lighter(130));
        loadCurrentHarmonics[i]->setBarsGroup(group1);
        loadCurrentHarmonics[i]->setWidth(1.0 / (50.0 * 10.0));
        loadCurrentHarmonics[i]->setWidthType(QCPBars::wtAxisRectRatio);
        // }
        // for(int i=0;i<3;i++){
        compCurrentHarmonics[i]->setAntialiased(false);
        compCurrentHarmonics[i]->setStackingGap(0);
        compCurrentHarmonics[i]->setPen(QPen(QColor(0, 168, 140), 1));
        compCurrentHarmonics[i]->setBrush(QColor(0, 168, 140).darker(100));
        //    compCurrentHarmonics[i]->moveAbove(loadCurrentHarmonics[i]);
        compCurrentHarmonics[i]->setBarsGroup(group1);
        compCurrentHarmonics[i]->setWidth(1.0 / (50.0 * 10.0));
        compCurrentHarmonics[i]->setWidthType(QCPBars::wtAxisRectRatio);
    }
    // Add data

    QVector<double> loadData(HARMONICS_NUM);
    QVector<double> compData(HARMONICS_NUM);
    QVector<double> ticks(HARMONICS_NUM);
    for (int i = 1; i < ticks.size(); i++)
    {
        ticks[i] = i;
        loadData[i] = 0;
        compData[i] = 0;
    }
    for (int i = 0; i < 3; i++)
    {
        loadCurrentHarmonics[i]->setData(ticks, loadData);
        loadCurrentHarmonics_data[i].resize(HARMONICS_NUM);
    }
    for (int i = 0; i < 3; i++)
    {
        compCurrentHarmonics[i]->setData(ticks, compData);
        compCurrentHarmonics_data[i].resize(HARMONICS_NUM);
    }
    ui->Harmonics_plot->setInteractions(0 /*QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables*/);
    ui->Harmonics_plot->rescaleAxes();

    fixedTicker = QSharedPointer<QCPAxisTickerFixed>(new QCPAxisTickerFixed);
    ui->Harmonics_plot->xAxis->setTicker(fixedTicker);
    fixedTicker->setTickStep(2.0);  // tick step shall be 1.0
    fixedTicker->setTickOrigin(1);
    fixedTicker->setScaleStrategy(QCPAxisTickerFixed::ssNone);  // and no scaling of the tickstep (like multiples or powers) is allowed
    ui->Harmonics_plot->xAxis->setSubTicks(false);
    ui->Harmonics_plot->yAxis->setRange(0, 120);
    ui->Harmonics_plot->xAxis->setRange(-0.4, HARMONICS_NUM + 0.4 - 1);
    ui->Harmonics_plot->replot();
}
void MainWindow::on_pushButton_autoharm_clicked()
{
    ui->Harmonics_plot->yAxis->rescale(true);
}

void MainWindow::on_horizontalSlider_harmonicsSize_valueChanged(int value)
{
    maxHarm = minHarm + value;
    ui->Harmonics_plot->xAxis->setRange(-0.4 + minHarm, maxHarm + 0.4);
    for (int i = 0; i < 3; i++)
    {
        loadCurrentHarmonics[i]->setWidth(1.0 / ((maxHarm - minHarm) * 10.0));
        compCurrentHarmonics[i]->setWidth(1.0 / ((maxHarm - minHarm) * 10.0));
    }
    if (value > 25)
    {
        fixedTicker->setTickStep(2.0);
        fixedTicker->setTickOrigin(1);
    }
    else
    {
        fixedTicker->setTickStep(1.0);
        fixedTicker->setTickOrigin(0);
    }
    ui->Harmonics_plot->replot();

    ui->horizontalScrollBar_harmSize->setMaximum(50 - value);
}

void MainWindow::on_horizontalScrollBar_harmSize_valueChanged(int value)
{
    minHarm = value;
    maxHarm = minHarm + ui->horizontalSlider_harmonicsSize->value();
    ui->Harmonics_plot->xAxis->setRange(-0.4 + minHarm, maxHarm + 0.4);
    for (int i = 0; i < 3; i++)
    {
        loadCurrentHarmonics[i]->setWidth(1.0 / ((maxHarm - minHarm) * 10.0));
        compCurrentHarmonics[i]->setWidth(1.0 / ((maxHarm - minHarm) * 10.0));
    }
    ui->Harmonics_plot->replot();
}
