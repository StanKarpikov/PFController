#ifndef INTERFACE_MESSAGING_H
#define INTERFACE_MESSAGING_H

namespace InterfaceMessaging
{
    /* Messaging subsystem */
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

    enum eMESSAGE_TARGET{
        MESSAGE_TARGET_NONE      = (0),
        MESSAGE_TARGET_DEBUG     = (1<<0),
        MESSAGE_TARGET_STATUS    = (1<<1),
        MESSAGE_TARGET_HISTORY   = (1<<2),
        MESSAGE_TARGET_ALL       = (MESSAGE_TARGET_DEBUG|MESSAGE_TARGET_STATUS|MESSAGE_TARGET_HISTORY)
    };
}

#endif // INTERFACE_MESSAGING_H
