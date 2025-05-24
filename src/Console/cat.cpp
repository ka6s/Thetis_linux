#include <CAT.h>
#include <Console.h>
#include <QDebug>

CAT::CAT(Console* console, QObject* parent)
    : QObject(parent),
      console_(console),
      serialPort_(new QSerialPort(this)) {
    connect(serialPort_, &QSerialPort::readyRead, this, &CAT::handleSerialData);
    connect(serialPort_, &QSerialPort::errorOccurred, this, &CAT::handleSerialError);
}

CAT::~CAT() {
    closeSerialPort();
}

bool CAT::openSerialPort(const QString& portName, qint32 baudRate) {
    serialPort_->setPortName(portName);
    serialPort_->setBaudRate(baudRate);
    serialPort_->setDataBits(QSerialPort::Data8);
    serialPort_->setParity(QSerialPort::NoParity);
    serialPort_->setStopBits(QSerialPort::OneStop);
    serialPort_->setFlowControl(QSerialPort::NoFlowControl);

    if (!serialPort_->open(QIODevice::ReadWrite)) {
        qDebug() << "Failed to open serial port:" << serialPort_->errorString();
        return false;
    }
    qDebug() << "Serial port opened:" << portName;
    return true;
}

void CAT::closeSerialPort() {
    if (serialPort_->isOpen()) {
        serialPort_->close();
        qDebug() << "Serial port closed";
    }
}

void CAT::sendCommand(const QString& command) {
    if (serialPort_->isOpen()) {
        QByteArray data = (command + ";").toUtf8();
        serialPort_->write(data);
        qDebug() << "Sent CAT command:" << command;
    }
}

void CAT::handleSerialData() {
    buffer_ += QString::fromUtf8(serialPort_->readAll());
    int endIndex = buffer_.indexOf(';');
    while (endIndex != -1) {
        QString command = buffer_.left(endIndex);
        buffer_.remove(0, endIndex + 1);
        parseCommand(command);
        endIndex = buffer_.indexOf(';');
    }
}

void CAT::handleSerialError(QSerialPort::SerialPortError error) {
    if (error != QSerialPort::NoError) {
        qDebug() << "Serial port error:" << serialPort_->errorString();
        closeSerialPort();
    }
}

void CAT::parseCommand(const QString& command) {
    if (command.isEmpty()) return;

    qDebug() << "Received CAT command:" << command;

    // Example Kenwood CAT commands (e.g., "FA00007000000;" for VFO-A frequency)
    if (command.startsWith("FA")) {
        // Set VFO-A frequency (e.g., "FA00007000000;" = 7 MHz)
        QString freqStr = command.mid(2, 11);
        bool ok;
        qint64 freq = freqStr.toLongLong(&ok);
        if (ok) {
            console_->setFrequency(freq);
            sendCommand(QString("FA%1;").arg(freq, 11, 10, QChar('0')));
        }
    } else if (command.startsWith("MD")) {
        // Set mode (e.g., "MD1;" = LSB)
        QString mode = command.mid(2, 1);
        console_->setMode(mode);
        sendCommand(command);
    } else if (command.startsWith("IF")) {
        // Information request
        // Placeholder: Respond with radio state
        sendCommand("IF00007000000     +0000 0000000000000;");
    } else {
        qDebug() << "Unsupported CAT command:" << command;
    }
}
