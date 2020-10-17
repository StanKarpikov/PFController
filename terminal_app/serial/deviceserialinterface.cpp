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
#include "deviceserialinterface.h"
#include "interface_messaging.h"
#include <queue>
using namespace std;
using namespace InterfaceMessaging;

PFCSerialInterface::PFCSerialInterface(QObject *parent)
{
    Q_UNUSED(parent)
    qRegisterMetaType<std::vector<DeviceSerialMessage>>("std::vector<Package>");
    connect(this, &PFCSerialInterface::informConnectionChanged,
            this, &PFCSerialInterface::connectionHasChanged);
}

PFCSerialInterface::~PFCSerialInterface()
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
void PFCSerialInterface::ConnectTo(
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
    Q_UNUSED(localEchoEnabled)
    Q_UNUSED(timeout)
    Q_UNUSED(numberOfRetries)
    if (_serial->isOpen())
    {
        message(MESSAGE_TYPE_CONNECTION, MESSAGE_NORMAL, MESSAGE_TARGET_ALL,
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
        message(MESSAGE_TYPE_CONNECTION, MESSAGE_NORMAL, MESSAGE_TARGET_ALL,
                (QString("Port %1 opened: %2").arg(name).arg(QString::number(baudRate))).toStdString());
        emit connected();
    }
    else
    {
        message(MESSAGE_TYPE_CONNECTION, MESSAGE_WARNING, MESSAGE_TARGET_ALL,
                (QString("Error opening port %1: %2").arg(name).arg(QString::number(baudRate))).toStdString());
    }
}
void PFCSerialInterface::disconnectFromDevice()
{
    if (_serial->isOpen())
    {
        _serial->close();
        message(MESSAGE_TYPE_CONNECTION, MESSAGE_NORMAL, MESSAGE_TARGET_ALL,
                "Port closed");
        emit disconnected();
    }
    else
    {
        message(MESSAGE_TYPE_CONNECTION, MESSAGE_WARNING, MESSAGE_TARGET_DEBUG,
                "Trying to close a port that has not been opened!");
        emit disconnected();
    }
}
void PFCSerialInterface::handleError(QSerialPort::SerialPortError err)
{
    if (err == 0) return;
    uint32_t target = MESSAGE_TARGET_ALL;
    if (noError == 0) target = MESSAGE_TARGET_DEBUG;
    noError = 0;
    message(MESSAGE_TYPE_CONNECTION, MESSAGE_ERROR, MESSAGE_TARGET_DEBUG,
            (QString("Port error: %1(%2)")
                .arg(err)
                .arg(_serial->errorString())).toStdString());
    if (err == 12)
    {
        message(MESSAGE_TYPE_CONNECTION, MESSAGE_ERROR, target,
                "Connection lost");
    }
    //Disconnect();
}
DeviceSerialMessage *PFCSerialInterface::serialReadPackage(int timeout)
{
    QElapsedTimer timer;
    vector<uint8_t> readBuf;

    timer.start();

    try
    {
        while (timer.elapsed() < timeout)
        {
            QByteArray portion = _serial->readAll();

            if (portion.size())
            {
                message(MESSAGE_TYPE_CONNECTION, MESSAGE_NORMAL, MESSAGE_TARGET_NONE,
                        (QString("GET : ").append(QString::fromStdString(hex_dump(
                            std::vector<uint8_t>(
                                portion.begin(), portion.end()))))).toStdString());

                readBuf.reserve(readBuf.size() + portion.size());
                copy(portion.begin(), portion.end(), back_inserter(readBuf));

                DeviceSerialMessage *response = DeviceSerialMessage::popFromBuffer(readBuf);

                if (response)
                {
                    if (noError == 0)
                    {
                        message(MESSAGE_TYPE_CONNECTION, MESSAGE_NORMAL, MESSAGE_TARGET_ALL,
                                "Connection restored");
                    }
                    noError = 1;
                    return response;
                }
            }
            _serial->waitForReadyRead(timeout - timer.elapsed());
        }
        message(MESSAGE_TYPE_CONNECTION, MESSAGE_WARNING, MESSAGE_TARGET_DEBUG,
                "Timeout waiting for the answer");
    }
    catch (ProtocolException &e)
    {
        message(MESSAGE_TYPE_CONNECTION, MESSAGE_ERROR, MESSAGE_TARGET_DEBUG,
                (QString("Exception %1")
                    .arg(e.what())).toStdString());
    }

    return Q_NULLPTR;
}

void PFCSerialInterface::run()
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
    connect(_serial, static_cast<void(QSerialPort::*)(QSerialPort::SerialPortError)>(&QSerialPort::error),
            this, &PFCSerialInterface::handleError);
    connect(this, &PFCSerialInterface::couldWrite,
            this, &PFCSerialInterface::sendQueue, Qt::QueuedConnection);
}

