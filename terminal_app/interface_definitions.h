#ifndef INTERFACE_DEFINITIONS_H
#define INTERFACE_DEFINITIONS_H

#include <QtCore/QtGlobal>
#include <QBrush>

namespace InterfaceDefinitions
{
    /* Constants */
    const QBrush editableCellBrush = QBrush(QColor(240, 255, 240));

    inline static const std::string DARK_GREY = "#808080";
    inline static const std::string DARK_RED = "#800000";
    inline static const std::string LIGHT_GREY = "#c0c0c0";
    inline static const std::string EXTRA_LIGHT_GREY = "#999966";
    inline static const std::string DARK_GREEN = "#008000";

    /* Texts */
    inline static const std::string STRING_PFC_STATE_INIT = "Initialization";
    inline static const std::string STRING_PFC_STATE_STOP = "Stopped";
    inline static const std::string STRING_PFC_STATE_SYNC = "Syncronisation";
    inline static const std::string STRING_PFC_STATE_PRECHARGE_PREPARE = "Precharge prepare";
    inline static const std::string STRING_PFC_STATE_PRECHARGE = "Precharge";
    inline static const std::string STRING_PFC_STATE_MAIN = "Main switch";
    inline static const std::string STRING_PFC_STATE_PRECHARGE_DISABLE = "Precharge off";
    inline static const std::string STRING_PFC_STATE_WORK = "Work";
    inline static const std::string STRING_PFC_STATE_CHARGE = "Charge";
    inline static const std::string STRING_PFC_STATE_TEST = "Test";
    inline static const std::string STRING_PFC_STATE_STOPPING = "Stopping..";
    inline static const std::string STRING_PFC_STATE_FAULTBLOCK = " Fault ";
    inline static const std::string STRING_PFC_STATE_UNKNOWN = "Unknown state";
}

#endif // INTERFACE_DEFINITIONS_H
