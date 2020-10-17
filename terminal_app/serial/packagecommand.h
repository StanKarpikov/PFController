#ifndef __INTERFACE_PACKAGE_H
#define __INTERFACE_PACKAGE_H

#include <QObject>
#include "deviceserialmessage.h"

class InterfacePackage : public QObject
{
    Q_OBJECT
   public:
    explicit InterfacePackage(QObject *parent = Q_NULLPTR);
    virtual ~InterfacePackage(void);

    DeviceSerialMessage *package_to_send;
    DeviceSerialMessage *package_read;
    void finishProcessing(bool is_timed_out);
   signals:
    void complete(bool timeout, InterfacePackage *c);
};

#endif  /* __INTERFACE_PACKAGE_H */