int PFCSerialInterface::serialWrite(const vector<uint8_t> &dataToWrite)
{
    auto dataSize = dataToWrite.size();

    // C11++ style - get raw data from vector
    const char *data = reinterpret_cast<const char *>(&dataToWrite.data()[0]);

    if (!_serial || !_serial->isOpen())
    {
        message(MESSAGE_TYPE_CONNECTION, MESSAGE_ERROR, MESSAGE_TARGET_NONE,
                "Port not opened");
        return -1;
    }
    auto written = _serial->write(data, dataSize);

    if (written < 0)
    {
        message(MESSAGE_TYPE_CONNECTION, MESSAGE_ERROR, MESSAGE_TARGET_DEBUG,
                "Write error");
        return -1;
    }
    else if (written != dataSize)
    {
        message(MESSAGE_TYPE_CONNECTION, MESSAGE_ERROR, MESSAGE_TARGET_DEBUG,
                "Write error: not all");
        return -1;
    }

    if (!_serial->waitForBytesWritten(3000))
    {
        message(MESSAGE_TYPE_CONNECTION, MESSAGE_ERROR, MESSAGE_TARGET_DEBUG,
                "Write error: wait");
        return -1;
    }

    return static_cast<int>(written);
}

std::string PFCSerialInterface::hex_dump(const std::vector<uint8_t> &buf)
{
    ostringstream oss;
    for (uint i = 0; i < buf.size(); i++)
        oss << " " << std::uppercase << setfill('0') << setw(2) << std::hex << buf[i];
    return oss.str();
}

void PFCSerialInterface::sendQueue()
{
    int written;
    InterfacePackage *pc = Q_NULLPTR;

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

    const vector<uint8_t> &dataToWrite = pc->package_to_send->toBuffer();

    written = serialWrite(dataToWrite);
    if (written < 0)
    {
        pc->finishProcessing(true);
        emit informConnectionChanged(false);
        return;
    }
    message(MESSAGE_TYPE_CONNECTION, MESSAGE_NORMAL, MESSAGE_TARGET_NONE,
            (QString("SENT: ").append(QString::fromStdString(hex_dump(dataToWrite)))).toStdString());

    if (pc->package_read)
    {
        delete pc->package_read;
        pc->package_read = Q_NULLPTR;
    }

    pc->package_read = this->serialReadPackage(READ_TIMEOUT_MS);

    if (pc->package_read)
    {
        message(MESSAGE_TYPE_CONNECTION, MESSAGE_NORMAL, MESSAGE_TARGET_NONE,
                (QString("RECEIVED: ").append(QString::fromStdString(hex_dump(pc->package_read->data())))).toStdString());
        emit informConnectionChanged(true);
        pc->finishProcessing(false);
    }
    else
    {
        message(MESSAGE_TYPE_CONNECTION, MESSAGE_ERROR, MESSAGE_TARGET_DEBUG,
                "Timeout is esceeded!");
        pc->finishProcessing(true);
        emit informConnectionChanged(false);
    }

    if (!_queue.empty()) emit couldWrite(); /* TODO: Check if this is redundant */
}

void PFCSerialInterface::connectionHasChanged(bool connected)
{
    _connected = connected;
}

void PFCSerialInterface::enqueueCommand(InterfacePackage *pc)
{
    //if (!_connected){
    //    pc->finishCommand(true);
    //   return;
    //}
    if (_queue.size() > SEND_QUEUE_LEN_MAX)
    {
        message(MESSAGE_TYPE_CONNECTION, MESSAGE_WARNING, MESSAGE_TARGET_DEBUG,
                "The send queue is full!");
        pc->finishProcessing(true);
        return;
    }
    {  // Locking in the RAII style
        QMutexLocker locker(&sendQueueMutex);

        std::priority_queue<InterfacePackage *, std::vector<InterfacePackage *>, CustomCompare> tmp_q = _queue;

        while (!tmp_q.empty())
        {
            InterfacePackage *q_element = tmp_q.top();
            if (q_element->package_to_send->command() == pc->package_to_send->command())
            {
                if (q_element->package_to_send->data() == pc->package_to_send->data())
                {
                    pc->finishProcessing(true);
                    return;
                }
            }
            tmp_q.pop();
        }

        _queue.push(pc);
    }

    emit couldWrite();
}
