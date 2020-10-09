#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "settingsdialog.h"
#include <QMessageBox>
#include <QMessageBox>
#include <QDateTime>
#include <QGraphicsDropShadowEffect>
#include <QListWidgetItem>

enum {
    table_filters_row_K_I,
    table_filters_row_K_U,
    table_filters_row_K_Ud
};

void MainWindow::pageSettingsFiltersInit(){
    for(int i=0;i<ui->tableWidget_settings_filters->rowCount();i++){
        ui->tableWidget_settings_filters->item(i, 0)->setFlags(Qt::ItemIsEnabled | Qt::ItemIsEditable);
        ui->tableWidget_settings_filters->item(i, 1)->setFlags(0);
        ui->tableWidget_settings_filters->item(i, 2)->setFlags(0);
        ui->tableWidget_settings_filters->item(i, 0)->setBackground(editableCellBrush);
    }
    connect(ui->tableWidget_settings_filters, SIGNAL(cellChanged(int,int)),
            this, SLOT(tableSettingsFilters_changed(int,int)));
}

void MainWindow::tableSettingsFilters_changed(int row, int col){
    QTableWidgetItem *item=ui->tableWidget_settings_filters->item(row,col);
    float val=item->text().toFloat();
    switch(row){       
        case table_filters_row_K_I:
            pfc_settings.SETTINGS.FILTERS.K_I=val;
        break;
        case table_filters_row_K_U:
            pfc_settings.SETTINGS.FILTERS.K_U=val;
        break;
        case table_filters_row_K_Ud:
            pfc_settings.SETTINGS.FILTERS.K_Ud=val;
        break;
    }
    writeSettingsFilters(
                pfc_settings.SETTINGS.FILTERS.K_I,
                pfc_settings.SETTINGS.FILTERS.K_U,
                pfc_settings.SETTINGS.FILTERS.K_Ud
                );
}
//========================================================================
void MainWindow::setSettingsFilters(
        float K_I,
        float K_U,
        float K_Ud
        ){
    pfc_settings.SETTINGS.FILTERS.K_I         =K_I;
    pfc_settings.SETTINGS.FILTERS.K_U         =K_U;
    pfc_settings.SETTINGS.FILTERS.K_Ud        =K_Ud;

#define SET_TABLE_FILTERS(VAL)\
    ui->tableWidget_settings_filters->item(table_filters_row_##VAL,0)->setText(QString().sprintf("%.3f",VAL));

    SET_TABLE_FILTERS(K_I);
    SET_TABLE_FILTERS(K_U);
    SET_TABLE_FILTERS(K_Ud);
}
