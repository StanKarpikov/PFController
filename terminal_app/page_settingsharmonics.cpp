#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "settingsdialog.h"
#include <QMessageBox>
#include <QMessageBox>
#include <QDateTime>
#include <QGraphicsDropShadowEffect>
#include <QListWidgetItem>
#include <QCheckBox>

enum
{
    table_harmonics_row_harmonics_Amp_Kp,
    table_harmonics_row_harmonics_Amp_Ki,
    table_harmonics_row_harmonics_Amp_Kd,
    table_harmonics_row_harmonics_Phase_Kp,
    table_harmonics_row_harmonics_Phase_Ki,
    table_harmonics_row_harmonics_Phase_Kd,
    table_harmonics_row_limit_harmonics_Kamp_UP,
    table_harmonics_row_limit_harmonics_Kamp_DOWN,
    table_harmonics_row_harmonics_1_Kp,
    table_harmonics_row_harmonics_1_Ki,
    table_harmonics_row_harmonics_1_Kd,
    table_harmonics_row_limit_harmonics_1_K,
    table_harmonics_row_test_pid_I_ampl,
    table_harmonics_row_test_pid_I_phase,
    table_harmonics_row_test_ampl,
    table_harmonics_row_test_time,
    table_harmonics_row_ampl_to_duty_cycle,
    table_harmonics_row_suppress_all,
    table_harmonics_row_suppress_H0
};

QVector<QCheckBox *> check_H;

void MainWindow::page_SettingsHarmonics_Init()
{
    check_H = {
        0,
        ui->check_H1,
        ui->check_H2,
        ui->check_H3,
        ui->check_H4,
        ui->check_H5,
        ui->check_H6,
        ui->check_H7,
        ui->check_H8,
        ui->check_H9,
        ui->check_H10,
        ui->check_H11,
        ui->check_H12,
        ui->check_H13,
        ui->check_H14,
        ui->check_H15,
        ui->check_H16,
        ui->check_H17,
        ui->check_H18,
        ui->check_H19,
        ui->check_H20,
        ui->check_H21,
        ui->check_H22,
        ui->check_H23,
        ui->check_H24,
        ui->check_H25,
        ui->check_H26,
        ui->check_H27,
        ui->check_H28,
        ui->check_H29,
        ui->check_H30,
        ui->check_H31,
        ui->check_H32,
        ui->check_H33,
        ui->check_H34,
        ui->check_H35,
        ui->check_H36,
        ui->check_H37,
        ui->check_H38,
        ui->check_H39,
        ui->check_H40,
        ui->check_H41,
        ui->check_H42,
        ui->check_H43,
        ui->check_H44,
        ui->check_H45,
        ui->check_H46,
        ui->check_H47,
        ui->check_H48,
        ui->check_H49,
        ui->check_H50};

    for (int i = 0; i < ui->tableWidget_settings_harmonics->rowCount(); i++)
    {
        ui->tableWidget_settings_harmonics->item(i, 0)->setFlags(Qt::ItemIsEnabled | Qt::ItemIsEditable);
        ui->tableWidget_settings_harmonics->item(i, 0)->setBackground(editableCellBrush);
        ui->tableWidget_settings_harmonics->item(i, 1)->setFlags(0);
        //ui->tableWidget_settings_harmonics->item(i, 2)->setFlags(0);
    }

    connect(ui->tableWidget_settings_harmonics, SIGNAL(cellChanged(int, int)),
            this, SLOT(tableSettingsHarmonics_changed(int, int)));
}

