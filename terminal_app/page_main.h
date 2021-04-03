#ifndef PAGE_MAIN_H
#define PAGE_MAIN_H

#include <QtCore/QtGlobal>
#include "device_definition.h"
#include "device.h"
#include "ui_mainwindow.h"

class PageMain: public QObject
{
    Q_OBJECT

private:
    Ui::MainWindow *_ui;
    PFCconfig::PFCsettings *_pfc_settings;
    PFC *_pfc;

public:
    explicit PageMain(Ui::MainWindow *ui, PFCconfig::PFCsettings *pfc_settings, PFC *pfc);
    virtual ~PageMain(void);
    void pageMainInit(void);
    void update(void);

public slots:
    void setWorkState(uint32_t state, uint32_t ch_a, uint32_t ch_b, uint32_t ch_c);
    void setConnection(bool connected);
    void setVersionInfo(
            uint32_t major,
            uint32_t minor,
            uint32_t micro,
            uint32_t build,
            uint32_t day,
            uint32_t month,
            uint32_t year,
            uint32_t hour,
            uint32_t minute,
            uint32_t second);
    void updateCheckboxVal(QCheckBox* checkbox, bool value);

signals:
    void updateWorkState(uint64_t currentTime);
    void updateVersionInfo();
};

#endif // PAGE_MAIN_H
