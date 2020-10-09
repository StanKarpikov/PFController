#ifndef PACKAGECOMMAND_H
#define PACKAGECOMMAND_H

#include <QObject>
#include "DeviceSerialMessage.h"
#include "types.h"

class PackageCommand : public QObject
{
    Q_OBJECT
   public:
    explicit PackageCommand(QObject *parent = 0);
    virtual ~PackageCommand(void);

    DeviceSerialMessage *package_out;  //!< Пакет на отправку
    DeviceSerialMessage *package_in;
    void finishCommand(bool is_timed_out);
   signals:
    void complete(bool timeout, PackageCommand *c);
   public slots:
};

#endif  // PACKAGECOMMAND_H