void MainWindow::tableSettingsHarmonics_changed(int row, int col)
{
    QTableWidgetItem *item = ui->tableWidget_settings_harmonics->item(row, col);
    float val = item->text().toFloat();
    int PID = 0, SUPPR = 0;

#define CASEROW(CASE)                 \
    case table_harmonics_row_##CASE:  \
        ADF_var.HARMONICS.CASE = val; \
        PID = 1;                      \
        break;

    switch (row)
    {
        CASEROW(harmonics_Amp_Kp)
        CASEROW(harmonics_Amp_Ki)
        CASEROW(harmonics_Amp_Kd)
        CASEROW(harmonics_Phase_Kp)
        CASEROW(harmonics_Phase_Ki)
        CASEROW(harmonics_Phase_Kd)
        CASEROW(limit_harmonics_Kamp_UP)
        CASEROW(limit_harmonics_Kamp_DOWN)
        CASEROW(harmonics_1_Kp)
        CASEROW(harmonics_1_Ki)
        CASEROW(harmonics_1_Kd)
        CASEROW(limit_harmonics_1_K)
        CASEROW(test_pid_I_ampl)
        CASEROW(test_pid_I_phase)
        CASEROW(test_ampl)
        CASEROW(test_time)
        CASEROW(ampl_to_duty_cycle)
        case table_harmonics_row_suppress_all:
            //if(val>1)val=1;
            KKM_var.HARMONICS.fft_suppression.fill(val);
            SUPPR = 1;
            break;
        default:
            //if(val>1)val=1;
            KKM_var.HARMONICS.fft_suppression[row - table_harmonics_row_suppress_H0] = val;
            SUPPR = 1;
            break;
    }
    if (PID) writeSettingsHarmonicsPID(
        KKM_var.HARMONICS.harmonics_Amp_Kp,
        KKM_var.HARMONICS.harmonics_Amp_Ki,
        KKM_var.HARMONICS.harmonics_Amp_Kd,

        KKM_var.HARMONICS.harmonics_Phase_Kp,
        KKM_var.HARMONICS.harmonics_Phase_Ki,
        KKM_var.HARMONICS.harmonics_Phase_Kd,

        KKM_var.HARMONICS.harmonics_1_Kp,
        KKM_var.HARMONICS.harmonics_1_Ki,
        KKM_var.HARMONICS.harmonics_1_Kd,

        KKM_var.HARMONICS.CompensationPriority,

        KKM_var.HARMONICS.limit_harmonics_1_K,
        KKM_var.HARMONICS.limit_harmonics_Kamp_UP,
        KKM_var.HARMONICS.limit_harmonics_Kamp_DOWN,

        KKM_var.HARMONICS.ampl_to_duty_cycle,
        KKM_var.HARMONICS.test_ampl,
        KKM_var.HARMONICS.test_time,
        KKM_var.HARMONICS.test_pid_I_phase,
        KKM_var.HARMONICS.test_pid_I_ampl);
    if (SUPPR) writeSettingsHarmonicsSuppression(KKM_var.HARMONICS.fft_suppression);
}
//========================================================================

