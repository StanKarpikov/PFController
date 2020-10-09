QT += widgets serialport serialbus printsupport testlib

TARGET = terminal
TEMPLATE = app

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    settingsdialog.cpp \
    serial/crc.cpp \
    serial/packagecommand.cpp \
    qcustomplot.cpp \
    page_oscillog.cpp \
    page_main.cpp \
    page_settingscalibrations.cpp \
    page_settingscapacitors.cpp \
    page_settingsprotection.cpp \
    eqwidget.cpp \
    page_filters.cpp \
    serial/DeviceSerialInterface.cpp \
    serial/DeviceSerialMessage.cpp \
    device.cpp

HEADERS += \
    mainwindow.h \
    settingsdialog.h \
    mainwindow.h \
    serial/crc.h \
    serial/packagecommand.h \
    qcustomplot.h \
    eqwidget.h \
    htmldelegate.h \
    serial/DeviceSerialInterface.h \
    serial/DeviceSerialMessage.h \
    device.h

FORMS += \
    mainwindow.ui \
    settingsdialog.ui

RESOURCES += \
    terminal_pfc.qrc
