
#include "ui_mainwindow.h"
#include "settingsdialog.h"
#include "page_oscillog.h"
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

PageOscillog::PageOscillog(Ui::MainWindow *ui, PFCconfig::PFCsettings *pfc_settings, PFC *pfc):
    _ui(ui), _pfc_settings(pfc_settings), _pfc(pfc),
    _oscillog_data(static_cast<int>(DiagramOscillogChannels::OSCILLOG_SIZE))
{
}

PageOscillog::~PageOscillog(void)
{
}

void PageOscillog::pageOscillogInit(void)
{
    connect(_pfc, &PFC::setOscillog, this, &PageOscillog::setOscillog);
    connect(this, &PageOscillog::updateOscillog,
            _pfc, &PFC::updateOscillog);
    connect(_ui->buttonAutoConfigOsc, &QPushButton::clicked, this, &PageOscillog::buttonAutoConfigOscClicked);

    _oscillog_array[OscillogCnannel::OSC_U_A] = DiagramOscillogChannels::OSCILLOG_U_A;
    _oscillog_array[OscillogCnannel::OSC_U_B] = DiagramOscillogChannels::OSCILLOG_U_B;
    _oscillog_array[OscillogCnannel::OSC_U_C] = DiagramOscillogChannels::OSCILLOG_U_C;
    _oscillog_array[OscillogCnannel::OSC_I_A] = DiagramOscillogChannels::OSCILLOG_I_A;
    _oscillog_array[OscillogCnannel::OSC_I_B] = DiagramOscillogChannels::OSCILLOG_I_B;
    _oscillog_array[OscillogCnannel::OSC_I_C] = DiagramOscillogChannels::OSCILLOG_I_C;
    _oscillog_array[OscillogCnannel::OSC_UD] = DiagramOscillogChannels::OSCILLOG_UD;
    _oscillog_array[OscillogCnannel::OSC_COMP_A] = DiagramOscillogChannels::OSCILLOG_COMP_A;
    _oscillog_array[OscillogCnannel::OSC_COMP_B] = DiagramOscillogChannels::OSCILLOG_COMP_B;
    _oscillog_array[OscillogCnannel::OSC_COMP_C] = DiagramOscillogChannels::OSCILLOG_COMP_C;

    _ui->OscillogPlot->addGraph(_ui->OscillogPlot->xAxis, _ui->OscillogPlot->yAxis)->setName("I A");
    _ui->OscillogPlot->addGraph(_ui->OscillogPlot->xAxis, _ui->OscillogPlot->yAxis)->setName("I B");
    _ui->OscillogPlot->addGraph(_ui->OscillogPlot->xAxis, _ui->OscillogPlot->yAxis)->setName("I C");

    _ui->OscillogPlot->addGraph(_ui->OscillogPlot->xAxis, _ui->OscillogPlot->yAxis2)->setName("U A");
    _ui->OscillogPlot->addGraph(_ui->OscillogPlot->xAxis, _ui->OscillogPlot->yAxis2)->setName("U B");
    _ui->OscillogPlot->addGraph(_ui->OscillogPlot->xAxis, _ui->OscillogPlot->yAxis2)->setName("U C");

    _ui->OscillogPlot->addGraph(_ui->OscillogPlot->xAxis, _ui->OscillogPlot->yAxis2)->setName("U cap");

    _ui->OscillogPlot->addGraph(_ui->OscillogPlot->xAxis, _ui->OscillogPlot->yAxis)->setName("Icomp A");
    _ui->OscillogPlot->addGraph(_ui->OscillogPlot->xAxis, _ui->OscillogPlot->yAxis)->setName("Icomp B");
    _ui->OscillogPlot->addGraph(_ui->OscillogPlot->xAxis, _ui->OscillogPlot->yAxis)->setName("Icomp C");

    _ui->OscillogPlot->graph(enum_int(DiagramOscillogChannels::OSCILLOG_U_A))->setPen(QPen(Qt::red));
    _ui->OscillogPlot->graph(enum_int(DiagramOscillogChannels::OSCILLOG_U_B))->setPen(QPen(Qt::blue));
    _ui->OscillogPlot->graph(enum_int(DiagramOscillogChannels::OSCILLOG_U_C))->setPen(QPen(Qt::green));

    _ui->OscillogPlot->graph(enum_int(DiagramOscillogChannels::OSCILLOG_I_A))->setPen(QPen(QColor(Qt::darkRed).lighter(200)));
    _ui->OscillogPlot->graph(enum_int(DiagramOscillogChannels::OSCILLOG_I_B))->setPen(QPen(QColor(Qt::darkBlue).lighter(200)));
    _ui->OscillogPlot->graph(enum_int(DiagramOscillogChannels::OSCILLOG_I_C))->setPen(QPen(QColor(Qt::darkGreen).lighter(200)));
    _ui->OscillogPlot->graph(enum_int(DiagramOscillogChannels::OSCILLOG_I_A))->setBrush(QBrush(QColor(139, 0, 0, 100)));
    _ui->OscillogPlot->graph(enum_int(DiagramOscillogChannels::OSCILLOG_I_B))->setBrush(QBrush(QColor(0, 0, 139, 100)));
    _ui->OscillogPlot->graph(enum_int(DiagramOscillogChannels::OSCILLOG_I_C))->setBrush(QBrush(QColor(0, 139, 0, 100)));

    _ui->OscillogPlot->graph(enum_int(DiagramOscillogChannels::OSCILLOG_UD))->setPen(QPen(qRgb(139, 0, 139)));

    _ui->OscillogPlot->graph(enum_int(DiagramOscillogChannels::OSCILLOG_COMP_A))->setPen(QPen(qRgb(255, 105, 180)));
    _ui->OscillogPlot->graph(enum_int(DiagramOscillogChannels::OSCILLOG_COMP_B))->setPen(QPen(qRgb(123, 104, 238)));
    _ui->OscillogPlot->graph(enum_int(DiagramOscillogChannels::OSCILLOG_COMP_C))->setPen(QPen(qRgb(95, 158, 160)));

    _ui->OscillogPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);

    _ui->OscillogPlot->yAxis2->setVisible(true);
    // give the axes some labels:
    _ui->OscillogPlot->xAxis->setLabel("ms");
    _ui->OscillogPlot->yAxis->setLabel("А");
    _ui->OscillogPlot->yAxis2->setLabel("В");
    // make left and bottom axes transfer their ranges to right and top axes:
    connect(_ui->OscillogPlot->xAxis, SIGNAL(rangeChanged(QCPRange)), _ui->OscillogPlot->xAxis2, SLOT(setRange(QCPRange)));

    _ui->OscillogPlot->graph(0)->rescaleAxes(true);

    _oscillog_xval.resize(128);

    for (uint i = 0; i < enum_int(DiagramOscillogChannels::OSCILLOG_SIZE); i++)
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
            this, &PageOscillog::xAxisRangeChanged);
}