void MainWindow::HarmonicsFFTSuppressionUpdate()
{
    ui->tableWidget_settings_harmonics->blockSignals(true);

    for (int i = 0; i < HARMONICS_NUM; i++)
    {
        if (ui->tableWidget_settings_harmonics->currentRow() != table_harmonics_row_suppress_H0 + i)
        {
            ui->tableWidget_settings_harmonics->item(
                                                  table_harmonics_row_suppress_H0 + i, 0)
                ->setText(QString().sprintf("%.3f", KKM_var.HARMONICS.fft_suppression[i]));
        }
        //SET_TABLE_HARMA(suppress_H0+i,ADF_var.HARMONICS.fft_suppression[i]);
    }
    ui->tableWidget_settings_harmonics->blockSignals(false);

    int check_H1_50 = 0;
    int check_H1_10 = 0;
    int check_H11_20 = 0;
    int check_H21_30 = 0;
    int check_H31_40 = 0;
    int check_H41_50 = 0;

    for (int i = 1; i < HARMONICS_NUM; i++)
    {
        check_H[i]->blockSignals(true);
        if (KKM_var.HARMONICS.fft_suppression[i] != 0.0)
        {
            check_H[i]->setChecked(true);
            check_H1_50++;
            if (i > 0 && i <= 10) check_H1_10++;
            if (i > 10 && i <= 20) check_H11_20++;
            if (i > 20 && i <= 30) check_H21_30++;
            if (i > 30 && i <= 40) check_H31_40++;
            if (i > 40 && i <= 50) check_H41_50++;
        }
        else
        {
            check_H[i]->setChecked(false);
        }
        check_H[i]->blockSignals(false);
    }

    ui->check_H1_50->blockSignals(true);
    if (check_H1_50 == 50)
    {
        ui->check_H1_50->setCheckState(Qt::Checked);
    }
    else if (check_H1_50 == 0)
    {
        ui->check_H1_50->setCheckState(Qt::Unchecked);
    }
    else
    {
        ui->check_H1_50->setCheckState(Qt::PartiallyChecked);
    }
    ui->check_H1_50->blockSignals(false);

    ui->check_H1_10->blockSignals(true);
    if (check_H1_10 == 10)
    {
        ui->check_H1_10->setChecked(true);
    }
    else if (check_H1_10 == 0)
    {
        ui->check_H1_10->setChecked(false);
    }
    else
    {
        ui->check_H1_10->setChecked(true);
        ui->check_H1_10->setCheckState(Qt::PartiallyChecked);
    }
    ui->check_H1_10->blockSignals(false);

    ui->check_H11_20->blockSignals(true);
    if (check_H11_20 == 10)
    {
        ui->check_H11_20->setChecked(true);
    }
    else if (check_H11_20 == 0)
    {
        ui->check_H11_20->setChecked(false);
    }
    else
    {
        ui->check_H11_20->setChecked(true);
        ui->check_H11_20->setCheckState(Qt::PartiallyChecked);
    }
    ui->check_H11_20->blockSignals(false);

    ui->check_H21_30->blockSignals(true);
    if (check_H21_30 == 10)
    {
        ui->check_H21_30->setChecked(true);
    }
    else if (check_H21_30 == 0)
    {
        ui->check_H21_30->setChecked(false);
    }
    else
    {
        ui->check_H21_30->setChecked(true);
        ui->check_H21_30->setCheckState(Qt::PartiallyChecked);
    }
    ui->check_H21_30->blockSignals(false);

    ui->check_H31_40->blockSignals(true);
    if (check_H31_40 == 10)
    {
        ui->check_H31_40->setChecked(true);
    }
    else if (check_H31_40 == 0)
    {
        ui->check_H31_40->setChecked(false);
    }
    else
    {
        ui->check_H31_40->setChecked(true);
        ui->check_H31_40->setCheckState(Qt::PartiallyChecked);
    }
    ui->check_H31_40->blockSignals(false);

    ui->check_H41_50->blockSignals(true);
    if (check_H41_50 == 10)
    {
        ui->check_H41_50->setChecked(true);
    }
    else if (check_H41_50 == 0)
    {
        ui->check_H41_50->setChecked(false);
    }
    else
    {
        ui->check_H41_50->setChecked(true);
        ui->check_H41_50->setCheckState(Qt::PartiallyChecked);
    }
    ui->check_H41_50->blockSignals(false);
}

void MainWindow::setSettingsHarmonicsSuppression(
    QVector<float> fft_suppress)
{
    KKM_var.HARMONICS.fft_suppression = fft_suppress;
    HarmonicsFFTSuppressionUpdate();
}
#define UPDATE_ELEMENT(ELEM, VAL)  \
    if (!ELEM->hasFocus())         \
    {                              \
        ELEM->blockSignals(true);  \
        ELEM->setValue(VAL);       \
        ELEM->blockSignals(false); \
    }
