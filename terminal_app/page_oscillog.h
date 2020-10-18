#ifndef PAGE_OSCILLOG_H
#define PAGE_OSCILLOG_H

#include <QtCore/QtGlobal>
#include "device_definition.h"
#include "device.h"
#include "ui_mainwindow.h"

class PageOscillog: public QObject
{
    Q_OBJECT

private:
    enum class DiagramOscillogChannels
    {
        OSCILLOG_I_A,
        OSCILLOG_I_B,
        OSCILLOG_I_C,

        OSCILLOG_U_A,
        OSCILLOG_U_B,
        OSCILLOG_U_C,

        OSCILLOG_UD,

        OSCILLOG_COMP_A,
        OSCILLOG_COMP_B,
        OSCILLOG_COMP_C,

        OSCILLOG_SIZE
    };

    Ui::MainWindow *_ui;
    PFCconfig::PFCsettings *_pfc_settings;
    PFC *_pfc;
    std::vector<double> _oscillog_xval, _harmonics_xval;
    std::vector<std::vector<double>> _oscillog_data;
    QMap<PFCconfig::Interface::OscillogCnannel,DiagramOscillogChannels> _oscillog_array;
    QSharedPointer<QCPAxisTickerFixed> _fixed_ticker;

public:
    explicit PageOscillog(Ui::MainWindow *ui, PFCconfig::PFCsettings *pfc_settings, PFC *pfc);
    virtual ~PageOscillog(void);
    void pageOscillogInit(void);

private slots:
    void xAxisRangeChanged( QCPRange newRange ,QCPRange oldRange);
    void mouseWheel(QWheelEvent *event);

public slots:
    void buttonAutoConfigOscClicked();
    void setOscillog(PFCconfig::Interface::OscillogCnannel channel, std::vector<double> data);
    void update(void);

signals:
    void updateOscillog(PFCconfig::Interface::OscillogCnannel channel);
};

#endif // PAGE_OSCILLOG_H
