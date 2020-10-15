#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "settingsdialog.h"
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

/*--------------------------------------------------------------
                       PRIVATE FUNCTIONS
--------------------------------------------------------------*/

void MainWindow::pageSettingsFiltersInit()
{
    for (int i = 0; i < _ui->tableSettingsFilters->rowCount(); i++)
    {
        _ui->tableSettingsFilters->item(i, 0)->setFlags(Qt::ItemIsEnabled | Qt::ItemIsEditable);
        _ui->tableSettingsFilters->item(i, 1)->setFlags(Q_NULLPTR);
        _ui->tableSettingsFilters->item(i, 2)->setFlags(Q_NULLPTR);
        _ui->tableSettingsFilters->item(i, 0)->setBackground(editableCellBrush);
    }
    connect(_ui->tableSettingsFilters, &QTableWidget::cellChanged,
            this, &MainWindow::tableSettingsFiltersChanged);
}

void MainWindow::tableSettingsFiltersChanged(int row, int col)
{
    QTableWidgetItem *item = _ui->tableSettingsFilters->item(row, col);
    float value = item->text().toFloat();
    switch (static_cast<TableFiltersRows>(row))
    {
        case TableFiltersRows::table_filters_row_K_I:
            _pfc_settings.settings.filters.K_I = value;
            break;
        case TableFiltersRows::table_filters_row_K_U:
            _pfc_settings.settings.filters.K_U = value;
            break;
        case TableFiltersRows::table_filters_row_K_Ud:
            _pfc_settings.settings.filters.K_Ud = value;
            break;
    }
    writeSettingsFilters(
        _pfc_settings.settings.filters.K_I,
        _pfc_settings.settings.filters.K_U,
        _pfc_settings.settings.filters.K_Ud);
}

void MainWindow::setSettingsFilters(
    float K_I,
    float K_U,
    float K_Ud)
{
    _pfc_settings.settings.filters.K_I = K_I;
    _pfc_settings.settings.filters.K_U = K_U;
    _pfc_settings.settings.filters.K_Ud = K_Ud;

    _ui->tableSettingsFilters->item(enum_int(TableFiltersRows::table_filters_row_K_I), 0)->setText(QString().sprintf("%.3f", static_cast<double>(K_I)));
    _ui->tableSettingsFilters->item(enum_int(TableFiltersRows::table_filters_row_K_U), 0)->setText(QString().sprintf("%.3f", static_cast<double>(K_U)));
    _ui->tableSettingsFilters->item(enum_int(TableFiltersRows::table_filters_row_K_Ud), 0)->setText(QString().sprintf("%.3f", static_cast<double>(K_Ud)));
}
