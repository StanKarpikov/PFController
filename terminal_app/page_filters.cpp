#include "page_filters.h"
#include "ui_mainwindow.h"
#include "settingsdialog.h"
#include "interface_definitions.h"
#include <QMessageBox>
#include <QMessageBox>
#include <QDateTime>
#include <QGraphicsDropShadowEffect>
#include <QListWidgetItem>

/*--------------------------------------------------------------
                       NAMESPACES
--------------------------------------------------------------*/

using namespace PFCconfig;
using namespace PFCconfig::ADC;
using namespace PFCconfig::Interface;
using namespace PFCconfig::Events;
using namespace InterfaceDefinitions;

/*--------------------------------------------------------------
                       PRIVATE FUNCTIONS
--------------------------------------------------------------*/

PageFilters::PageFilters(Ui::MainWindow *ui, PFCconfig::PFCsettings *pfc_settings, PFC *pfc):
    _ui(ui), _pfc_settings(pfc_settings), _pfc(pfc)
{
    connect(_pfc, &PFC::setSettingsFilters,
            this, &PageFilters::setSettingsFilters);
    connect(_pfc, &PFC::ansSettingsFilters,
            this, &PageFilters::ansSettingsFilters);
    connect(this, &PageFilters::writeSettingsFilters,
            _pfc, &PFC::writeSettingsFilters);
}

PageFilters::~PageFilters(void)
{

}

void PageFilters::pageSettingsFiltersInit(void)
{
    for (int i = 0; i < _ui->tableSettingsFilters->rowCount(); i++)
    {
        _ui->tableSettingsFilters->item(i, 0)->setFlags(Qt::ItemIsEnabled | Qt::ItemIsEditable);
        _ui->tableSettingsFilters->item(i, 1)->setFlags(Q_NULLPTR);
        _ui->tableSettingsFilters->item(i, 2)->setFlags(Q_NULLPTR);
        _ui->tableSettingsFilters->item(i, 0)->setBackground(editableCellBrush);
    }
    QObject::connect(_ui->tableSettingsFilters, &QTableWidget::cellChanged,
            this, &PageFilters::tableSettingsFiltersChanged);
}

void PageFilters::tableSettingsFiltersChanged(int row, int col)
{
    QTableWidgetItem *item = _ui->tableSettingsFilters->item(row, col);
    float value = item->text().toFloat();
    switch (static_cast<TableFiltersRows>(row))
    {
        case TableFiltersRows::table_filters_row_K_I:
            _pfc_settings->settings.filters.K_I = value;
            break;
        case TableFiltersRows::table_filters_row_K_U:
            _pfc_settings->settings.filters.K_U = value;
            break;
        case TableFiltersRows::table_filters_row_K_Ud:
            _pfc_settings->settings.filters.K_Ud = value;
            break;
    }
    writeSettingsFilters(
        _pfc_settings->settings.filters.K_I,
        _pfc_settings->settings.filters.K_U,
        _pfc_settings->settings.filters.K_Ud);
}

void PageFilters::setSettingsFilters(
    float K_I,
    float K_U,
    float K_Ud)
{
    _pfc_settings->settings.filters.K_I = K_I;
    _pfc_settings->settings.filters.K_U = K_U;
    _pfc_settings->settings.filters.K_Ud = K_Ud;

    _ui->tableSettingsFilters->item(enum_int(TableFiltersRows::table_filters_row_K_I), 0)->setText(QString().sprintf("%.3f", static_cast<double>(K_I)));
    _ui->tableSettingsFilters->item(enum_int(TableFiltersRows::table_filters_row_K_U), 0)->setText(QString().sprintf("%.3f", static_cast<double>(K_U)));
    _ui->tableSettingsFilters->item(enum_int(TableFiltersRows::table_filters_row_K_Ud), 0)->setText(QString().sprintf("%.3f", static_cast<double>(K_Ud)));
}

void PageFilters::ansSettingsFilters(bool writed)
{
    Q_UNUSED(writed)
}
