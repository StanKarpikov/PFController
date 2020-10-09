#ifndef TYPES_H
#define TYPES_H

enum eMESSAGE_TYPES{
    MESSAGE_TYPE_GENERAL,
    MESSAGE_TYPE_CONNECTION,
    MESSAGE_TYPE_GLOBALWARNING,
    MESSAGE_TYPE_GLOBALFAULT,
    MESSAGE_TYPE_STATE,
    MESSAGE_TYPE_CAPACITORS,
    MESSAGE_TYPE_NETWORK
};

enum eMESSAGE_LEVELS{
    MESSAGE_NORMAL,
    MESSAGE_WARNING,
    MESSAGE_ERROR
};

#define MESSAGE_TARGET_NONE      (0)
#define MESSAGE_TARGET_DEBUG     (1<<0)
#define MESSAGE_TARGET_STATUS    (1<<1)
#define MESSAGE_TARGET_HISTORY   (1<<2)
#define MESSAGE_TARGET_ALL       (MESSAGE_TARGET_DEBUG|MESSAGE_TARGET_STATUS|MESSAGE_TARGET_HISTORY)

#define MESSAGE(type, level, targMessage) \
    QMetaObject::invokeMethod((QObject*)w, "Message", \
        Qt::QueuedConnection, \
        Q_ARG(quint8,  type ), \
        Q_ARG(quint8,  level ), \
        Q_ARG(quint8,  target ), \
        Q_ARG(QString, str ) \
    );
#endif // TYPES_H