void PageOscillog::xAxisRangeChanged(QCPRange newRange, QCPRange oldRange)
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

void PageOscillog::mouseWheel(QWheelEvent *event)
{
    double factor;
    double wheelSteps = event->delta() / 120.0;  // a single step delta is +/-120 usually
    factor = qPow(_ui->OscillogPlot->axisRect()->rangeZoomFactor(Qt::Vertical), wheelSteps);
    _ui->OscillogPlot->yAxis2->scaleRange(factor, _ui->OscillogPlot->yAxis2->pixelToCoord(event->pos().y()));
}

void PageOscillog::buttonAutoConfigOscClicked()
{
    _ui->OscillogPlot->rescaleAxes(true);
}

void PageOscillog::setOscillog(OscillogCnannel channel, std::vector<double> data)
{
    double K1 = (static_cast<double>(_ui->sliderOscFilter->value())) / 1000.0;
    //qDebug()<<data;
    if (channel <= OscillogCnannel::OSC_COMP_C)
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

void PageOscillog::update(void)
{
    if (_ui->checkOscIa->isChecked()) emit updateOscillog(OscillogCnannel::OSC_I_A);
    if (_ui->checkOscIb->isChecked()) emit updateOscillog(OscillogCnannel::OSC_I_B);
    if (_ui->checkOscIc->isChecked()) emit updateOscillog(OscillogCnannel::OSC_I_C);

    if (_ui->checkOscUa->isChecked()) emit updateOscillog(OscillogCnannel::OSC_U_A);
    if (_ui->checkOscUb->isChecked()) emit updateOscillog(OscillogCnannel::OSC_U_B);
    if (_ui->checkOscUc->isChecked()) emit updateOscillog(OscillogCnannel::OSC_U_C);

    if (_ui->checkOscUd->isChecked()) emit updateOscillog(OscillogCnannel::OSC_UD);

    if (_ui->checkOscICompA->isChecked()) emit updateOscillog(OscillogCnannel::OSC_COMP_A);
    if (_ui->checkOscICompB->isChecked()) emit updateOscillog(OscillogCnannel::OSC_COMP_B);
    if (_ui->checkOscICompC->isChecked()) emit updateOscillog(OscillogCnannel::OSC_COMP_C);

    _ui->OscillogPlot->graph(static_cast<int>(DiagramOscillogChannels::OSCILLOG_I_A))->setVisible(_ui->checkOscIa->isChecked());
    _ui->OscillogPlot->graph(static_cast<int>(DiagramOscillogChannels::OSCILLOG_I_B))->setVisible(_ui->checkOscIb->isChecked());
    _ui->OscillogPlot->graph(static_cast<int>(DiagramOscillogChannels::OSCILLOG_I_C))->setVisible(_ui->checkOscIc->isChecked());

    _ui->OscillogPlot->graph(static_cast<int>(DiagramOscillogChannels::OSCILLOG_U_A))->setVisible(_ui->checkOscUa->isChecked());
    _ui->OscillogPlot->graph(static_cast<int>(DiagramOscillogChannels::OSCILLOG_U_B))->setVisible(_ui->checkOscUb->isChecked());
    _ui->OscillogPlot->graph(static_cast<int>(DiagramOscillogChannels::OSCILLOG_U_C))->setVisible(_ui->checkOscUc->isChecked());

    _ui->OscillogPlot->graph(static_cast<int>(DiagramOscillogChannels::OSCILLOG_UD))->setVisible(_ui->checkOscUd->isChecked());

    _ui->OscillogPlot->graph(static_cast<int>(DiagramOscillogChannels::OSCILLOG_COMP_A))->setVisible(_ui->checkOscICompA->isChecked());
    _ui->OscillogPlot->graph(static_cast<int>(DiagramOscillogChannels::OSCILLOG_COMP_B))->setVisible(_ui->checkOscICompB->isChecked());
    _ui->OscillogPlot->graph(static_cast<int>(DiagramOscillogChannels::OSCILLOG_COMP_C))->setVisible(_ui->checkOscICompC->isChecked());
}
