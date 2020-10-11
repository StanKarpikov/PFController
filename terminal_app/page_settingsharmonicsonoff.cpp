#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "settingsdialog.h"
#include <QMessageBox>
#include <QMessageBox>
#include <QDateTime>
#include <QGraphicsDropShadowEffect>
#include <QListWidgetItem>

void MainWindow::page_SettingsHarmonicsOnOff_Init()
{
}
//-----------------------------------------------------------------------
void MainWindow::on_check_H246_stateChanged(int arg1)
{
    for (int i = 2; i < HARMONICS_NUM; i += 2)
    {
        KKM_var.HARMONICS.fft_suppression[i] = (arg1 == 1);
    }
    writeSettingsHarmonicsSuppression(KKM_var.HARMONICS.fft_suppression);
}
//-----------------------------------------------------------------------
void MainWindow::on_check_H1_50_stateChanged(int arg1)
{
    for (int i = 1; i < HARMONICS_NUM; i++)
    {
        KKM_var.HARMONICS.fft_suppression[i] = (arg1 == 1);
    }
    writeSettingsHarmonicsSuppression(KKM_var.HARMONICS.fft_suppression);
}
//-----------------------------------------------------------------------
void MainWindow::on_check_H1_10_stateChanged(int arg1)
{
    for (int i = 1; i <= 10; i++)
    {
        KKM_var.HARMONICS.fft_suppression[i] = (arg1 == 1);
    }
    writeSettingsHarmonicsSuppression(KKM_var.HARMONICS.fft_suppression);
}

void MainWindow::on_check_H11_20_stateChanged(int arg1)
{
    for (int i = 11; i <= 20; i++)
    {
        KKM_var.HARMONICS.fft_suppression[i] = (arg1 == 1);
    }
    writeSettingsHarmonicsSuppression(KKM_var.HARMONICS.fft_suppression);
}

void MainWindow::on_check_H21_30_stateChanged(int arg1)
{
    for (int i = 21; i <= 30; i++)
    {
        KKM_var.HARMONICS.fft_suppression[i] = (arg1 == 1);
    }
    writeSettingsHarmonicsSuppression(KKM_var.HARMONICS.fft_suppression);
}

void MainWindow::on_check_H31_40_stateChanged(int arg1)
{
    for (int i = 31; i <= 40; i++)
    {
        KKM_var.HARMONICS.fft_suppression[i] = (arg1 == 1);
    }
    writeSettingsHarmonicsSuppression(KKM_var.HARMONICS.fft_suppression);
}

void MainWindow::on_check_H41_50_stateChanged(int arg1)
{
    for (int i = 41; i <= 50; i++)
    {
        KKM_var.HARMONICS.fft_suppression[i] = (arg1 == 1);
    }
    writeSettingsHarmonicsSuppression(KKM_var.HARMONICS.fft_suppression);
}
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
#define TOGGLE_HANDLER(NUM)                                                   \
    void MainWindow::on_check_H##NUM##_toggled(bool checked)                  \
    {                                                                         \
        ADF_var.HARMONICS.fft_suppression[NUM] = checked;                     \
        writeSettingsHarmonicsSuppression(ADF_var.HARMONICS.fft_suppression); \
    }

TOGGLE_HANDLER(1)
TOGGLE_HANDLER(2)
TOGGLE_HANDLER(3)
TOGGLE_HANDLER(4)
TOGGLE_HANDLER(5)
TOGGLE_HANDLER(6)
TOGGLE_HANDLER(7)
TOGGLE_HANDLER(8)
TOGGLE_HANDLER(9)
TOGGLE_HANDLER(10)
TOGGLE_HANDLER(11)
TOGGLE_HANDLER(12)
TOGGLE_HANDLER(13)
TOGGLE_HANDLER(14)
TOGGLE_HANDLER(15)
TOGGLE_HANDLER(16)
TOGGLE_HANDLER(17)
TOGGLE_HANDLER(18)
TOGGLE_HANDLER(19)
TOGGLE_HANDLER(20)
TOGGLE_HANDLER(21)
TOGGLE_HANDLER(22)
TOGGLE_HANDLER(23)
TOGGLE_HANDLER(24)
TOGGLE_HANDLER(25)
TOGGLE_HANDLER(26)
TOGGLE_HANDLER(27)
TOGGLE_HANDLER(28)
TOGGLE_HANDLER(29)
TOGGLE_HANDLER(30)
TOGGLE_HANDLER(31)
TOGGLE_HANDLER(32)
TOGGLE_HANDLER(33)
TOGGLE_HANDLER(34)
TOGGLE_HANDLER(35)
TOGGLE_HANDLER(36)
TOGGLE_HANDLER(37)
TOGGLE_HANDLER(38)
TOGGLE_HANDLER(39)
TOGGLE_HANDLER(40)
TOGGLE_HANDLER(41)
TOGGLE_HANDLER(42)
TOGGLE_HANDLER(43)
TOGGLE_HANDLER(44)
TOGGLE_HANDLER(45)
TOGGLE_HANDLER(46)
TOGGLE_HANDLER(47)
TOGGLE_HANDLER(48)
TOGGLE_HANDLER(49)
TOGGLE_HANDLER(50)
