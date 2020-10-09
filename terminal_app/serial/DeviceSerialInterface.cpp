/**
 *
 */

#include <QtSerialPort/QSerialPort>
#include <QTimer>
#include <QDebug>
#include <QMutexLocker>
#include <QElapsedTimer>
#include <iostream>
#include <sstream>
#include <iomanip>
#include "DeviceSerialInterface.h"
#include <queue>

using namespace std;

ADFSerialInterface::ADFSerialInterface(QObject *parent) throw(ProtocolException)
{
    qRegisterMetaType<std::vector<DeviceSerialMessage>>("std::vector<Package>");
    connect(this, &connectionChanged,
            this, &_ConnectionChanged);
}

ADFSerialInterface::~ADFSerialInterface()
{
    {  // Locking in the RAII style
        QMutexLocker locker(&sendQueueMutex);

        while (_queue.size())
        {
            delete _queue.top();
            _queue.pop();
        }
    }
}
void ADFSerialInterface::ConnectTo(
    QString name,
    qint32 baudRate,
    QSerialPort::DataBits dataBits,
    QSerialPort::Parity parity,
    QSerialPort::StopBits stopBits,
    QSerialPort::FlowControl flowControl,
    bool localEchoEnabled,
    quint32 timeout,
    quint32 numberOfRetries)
{
    if (_serial->isOpen())
    {
        Message(MESSAGE_TYPE_CONNECTION, MESSAGE_NORMAL, MESSAGE_TARGET_ALL,
                "Порт закрыт");
        _serial->close();
    }

    _serial->setPortName(name);
    _serial->setBaudRate(baudRate);
    _serial->setDataBits(dataBits);
    _serial->setParity(parity);
    _serial->setStopBits(stopBits);
    _serial->setFlowControl(flowControl);

    if (_serial->open(QIODevice::ReadWrite))
    {
        Message(MESSAGE_TYPE_CONNECTION, MESSAGE_NORMAL, MESSAGE_TARGET_ALL,
                QString("Порт %1 открыт: %2").arg(name).arg(QString::number(baudRate)));
        emit connected();
    }
    else
    {
        Message(MESSAGE_TYPE_CONNECTION, MESSAGE_WARNING, MESSAGE_TARGET_ALL,
                QString("Ошибка открытия порта %1: %2").arg(name).arg(QString::number(baudRate)));
    }
}
void ADFSerialInterface::Disconnect()
{
    if (_serial->isOpen())
    {
        _serial->close();
        Message(MESSAGE_TYPE_CONNECTION, MESSAGE_NORMAL, MESSAGE_TARGET_ALL,
                "Порт закрыт");
        emit disconnected();
    }
    else
    {
        Message(MESSAGE_TYPE_CONNECTION, MESSAGE_WARNING, MESSAGE_TARGET_DEBUG,
                "Попытка закрытия неотрытого порта!");
        emit disconnected();
    }
}
void ADFSerialInterface::handleError(QSerialPort::SerialPortError err)
{
    if (err == 0) return;
    uint32_t target = MESSAGE_TARGET_ALL;
    if (noError == 0) target = MESSAGE_TARGET_DEBUG;
    noError = 0;
    Message(MESSAGE_TYPE_CONNECTION, MESSAGE_ERROR, MESSAGE_TARGET_DEBUG,
            QString("Ошибка порта: %1(%2)")
                .arg(err)
                .arg(_serial->errorString()));
    if (err == 12)
    {
        Message(MESSAGE_TYPE_CONNECTION, MESSAGE_ERROR, target,
                QString("Соединение потеряно"));
    }
    //Disconnect();
}
DeviceSerialMessage *ADFSerialInterface::serialReadPackage(int timeout)
{
    QElapsedTimer timer;
    vector<unsigned char> readBuf;

    timer.start();

    try
    {
        while (timer.elapsed() < timeout)
        {
            QByteArray portion = _serial->readAll();

            if (portion.size())
            {
                Message(MESSAGE_TYPE_CONNECTION, MESSAGE_NORMAL, MESSAGE_TARGET_NONE,
                        QString("GET : ").append(QString::fromStdString(hex_dump(
                            std::vector<unsigned char>(
                                portion.begin(), portion.end())))));

                readBuf.reserve(readBuf.size() + portion.size());
                copy(portion.begin(), portion.end(), back_inserter(readBuf));

                DeviceSerialMessage *response = DeviceSerialMessage::popFromBuffer(readBuf);

                if (response)
                {
                    if (noError == 0)
                    {
                        Message(MESSAGE_TYPE_CONNECTION, MESSAGE_NORMAL, MESSAGE_TARGET_ALL,
                                QString("Соединение восстановлено"));
                    }
                    noError = 1;
                    return response;
                }
            }
            _serial->waitForReadyRead(timeout - timer.elapsed());
        }
        Message(MESSAGE_TYPE_CONNECTION, MESSAGE_WARNING, MESSAGE_TARGET_DEBUG,
                "Ошибка ожидания ответа");
    }
    catch (ProtocolException &e)
    {
        Message(MESSAGE_TYPE_CONNECTION, MESSAGE_ERROR, MESSAGE_TARGET_DEBUG,
                QString("Exception %1")
                    .arg(e.what()));
    }

    return NULL;
}

