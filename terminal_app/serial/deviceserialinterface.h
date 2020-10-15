#ifndef _DEVICE_SERIAL_INTERFACE_H_
#define _DEVICE_SERIAL_INTERFACE_H_

#include <exception>
#include <QThread>
#include <QQueue>
#include <QMutex>
#include "deviceserialmessage.h"
#include "packagecommand.h"
#include <queue>
#include <QSerialPort>
#include <functional>
#include <map>
#include "types.h"
class QSerialPort;
class QTimer;

class ProtocolException : public std::runtime_error
{
    using std::runtime_error::runtime_error;
};

class PFCSerialInterface : public QObject
{
    Q_OBJECT
   public:
    explicit PFCSerialInterface(QObject *parent = Q_NULLPTR);
    virtual ~PFCSerialInterface();

    void enqueueCommand(InterfacePackage *pc);
    std::string hex_dump(const std::vector<uint8_t> &buf);

   protected:
    int serialWrite(const std::vector<uint8_t> &dataToWrite);

    DeviceSerialMessage *serialReadPackage(int timeout);
   public slots:
    void ConnectTo(
        QString name,
        qint32 baudRate = QSerialPort::Baud115200,
        QSerialPort::DataBits dataBits = QSerialPort::Data8,
        QSerialPort::Parity parity = QSerialPort::NoParity,
        QSerialPort::StopBits stopBits = QSerialPort::OneStop,
        QSerialPort::FlowControl flowControl = QSerialPort::NoFlowControl,
        bool localEchoEnabled = 0,
        quint32 timeout = 1000,
        quint32 numberOfRetries = 3);
    void disconnectFromDevice();
    void run();

   private slots:
    void sendQueue();
    void handleError(QSerialPort::SerialPortError err);
    void connectionHasChanged(bool connected);

   signals:
    void couldWrite();  //!< Сигнал что в очереди появились пакеты на отправку
    void connected();
    void disconnected();
    void informConnectionChanged(bool connected);
    void message(uint8_t type, uint8_t level, uint8_t target, std::string message);

   private:
    QSerialPort *_serial;  //!< Объект для работы с последовательным портом
    struct CustomCompare
    {
        bool operator()(const InterfacePackage *lhs, const InterfacePackage *rhs)
        {
            if (lhs->package_to_send->priority() != rhs->package_to_send->priority())
            {
                return lhs->package_to_send->priority() > rhs->package_to_send->priority();
            }
            else
            {
                return lhs->package_to_send->timestamp() > rhs->package_to_send->timestamp();
            }
        }
    };
    std::priority_queue<InterfacePackage *, std::vector<InterfacePackage *>, CustomCompare> _queue;
    mutable QMutex sendQueueMutex;
   private:
    bool _connected = false;
    int noError = 1;
    static const int SEND_QUEUE_LEN_MAX = 50;
    static const int READ_TIMEOUT_MS = 500;  //!< таймаут ожидания на чтения в миллисекундах
};

#endif /* _DEVICE_SERIAL_INTERFACE_H_ */
