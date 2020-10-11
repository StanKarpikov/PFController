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

void MainWindow::pageSettingsFiltersInit()
{
    for (int i = 0; i < _ui->tableWidget_settings_filters->rowCount(); i++)
    {
        _ui->tableWidget_settings_filters->item(i, 0)->setFlags(Qt::ItemIsEnabled | Qt::ItemIsEditable);
        _ui->tableWidget_settings_filters->item(i, 1)->setFlags(Q_NULLPTR);
        _ui->tableWidget_settings_filters->item(i, 2)->setFlags(Q_NULLPTR);
        _ui->tableWidget_settings_filters->item(i, 0)->setBackground(editableCellBrush);
    }
    connect(_ui->tableWidget_settings_filters, SIGNAL(cellChanged(int, int)),
            this, SLOT(tableSettingsFiltersChanged(int, int)));
}

void MainWindow::tableSettingsFiltersChanged(int row, int col)
{
    QTableWidgetItem *item = _ui->tableWidget_settings_filters->item(row, col);
    float value = item->text().toFloat();
    switch (static_cast<TableFiltersRows>(row))
    {
        case TableFiltersRows::table_filters_row_K_I:
            _pfc_settings.SETTINGS.FILTERS.K_I = value;
            break;
        case TableFiltersRows::table_filters_row_K_U:
            _pfc_settings.SETTINGS.FILTERS.K_U = value;
            break;
        case TableFiltersRows::table_filters_row_K_Ud:
            _pfc_settings.SETTINGS.FILTERS.K_Ud = value;
            break;
    }
    writeSettingsFilters(
        _pfc_settings.SETTINGS.FILTERS.K_I,
        _pfc_settings.SETTINGS.FILTERS.K_U,
        _pfc_settings.SETTINGS.FILTERS.K_Ud);
}

void MainWindow::setSettingsFilters(
    float K_I,
    float K_U,
    float K_Ud)
{
    _pfc_settings.SETTINGS.FILTERS.K_I = K_I;
    _pfc_settings.SETTINGS.FILTERS.K_U = K_U;
    _pfc_settings.SETTINGS.FILTERS.K_Ud = K_Ud;

    _ui->tableWidget_settings_filters->item(enum_int(TableFiltersRows::table_filters_row_K_I), 0)->setText(QString().sprintf("%.3f", static_cast<double>(K_I)));
    _ui->tableWidget_settings_filters->item(enum_int(TableFiltersRows::table_filters_row_K_U), 0)->setText(QString().sprintf("%.3f", static_cast<double>(K_U)));
    _ui->tableWidget_settings_filters->item(enum_int(TableFiltersRows::table_filters_row_K_Ud), 0)->setText(QString().sprintf("%.3f", static_cast<double>(K_Ud)));
}
