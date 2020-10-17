#ifndef PAGE_FILTERS_H
#define PAGE_FILTERS_H

#include <QtCore/QtGlobal>
#include "device_definition.h"
#include "device.h"
#include "ui_mainwindow.h"

class PageFilters: public QObject
{
    Q_OBJECT

private:
    Ui::MainWindow *_ui;
    PFCconfig::PFCsettings *_pfc_settings;
    PFC *_pfc;

public:
    explicit PageFilters(Ui::MainWindow *ui, PFCconfig::PFCsettings *pfc_settings, PFC *pfc);
    virtual ~PageFilters(void);

    enum class TableFiltersRows
    {
        table_filters_row_K_I,
        table_filters_row_K_U,
        table_filters_row_K_Ud
    };

    void pageSettingsFiltersInit(void);

public slots:
    void setSettingsFilters(
            float K_I,
            float K_U,
            float K_Ud);
    void ansSettingsFilters(bool writed);
    void tableSettingsFiltersChanged(int row, int col);
signals:
    void writeSettingsFilters(
            float K_I,
            float K_U,
            float K_Ud);
};


#endif // PAGE_FILTERS_H
