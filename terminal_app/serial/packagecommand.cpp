#include "packagecommand.h"

InterfacePackage::InterfacePackage(QObject *parent)
    : QObject(parent)
{
    this->package_read = nullptr;
    this->package_to_send = new DeviceSerialMessage();
}

InterfacePackage::~InterfacePackage()
{
    if (this->package_read)
    {
        delete this->package_read;
        this->package_read = Q_NULLPTR;
    }
    if (this->package_to_send)
    {
        delete this->package_to_send;
        this->package_to_send = Q_NULLPTR;
    }
}

void InterfacePackage::finishProcessing(bool is_timed_out)
{
    emit complete(is_timed_out, this);
    this->deleteLater();
}
