#include <vector>
#include <algorithm>
#include <iostream>

#include "crc.h"
#include "deviceserialmessage.h"
#include <QDebug>

using namespace std;

DeviceSerialMessage::DeviceSerialMessage()
    : DeviceSerialMessage(MessagePriority::NORMAL, 0, Sender::PFC, false, false, 0, std::vector<uint8_t>())
{
}

DeviceSerialMessage::DeviceSerialMessage(const MessagePriority priority, const uint8_t command, const Sender sender, const bool error,
                                         const bool crcError, const uint16_t crc,
                                         const std::vector<uint8_t> &data)
    : _sender(sender), _error(error), _crcError(crcError), _crc(crc), _command(command), _priority(priority)
{
    std::copy(data.begin(), data.end(), back_inserter(_data));
}

DeviceSerialMessage::DeviceSerialMessage(const DeviceSerialMessage &other)
    : DeviceSerialMessage(other._priority, other._command, other._sender, other._error, other._crcError, other._crc, other._data)
{
}

DeviceSerialMessage::~DeviceSerialMessage()
{}

void DeviceSerialMessage::clear()
{
    clearData();
    setPriority(MessagePriority::NORMAL);
    setCommand(0);
    setSender(Sender::PFC);
    setError(false);
    setCrcError(false);
}

void DeviceSerialMessage::fill(
    MessagePriority priority,
    Sender sender,
    uint8_t comm,
    const std::vector<uint8_t> &data)
{
    setPriority(priority);
    setSender(sender);
    appendData(data);
    setCommand(comm);
    _timestamp = QDateTime::currentMSecsSinceEpoch();
}

uint64_t DeviceSerialMessage::timestamp(void)
{
    return _timestamp;
}

void DeviceSerialMessage::clearData()
{
    _data.clear();
}

bool DeviceSerialMessage::appendData(const std::vector<uint8_t> &data)
{
    if (data.size() > MAX_LENGTH - MIN_LENGTH - _data.size())
        return false;
    copy(data.begin(), data.end(), back_inserter(_data));
    return true;
}

bool DeviceSerialMessage::appendData(const uint8_t data)
{
    if (_data.size() >= MAX_LENGTH - MIN_LENGTH)
        return false;
    _data.push_back(data);
    return true;
}

std::vector<uint8_t> DeviceSerialMessage::data() const
{
    return _data;
}

uint8_t DeviceSerialMessage::data(uint pos) const
{
    return _data[pos];
}

unsigned int DeviceSerialMessage::dataLength() const
{
    return _data.size();
}

uint8_t DeviceSerialMessage::command() const
{
    return _command;
}

void DeviceSerialMessage::setCommand(uint8_t cmd)
{
    _command = cmd;
}

DeviceSerialMessage::Sender DeviceSerialMessage::sender() const
{
    return _sender;
}

void DeviceSerialMessage::setSender(Sender sender)
{
    _sender = sender;
}
DeviceSerialMessage::MessagePriority DeviceSerialMessage::priority() const
{
    return _priority;
}

void DeviceSerialMessage::setPriority(MessagePriority priority)
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

uint16_t DeviceSerialMessage::crc() const
{
    return _crc;
}

void DeviceSerialMessage::setCrc(const uint16_t crc)
{
    _crc = crc;
}

std::vector<uint8_t> DeviceSerialMessage::toBuffer() const
{
    vector<uint8_t> raw_data;
    raw_data.push_back(START_BYTE);

    uint8_t status = 0;
    if (sender() == Sender::GUI) status |= 0x01;
    if (error()) status |= 0x02;
    if (crcError()) status |= 0x04;
    raw_data.push_back(status);

    raw_data.push_back(_data.size() + 4);
    raw_data.push_back(command());
    copy(_data.begin(), _data.end(), back_inserter(raw_data));

    uint16_t crc = crc16(vector<uint8_t>(raw_data.begin() + 1, raw_data.end()));
    raw_data.push_back(crc & 0xFF);
    raw_data.push_back((crc >> 8) & 0xFF);
    raw_data.push_back(STOP_BYTE);

    return raw_data;
}

DeviceSerialMessage *DeviceSerialMessage::popFromBuffer(std::vector<uint8_t> &data)
{
    for (uint i = 0; i < data.size(); i++)
    {
        if (data[i] == START_BYTE)
        {
            uint start_pos = i;

            if (start_pos + HEADER_LEN > data.size())
                break;  // incomplete package header

            int status = data[start_pos + 1];
            int package_len = data[start_pos + 2];

            if (start_pos + HEADER_LEN + package_len > data.size())
                break;  // incomplete package

            uint8_t command = data[start_pos + 3];
            uint16_t crc = data[start_pos + HEADER_LEN + package_len - 2];
            crc |= data[start_pos + HEADER_LEN + package_len - 1] << 8;
            uint8_t stop_byte = data[start_pos + HEADER_LEN + package_len];

            if (stop_byte != STOP_BYTE)
                continue;  // no magic stop byte at the end of the package - this is not a valid package.

            uint16_t crccalc = crc16(vector<uint8_t>(data.begin() + start_pos + 1,
                                                           data.begin() + start_pos + HEADER_LEN + package_len - 2));

            if (crc != crccalc)
            {
                continue;  // crc mismatch
            }
            DeviceSerialMessage *p = new DeviceSerialMessage();
            p->setCommand(command);
            p->setCrc(crc);
            p->setSender(status & 0x01 ? Sender::GUI : Sender::PFC);
            p->setError(status & 0x02 ? true : false);
            p->setCrcError(status & 0x04 ? true : false);
            p->appendData(vector<uint8_t>(data.begin() + start_pos + HEADER_LEN + 1, data.begin() + start_pos + HEADER_LEN + package_len - 2));
            data.erase(data.begin(), data.begin() + HEADER_LEN + package_len);

            return p;
        }
    }

    return Q_NULLPTR;
}