void ADFSerialInterface::run()
{
    {  // Locking in the RAII style
        QMutexLocker locker(&sendQueueMutex);

        while (_queue.size())
        {
            delete _queue.top();
            _queue.pop();
        }
    }

    _serial = new QSerialPort();
    connect(_serial, SIGNAL(error(QSerialPort::SerialPortError)), this,
            SLOT(handleError(QSerialPort::SerialPortError)));
    connect(this, &ADFSerialInterface::couldWrite,
            this, &ADFSerialInterface::sendQueue, Qt::QueuedConnection);
}

/**
 * @brief Protocol::serialWrite
 * Отправка данных по QSerialPort
 *
 * @param dataToWrite
 */
int ADFSerialInterface::serialWrite(const vector<unsigned char> &dataToWrite)
{
    int dataSize = dataToWrite.size();

    // C11++ style - get raw data from vector
    const char *data = (const char *)dataToWrite.data();

    // отправляем
    if (!_serial || !_serial->isOpen())
    {
        Message(MESSAGE_TYPE_CONNECTION, MESSAGE_ERROR, MESSAGE_TARGET_NONE,
                "Port not opened");
        return -1;
    }
    int written = _serial->write(data, dataSize);

    if (written < 0)
    {
        Message(MESSAGE_TYPE_CONNECTION, MESSAGE_ERROR, MESSAGE_TARGET_DEBUG,
                "Write error");
        return -1;
    }
    else if (written != dataSize)
    {
        Message(MESSAGE_TYPE_CONNECTION, MESSAGE_ERROR, MESSAGE_TARGET_DEBUG,
                "Write error: not all");
        return -1;
    }
    // ждем пока все отправится
    if (!_serial->waitForBytesWritten(3000))
    {
        Message(MESSAGE_TYPE_CONNECTION, MESSAGE_ERROR, MESSAGE_TARGET_DEBUG,
                "Write error: wait");
        return -1;
    }

    return written;
}

std::string ADFSerialInterface::hex_dump(const std::vector<unsigned char> &buf)
{
    ostringstream oss;
    for (int i = 0; i < buf.size(); i++)
        oss << " " << std::uppercase << setfill('0') << setw(2) << std::hex << (int)buf[i];
    return oss.str();
}

/**
 * @brief Protocol::sendQueue
 * Оправляем первый пакет из очереди.
 * Непосредственно отправляет\получает данные с QSerialPort
 */
void ADFSerialInterface::sendQueue()
{
    int written;
    PackageCommand *pc = NULL;

    {  // Locking in the RAII style
        QMutexLocker locker(&sendQueueMutex);

        if (_queue.empty())
            return;
        else
        {
            pc = _queue.top();
            _queue.pop();
        }
    }

    const vector<unsigned char> &dataToWrite = pc->package_out->toBuffer();

    // отправляем пакет
    written = serialWrite(dataToWrite);
    if (written < 0)
    {
        pc->finishCommand(true);
        emit connectionChanged(false);
        return;
    }
    Message(MESSAGE_TYPE_CONNECTION, MESSAGE_NORMAL, MESSAGE_TARGET_NONE,
            QString("SENDED: ").append(QString::fromStdString(hex_dump(dataToWrite))));

    // получаем ответ
    if (pc->package_in)
    {
        delete pc->package_in;
        pc->package_in = NULL;
    }

    pc->package_in = this->serialReadPackage(READ_TIMEOUT_MS);

    if (pc->package_in)
    {
        Message(MESSAGE_TYPE_CONNECTION, MESSAGE_NORMAL, MESSAGE_TARGET_NONE,
                QString("RECEIVED: ").append(QString::fromStdString(hex_dump(pc->package_in->data()))));
        emit connectionChanged(true);
        pc->finishCommand(false);
    }
    else
    {
        Message(MESSAGE_TYPE_CONNECTION, MESSAGE_ERROR, MESSAGE_TARGET_DEBUG,
                "Превышено время ожидания!");
        pc->finishCommand(true);
        emit connectionChanged(false);
    }

    if (!_queue.empty()) emit couldWrite();  //избыточно
}

/**
 * @brief enqueueCommand Добавляет пакет в очередь на отправку.
 * @param pack Пакет для отправки
 */
void ADFSerialInterface::enqueueCommand(PackageCommand *pc)
{
    /*
     * Вызов этого метода происходит в потоке, отличном от протокольного,
     * поэтому синхронизируем доступ к очереди.
     */
    //if (!_connected){
    //    pc->finishCommand(true);
    //   return;
    //}
    if (_queue.size() > SEND_QUEUE_LEN_MAX)
    {
        Message(MESSAGE_TYPE_CONNECTION, MESSAGE_WARNING, MESSAGE_TARGET_DEBUG,
                "Очередь отправки переполнена!");
        pc->finishCommand(true);
        return;
    }
    {  // Locking in the RAII style
        QMutexLocker locker(&sendQueueMutex);

        std::priority_queue<PackageCommand *, std::vector<PackageCommand *>, CustomCompare> tmp_q = _queue;  //copy the original queue to the temporary queue

        while (!tmp_q.empty())
        {
            PackageCommand *q_element = tmp_q.top();
            if (q_element->package_out->command() == pc->package_out->command())
            {
                if (q_element->package_out->_data == pc->package_out->_data)
                {
                    pc->finishCommand(true);
                    return;
                }
            }
            tmp_q.pop();
        }

        _queue.push(pc);
    }

    // Посылаем сигнал только если очередь раньше была пуста
    emit couldWrite();
}
