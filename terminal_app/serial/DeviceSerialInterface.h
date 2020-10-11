#ifndef __PROTOCOL_H_
#define __PROTOCOL_H_

#include <exception>
#include <QThread>
#include <QQueue>
#include <QMutex>
#include "DeviceSerialMessage.h"
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
    using std::runtime_error::runtime_error;  // наследуем конструкторы runtime_error
};

class ADFSerialInterface : public QObject
{
    Q_OBJECT
   public:
    explicit ADFSerialInterface(QObject *parent = Q_NULLPTR);
    virtual ~ADFSerialInterface();

    /**
     * @brief enqueueCommand Добавляет пакет в очередь на отправку.
     * @param pc команда для отправки
     */
    void enqueueCommand(PackageCommand *pc);
    std::string hex_dump(const std::vector<unsigned char> &buf);

   protected:
    //  virtual void run();

    /**
     * @brief serialWrite Пишет данные в последовательный порт
     * @param dataToWrite данные для записи
     */
    int serialWrite(const std::vector<unsigned char> &dataToWrite);

    /**
     * @brief serialReadPackage Читает пакет из последовательного порта, ожидая не более заданного времени
     * @param timeout
     * @return
     */
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
    void Disconnect();
    void run();
    // слоты для работы с сигналами от QSerialPort
   private slots:
    void sendQueue();  //!< Отправляет первый пакет из очереди.
    void handleError(QSerialPort::SerialPortError err);
    void ConnectionChanged(bool connected)
    {
        _connected = connected;
    }
    // events
   signals:
    void couldWrite();  //!< Сигнал что в очереди появились пакеты на отправку
    void connected();
    void disconnected();
    void informConnectionChanged(bool connected);
    void Message(uint8_t type, uint8_t level, uint8_t target, QString message);
    // members
   private:
    QSerialPort *_serial;  //!< Объект для работы с последовательным портом
    struct CustomCompare
    {
        bool operator()(const PackageCommand *lhs, const PackageCommand *rhs)
        {
            if (lhs->package_out->priority() != rhs->package_out->priority())
            {
                return lhs->package_out->priority() > rhs->package_out->priority();
            }
            else
            {
                return lhs->package_out->timestamp() > rhs->package_out->timestamp();
            }
        }
    };
    std::priority_queue<PackageCommand *, std::vector<PackageCommand *>, CustomCompare> _queue;
    mutable QMutex sendQueueMutex;

    // contstants
   private:
    bool _connected = false;
    int noError = 1;
    static const int SEND_QUEUE_LEN_MAX = 50;
    static const int READ_TIMEOUT_MS = 500;  //!< таймаут ожидания на чтения в миллисекундах
};

#endif /* __PROTOCOL_H_ */
