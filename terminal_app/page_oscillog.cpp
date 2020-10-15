#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "settingsdialog.h"
#include <QMessageBox>
#include <QMessageBox>
#include <QDateTime>
#include <QGraphicsDropShadowEffect>
#include <QListWidgetItem>
#include <QMap>
/*--------------------------------------------------------------
                       NAMESPACES
--------------------------------------------------------------*/

using namespace PFCconfig;
using namespace PFCconfig::ADC;
using namespace PFCconfig::Interface;
using namespace PFCconfig::Events;

/*--------------------------------------------------------------
                       RIVATE FUNCTIONS
--------------------------------------------------------------*/

void MainWindow::pageOscillogInit(void)
{
    _oscillog_array[PFCOscillogCnannel::OSC_U_A] = OscillogChannels::OSCILLOG_U_A;
    _oscillog_array[PFCOscillogCnannel::OSC_U_B] = OscillogChannels::OSCILLOG_U_B;
    _oscillog_array[PFCOscillogCnannel::OSC_U_C] = OscillogChannels::OSCILLOG_U_C;
    _oscillog_array[PFCOscillogCnannel::OSC_I_A] = OscillogChannels::OSCILLOG_I_A;
    _oscillog_array[PFCOscillogCnannel::OSC_I_B] = OscillogChannels::OSCILLOG_I_B;
    _oscillog_array[PFCOscillogCnannel::OSC_I_C] = OscillogChannels::OSCILLOG_I_C;
    _oscillog_array[PFCOscillogCnannel::OSC_UD] = OscillogChannels::OSCILLOG_UD;
    _oscillog_array[PFCOscillogCnannel::OSC_COMP_A] = OscillogChannels::OSCILLOG_COMP_A;
    _oscillog_array[PFCOscillogCnannel::OSC_COMP_B] = OscillogChannels::OSCILLOG_COMP_B;
    _oscillog_array[PFCOscillogCnannel::OSC_COMP_C] = OscillogChannels::OSCILLOG_COMP_C;

    _ui->OscillogPlot->addGraph(_ui->OscillogPlot->xAxis, _ui->OscillogPlot->yAxis)->setName("Ia");  // Ia
    _ui->OscillogPlot->addGraph(_ui->OscillogPlot->xAxis, _ui->OscillogPlot->yAxis)->setName("Ib");  // Ib
    _ui->OscillogPlot->addGraph(_ui->OscillogPlot->xAxis, _ui->OscillogPlot->yAxis)->setName("Ic");  // Ic

    _ui->OscillogPlot->addGraph(_ui->OscillogPlot->xAxis, _ui->OscillogPlot->yAxis2)->setName("Ua");  // Ua
    _ui->OscillogPlot->addGraph(_ui->OscillogPlot->xAxis, _ui->OscillogPlot->yAxis2)->setName("Ub");  // Ub
    _ui->OscillogPlot->addGraph(_ui->OscillogPlot->xAxis, _ui->OscillogPlot->yAxis2)->setName("Uc");  // Uc

    _ui->OscillogPlot->addGraph(_ui->OscillogPlot->xAxis, _ui->OscillogPlot->yAxis2)->setName("U");  // U1

    _ui->OscillogPlot->addGraph(_ui->OscillogPlot->xAxis, _ui->OscillogPlot->yAxis)->setName("Icompa");  // Icompa
    _ui->OscillogPlot->addGraph(_ui->OscillogPlot->xAxis, _ui->OscillogPlot->yAxis)->setName("Icompb");  // Icompb
    _ui->OscillogPlot->addGraph(_ui->OscillogPlot->xAxis, _ui->OscillogPlot->yAxis)->setName("Icompc");  // Icompc

    _ui->OscillogPlot->graph(enum_int(OscillogChannels::OSCILLOG_U_A))->setPen(QPen(Qt::red));
    _ui->OscillogPlot->graph(enum_int(OscillogChannels::OSCILLOG_U_B))->setPen(QPen(Qt::blue));
    _ui->OscillogPlot->graph(enum_int(OscillogChannels::OSCILLOG_U_C))->setPen(QPen(Qt::green));

    _ui->OscillogPlot->graph(enum_int(OscillogChannels::OSCILLOG_I_A))->setPen(QPen(QColor(Qt::darkRed).lighter(200)));
    _ui->OscillogPlot->graph(enum_int(OscillogChannels::OSCILLOG_I_B))->setPen(QPen(QColor(Qt::darkBlue).lighter(200)));
    _ui->OscillogPlot->graph(enum_int(OscillogChannels::OSCILLOG_I_C))->setPen(QPen(QColor(Qt::darkGreen).lighter(200)));
    _ui->OscillogPlot->graph(enum_int(OscillogChannels::OSCILLOG_I_A))->setBrush(QBrush(QColor(139, 0, 0, 100)));
    _ui->OscillogPlot->graph(enum_int(OscillogChannels::OSCILLOG_I_B))->setBrush(QBrush(QColor(0, 0, 139, 100)));
    _ui->OscillogPlot->graph(enum_int(OscillogChannels::OSCILLOG_I_C))->setBrush(QBrush(QColor(0, 139, 0, 100)));

    _ui->OscillogPlot->graph(enum_int(OscillogChannels::OSCILLOG_UD))->setPen(QPen(qRgb(139, 0, 139)));

    _ui->OscillogPlot->graph(enum_int(OscillogChannels::OSCILLOG_COMP_A))->setPen(QPen(qRgb(255, 105, 180)));
    _ui->OscillogPlot->graph(enum_int(OscillogChannels::OSCILLOG_COMP_B))->setPen(QPen(qRgb(123, 104, 238)));
    _ui->OscillogPlot->graph(enum_int(OscillogChannels::OSCILLOG_COMP_C))->setPen(QPen(qRgb(95, 158, 160)));

    _ui->OscillogPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);

    _ui->OscillogPlot->yAxis2->setVisible(true);
    // give the axes some labels:
    _ui->OscillogPlot->xAxis->setLabel("мс");
    _ui->OscillogPlot->yAxis->setLabel("А");
    _ui->OscillogPlot->yAxis2->setLabel("В");
    // make left and bottom axes transfer their ranges to right and top axes:
    connect(_ui->OscillogPlot->xAxis, SIGNAL(rangeChanged(QCPRange)), _ui->OscillogPlot->xAxis2, SLOT(setRange(QCPRange)));

    _ui->OscillogPlot->graph(0)->rescaleAxes(true);

    _oscillog_xval.resize(128);

    for (uint i = 0; i < enum_int(OscillogChannels::OSCILLOG_SIZE); i++)
    {
        _oscillog_data[i].resize(OSCILLOG_TRANSFER_SIZE);
    }
    for (uint i = 0; i < OSCILLOG_TRANSFER_SIZE; i++)
    {
        _oscillog_xval[i] = (1.0 / 50.0 / 128.0) * static_cast<double>(i) * 1e3;
    }

    _ui->OscillogPlot->xAxis->setRange(0, 20.0 /*-20.0/128.0*/);
    _ui->OscillogPlot->yAxis2->setRange(-220 * sqrt(2) * 1.5, 220 * sqrt(2) * 1.5);
    _ui->OscillogPlot->yAxis->setRange(-1.2, 1.2);
    _ui->OscillogPlot->replot();

    connect(_ui->OscillogPlot->xAxis, static_cast<void(QCPAxis::*)(const QCPRange &, const QCPRange &)>(&QCPAxis::rangeChanged),
            this, &MainWindow::xAxisRangeChanged);
}
void MainWindow::xAxisRangeChanged(QCPRange newRange, QCPRange oldRange)
{
    Q_UNUSED(newRange)
    Q_UNUSED(oldRange)
    if (_ui->OscillogPlot->xAxis->range().lower <= 0)
    {
        _ui->OscillogPlot->xAxis->setRangeLower(0);
    }
    if (_ui->OscillogPlot->xAxis->range().upper >= 20)
    {
        _ui->OscillogPlot->xAxis->setRangeUpper(20);
    }
}
void MainWindow::mouseWheel(QWheelEvent *event)
{
    double factor;
    double wheelSteps = event->delta() / 120.0;  // a single step delta is +/-120 usually
    factor = qPow(_ui->OscillogPlot->axisRect()->rangeZoomFactor(Qt::Vertical), wheelSteps);
    _ui->OscillogPlot->yAxis2->scaleRange(factor, _ui->OscillogPlot->yAxis2->pixelToCoord(event->pos().y()));
}
void MainWindow::buttonAutoConfigOscClicked(void)
{
    _ui->OscillogPlot->rescaleAxes(true);
}
void MainWindow::setOscillog(PFCOscillogCnannel channel, std::vector<double> data)
{
    double K1 = (static_cast<double>(_ui->sliderOscFilter->value())) / 1000.0;
    //qDebug()<<data;
    if (channel <= PFCOscillogCnannel::OSC_COMP_C)
    {
        if (!_oscillog_array.contains(channel))
        {
            qDebug() << "Channel error 1";
            return;
        }
        for (uint i = 0; i < _oscillog_data[enum_uint(_oscillog_array[channel])].size(); i++)
        {
            _oscillog_data[enum_uint(_oscillog_array[channel])][i] =
                _oscillog_data[enum_uint(_oscillog_array[channel])][i] * K1 + data[i] * (1.0 - K1);
        }
        _ui->OscillogPlot->graph(enum_int(_oscillog_array[channel]))->setData(QVector<double>::fromStdVector(_oscillog_xval), QVector<double>::fromStdVector(_oscillog_data[enum_uint(_oscillog_array[channel])]));
        _ui->OscillogPlot->replot();
    }
    else
    {
        qDebug() << "Channel error 2";
    }
}