void MainWindow::setSettingsHarmonicsPID(
    float harmonics_Amp_Kp,  //Амплитуда 2..50
    float harmonics_Amp_Ki,
    float harmonics_Amp_Kd,
    float harmonics_Phase_Kp,  //Фаза 2..50
    float harmonics_Phase_Ki,
    float harmonics_Phase_Kd,
    float harmonics_1_Kp,  //Для 1 гармоники
    float harmonics_1_Ki,
    float harmonics_1_Kd,
    float CompensationPriority,
    float limit_harmonics_1_K,
    float limit_harmonics_Kamp_UP,
    float limit_harmonics_Kamp_DOWN,
    float ampl_to_duty_cycle,
    float test_ampl,
    uint32_t test_time,
    float test_pid_I_phase,
    float test_pid_I_ampl)
{
    KKM_var.HARMONICS.harmonics_Amp_Kp = harmonics_Amp_Kp;
    KKM_var.HARMONICS.harmonics_Amp_Ki = harmonics_Amp_Ki;
    KKM_var.HARMONICS.harmonics_Amp_Kd = harmonics_Amp_Kd;

    KKM_var.HARMONICS.harmonics_Phase_Kp = harmonics_Phase_Kp;
    KKM_var.HARMONICS.harmonics_Phase_Ki = harmonics_Phase_Ki;
    KKM_var.HARMONICS.harmonics_Phase_Kd = harmonics_Phase_Kd;

    KKM_var.HARMONICS.harmonics_1_Kp = harmonics_1_Kp;
    KKM_var.HARMONICS.harmonics_1_Ki = harmonics_1_Ki;
    KKM_var.HARMONICS.harmonics_1_Kd = harmonics_1_Kd;

    KKM_var.HARMONICS.CompensationPriority = CompensationPriority;

    KKM_var.HARMONICS.limit_harmonics_1_K = limit_harmonics_1_K;
    KKM_var.HARMONICS.limit_harmonics_Kamp_UP = limit_harmonics_Kamp_UP;
    KKM_var.HARMONICS.limit_harmonics_Kamp_DOWN = limit_harmonics_Kamp_DOWN;

    KKM_var.HARMONICS.ampl_to_duty_cycle = ampl_to_duty_cycle;
    KKM_var.HARMONICS.test_ampl = test_ampl;
    KKM_var.HARMONICS.test_time = test_time;
    KKM_var.HARMONICS.test_pid_I_phase = test_pid_I_phase;
    KKM_var.HARMONICS.test_pid_I_ampl = test_pid_I_ampl;

#define SET_TABLE_HARM(VAL)                                                                                              \
    if (ui->tableWidget_settings_harmonics->currentRow() != table_harmonics_row_##VAL)                                   \
    {                                                                                                                    \
        ui->tableWidget_settings_harmonics->item(table_harmonics_row_##VAL, 0)->setText(QString().sprintf("%.3f", VAL)); \
    }

    ui->tableWidget_settings_harmonics->blockSignals(true);
    SET_TABLE_HARM(harmonics_Amp_Kp)
    SET_TABLE_HARM(harmonics_Amp_Ki)
    SET_TABLE_HARM(harmonics_Amp_Kd)
    SET_TABLE_HARM(harmonics_Phase_Kp)
    SET_TABLE_HARM(harmonics_Phase_Ki)
    SET_TABLE_HARM(harmonics_Phase_Kd)
    SET_TABLE_HARM(limit_harmonics_Kamp_UP)
    SET_TABLE_HARM(limit_harmonics_Kamp_DOWN)
    SET_TABLE_HARM(harmonics_1_Kp)
    SET_TABLE_HARM(harmonics_1_Ki)
    SET_TABLE_HARM(harmonics_1_Kd)
    SET_TABLE_HARM(limit_harmonics_1_K)
    SET_TABLE_HARM(test_pid_I_ampl)
    SET_TABLE_HARM(test_pid_I_phase)
    SET_TABLE_HARM(test_ampl)
    if (ui->tableWidget_settings_harmonics->currentRow() != table_harmonics_row_test_time)
    {
        ui->tableWidget_settings_harmonics->item(table_harmonics_row_test_time, 0)->setText(QString().sprintf("%i", test_time));
    }
    SET_TABLE_HARM(ampl_to_duty_cycle)
    ui->tableWidget_settings_harmonics->blockSignals(false);

    UPDATE_ELEMENT(ui->horizontalSlider, KKM_var.HARMONICS.CompensationPriority * 100.0);
}

void MainWindow::on_horizontalSlider_valueChanged(int value)
{
    KKM_var.HARMONICS.CompensationPriority = value / 100.0;

    writeSettingsHarmonicsPID(
        KKM_var.HARMONICS.harmonics_Amp_Kp,
        KKM_var.HARMONICS.harmonics_Amp_Ki,
        KKM_var.HARMONICS.harmonics_Amp_Kd,

        KKM_var.HARMONICS.harmonics_Phase_Kp,
        KKM_var.HARMONICS.harmonics_Phase_Ki,
        KKM_var.HARMONICS.harmonics_Phase_Kd,

        KKM_var.HARMONICS.harmonics_1_Kp,
        KKM_var.HARMONICS.harmonics_1_Ki,
        KKM_var.HARMONICS.harmonics_1_Kd,

        KKM_var.HARMONICS.CompensationPriority,

        KKM_var.HARMONICS.limit_harmonics_1_K,
        KKM_var.HARMONICS.limit_harmonics_Kamp_UP,
        KKM_var.HARMONICS.limit_harmonics_Kamp_DOWN,

        KKM_var.HARMONICS.ampl_to_duty_cycle,
        KKM_var.HARMONICS.test_ampl,
        KKM_var.HARMONICS.test_time,
        KKM_var.HARMONICS.test_pid_I_phase,
        KKM_var.HARMONICS.test_pid_I_ampl);
}
