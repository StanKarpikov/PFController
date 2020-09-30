#include "packagecommand.h"

PackageCommand::PackageCommand(QObject *parent) : QObject(parent){
    this->package_in = nullptr;
    this->package_out = new DeviceSerialMessage();
}

PackageCommand::~PackageCommand(){
    if (this->package_in){
        delete this->package_in;
        this->package_in = NULL;
    }
    if (this->package_out){
        delete this->package_out;
        this->package_out = NULL;
    }
}

void PackageCommand::finishCommand(bool is_timed_out){
    emit complete(is_timed_out, this);
    this->deleteLater();
}
