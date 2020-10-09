#ifndef DEVICE_SERIAL_MESSAGE_H_
#define DEVICE_SERIAL_MESSAGE_H_

#include <vector>
#include "types.h"
#include <QtGlobal>
#include <QDateTime>

enum class Sender
{
    PFC,
    GUI
};
enum class ADFMessagePriority
{
    HIGH,
    NORMAL,
    LOW,
};

class DeviceSerialMessage
{
   public:
    static const int MAX_LENGTH;
    static const unsigned char START_BYTE;
    static const unsigned char STOP_BYTE;

   public:
    std::vector<unsigned char> _data;
    explicit DeviceSerialMessage(void);
    DeviceSerialMessage(const ADFMessagePriority prior, const unsigned char command, const Sender sender,
                        const bool error, const bool crcError, unsigned char crc,
                        const std::vector<unsigned char> &data);

    DeviceSerialMessage(const DeviceSerialMessage &other);

    virtual ~DeviceSerialMessage(void);

    virtual void clear();

    virtual void clearData();

    virtual bool addData(const std::vector<unsigned char> &data);

    virtual bool addData(const unsigned char data);

    virtual std::vector<unsigned char> data() const;

    virtual unsigned char data(unsigned int pos) const;

    unsigned int dataLength() const;

    unsigned char command() const;

    void setCommand(unsigned char cmd);

    Sender sender() const;

    void setSender(Sender sender);

    bool error() const;

    void setError(bool error);

    bool crcError() const;

    void setCrcError(bool crc_error);

    quint16 crc() const;

    void setCrc(quint16 crc);

    quint32 timestamp()
    {
        return _timestamp;
    }

    ADFMessagePriority priority() const;
    void setPriority(ADFMessagePriority priority);

    std::vector<unsigned char> toBuffer() const;

    void fill(
        ADFMessagePriority priority,
        Sender sender,
        unsigned char comm,
        const std::vector<unsigned char> &data)
    {
        setPriority(priority);
        setSender(sender);
        addData(data);
        setCommand(comm);
        _timestamp = QDateTime::currentMSecsSinceEpoch();
    }

   public:
    static DeviceSerialMessage *popFromBuffer(std::vector<unsigned char> &data);

   private:
    static const int MIN_LENGTH = 7;
    static const int HEADER_LEN = 3;

   private:
    Sender _sender;
    bool _error;
    bool _crcError;
    quint16 _crc;
    unsigned char _command;
    ADFMessagePriority _priority;
    quint32 _timestamp;
};

#endif /* DEVICE_SERIAL_MESSAGE_H_ */
