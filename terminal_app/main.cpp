/**
 * @file main.c
 * @author Stanislav Karpikov
 * @brief The main module: initialisation, basic functions
 */

/** @addtogroup app_main
 * @{
 */

/*--------------------------------------------------------------
                       INCLUDES
--------------------------------------------------------------*/

#include <QApplication>
#include "mainwindow.h"

/*--------------------------------------------------------------
                       PUBLIC FUNCTIONS
--------------------------------------------------------------*/

/**
 * @brief  The application entry point.
 * @return Status
 */
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    MainWindow *w = new MainWindow();
    w->show();
    return a.exec();
}

/** @} */
