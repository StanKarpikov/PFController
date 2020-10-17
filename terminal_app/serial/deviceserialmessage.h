#ifndef DEVICE_SERIAL_MESSAGE_H_
#define DEVICE_SERIAL_MESSAGE_H_

#include <vector>
#include <QtGlobal>
#include <QDateTime>

class DeviceSerialMessage
{
   private:
     static const auto MIN_LENGTH = 7;
     static const auto HEADER_LEN = 3;
     const static auto MAX_LENGTH = 256;
     const static auto START_BYTE = 0x55;
     const static auto STOP_BYTE  = 0x77;

   public:
     enum class Sender
     {
         PFC,
         GUI
     };

     enum class MessagePriority
     {
         HIGH,
         NORMAL,
         LOW,
     };

    explicit DeviceSerialMessage(void);
    DeviceSerialMessage(const MessagePriority prior, const uint8_t command, const Sender sender,
                        const bool error, const bool crcError, const uint16_t crc,
                        const std::vector<uint8_t> &data);

    DeviceSerialMessage(const DeviceSerialMessage &other);

    virtual ~DeviceSerialMessage(void);

    virtual void clear(void);

    virtual void clearData(void);

    virtual bool appendData(const std::vector<uint8_t> &data);

    virtual bool appendData(const uint8_t data);

    virtual std::vector<uint8_t> data(void) const;

    virtual uint8_t data(unsigned int pos) const;

    unsigned int dataLength() const;

    uint8_t command(void) const;

    void setCommand(uint8_t cmd);

    Sender sender(void) const;

    void setSender(Sender sender);

    bool error(void) const;

    void setError(bool error);

    bool crcError(void) const;

    void setCrcError(bool crc_error);

    uint16_t crc(void) const;

    void setCrc(const uint16_t crc);

    uint64_t timestamp(void);

    MessagePriority priority(void) const;
    void setPriority(MessagePriority priority);

    std::vector<uint8_t> toBuffer(void) const;

    void fill(
        MessagePriority priority,
        Sender sender,
        uint8_t comm,
        const std::vector<uint8_t> &data);

   public:
    static DeviceSerialMessage *popFromBuffer(std::vector<uint8_t> &data);

   private:
    std::vector<uint8_t> _data;
    Sender _sender;
    bool _error;
    bool _crcError;
    uint16_t _crc;
    uint8_t _command;
    MessagePriority _priority;
    uint64_t _timestamp;
};

#endif /* DEVICE_SERIAL_MESSAGE_H_ */
