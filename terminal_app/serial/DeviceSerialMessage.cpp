#include <vector>
#include <algorithm>
#include <iostream>

#include "crc.h"
#include "DeviceSerialMessage.h"

using namespace std;

const int            DeviceSerialMessage::MAX_LENGTH =     256;
const unsigned char  DeviceSerialMessage::START_BYTE =     0xAA;
const unsigned char  DeviceSerialMessage::STOP_BYTE  =     0xBB;

DeviceSerialMessage::DeviceSerialMessage()
    : DeviceSerialMessage(ADFMessagePriority::NORMAL,0, Sender::ADF, false, false, 0, std::vector<unsigned char>()) {
}

DeviceSerialMessage::DeviceSerialMessage(const ADFMessagePriority priority,const unsigned char command, const Sender sender, const bool error,
                 const bool crcError, unsigned char crc,
                 const std::vector<unsigned char> &data)
    :_sender(sender), _error(error), _crcError(crcError), _crc(crc), _priority(priority), _command(command) {
    std::copy(data.begin(), data.end(), back_inserter(_data));
}

DeviceSerialMessage::DeviceSerialMessage(const DeviceSerialMessage &other)
    :  DeviceSerialMessage(other._priority,other._command, other._sender, other._error, other._crcError, other._crc, other._data)
{
}

DeviceSerialMessage::~DeviceSerialMessage() {}

void DeviceSerialMessage::clear()
{
    clearData();
    setPriority(ADFMessagePriority::NORMAL);
    setCommand(0);
    setSender(Sender::ADF);
    setError(false);
    setCrcError(false);
}

void DeviceSerialMessage::clearData()
{
    _data.clear();
}

bool DeviceSerialMessage::addData(const std::vector<unsigned char> &data)
{
    if (data.size() > MAX_LENGTH - MIN_LENGTH - _data.size())
        return false;
    copy(data.begin(), data.end(), back_inserter(_data));
    return true;
}

bool DeviceSerialMessage::addData(const unsigned char data)
{
    if (_data.size() >= MAX_LENGTH - MIN_LENGTH)
        return false;
    _data.push_back(data);
    return true;
}

std::vector<unsigned char> DeviceSerialMessage::data() const
{
    return _data;
}

unsigned char DeviceSerialMessage::data(unsigned int pos) const {
    return _data[pos];
}

unsigned int DeviceSerialMessage::dataLength() const
{
    return _data.size();
}

unsigned char DeviceSerialMessage::command() const
{
    return _command;
}

void DeviceSerialMessage::setCommand(unsigned char cmd)
{
    _command = cmd;
}

Sender DeviceSerialMessage::sender() const
{
    return _sender;
}

void DeviceSerialMessage::setSender(Sender sender)
{
    _sender = sender;
}
ADFMessagePriority DeviceSerialMessage::priority() const
{
    return _priority;
}

void DeviceSerialMessage::setPriority(ADFMessagePriority priority)
{
    _priority = priority;
}

bool DeviceSerialMessage::error() const
{
    return _error;
}

void DeviceSerialMessage::setError(bool error)
{
    _error = error;
}

bool DeviceSerialMessage::crcError() const
{
    return _crcError;
}

void DeviceSerialMessage::setCrcError(bool crc_error)
{
    _crcError = crc_error;
}

quint16 DeviceSerialMessage::crc() const
{
    return _crc;
}

void DeviceSerialMessage::setCrc(quint16 crc)
{
    _crc = crc;
}

std::vector<unsigned char> DeviceSerialMessage::toBuffer() const
{
    vector<unsigned char> raw_data;
    raw_data.push_back(START_BYTE);

    unsigned char status = 0;
    if (sender() == Sender::GUI)    status |= 0x01;
    if (error())                    status |= 0x02;
    if (crcError())                 status |= 0x04;
    raw_data.push_back(status);

    raw_data.push_back(_data.size() + 4);
    raw_data.push_back(command());
    copy(_data.begin(), _data.end(), back_inserter(raw_data));

    quint16 crc = crc16(vector<unsigned char>(raw_data.begin() + 1, raw_data.end()));
    raw_data.push_back(crc&0xFF);
    raw_data.push_back((crc>>8)&0xFF);
    raw_data.push_back(STOP_BYTE);

    return raw_data;
}
#include "QDebug.h"
DeviceSerialMessage * DeviceSerialMessage::popFromBuffer(std::vector<unsigned char> &data){
    for (int i = 0; i < data.size(); i++){
        if (data[i] == START_BYTE){
            int start_pos = i;

            if (start_pos + HEADER_LEN > data.size())
                break;  // incomplete package header

            int status      = data[start_pos + 1];
            int package_len = data[start_pos + 2];

            if (start_pos + HEADER_LEN + package_len > data.size())
                break;  // incomplete package

            int command     = data[start_pos + 3];
            quint16 crc     = data[start_pos + HEADER_LEN + package_len - 2];
                    crc    |= data[start_pos + HEADER_LEN + package_len - 1]<<8;
            int stop_byte   = data[start_pos + HEADER_LEN + package_len];

            if (stop_byte != STOP_BYTE)
                continue;  // no magic stop byte at the end of the package - this is not a valid package.

            uint16_t crccalc=crc16(vector<unsigned char>(data.begin() + start_pos + 1,
                                                       data.begin() + start_pos + HEADER_LEN + package_len - 2));

            if (crc != crccalc){
                continue;  // crc mismatch
            }
            DeviceSerialMessage * p = new DeviceSerialMessage();
            p->setCommand(command);
            p->setCrc(crc);
            p->setSender(status & 0x01 ? Sender::GUI : Sender::ADF);
            p->setError(status & 0x02 ? true : false);
            p->setCrcError(status & 0x04 ? true : false);
            p->addData(vector<unsigned char>(data.begin() + start_pos + HEADER_LEN + 1, data.begin() + start_pos + HEADER_LEN + package_len - 2));
            data.erase(data.begin(), data.begin() + HEADER_LEN + package_len);

            return p;
        }
    }

    return NULL;
}
