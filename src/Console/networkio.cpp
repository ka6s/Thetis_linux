#include <NetworkIO.h>
#include <Console.h>
#include <wdsp.h>
#include <QDebug>
#include <cmath>

NetworkIO::NetworkIO(Console* console, QObject* parent)
    : QObject(parent),
      console_(console),
      socket_(new QUdpSocket(this)),
      packetCount_(0) {
    connect(socket_, &QUdpSocket::readyRead, this, &NetworkIO::readPendingDatagrams);
    qDebug() << "NetworkIO constructed";
}

NetworkIO::~NetworkIO() {
    socket_->close();
    qDebug() << "NetworkIO destructed";
}

bool NetworkIO::initialize(const QString& host, quint16 port) {
    radioAddress_ = QHostAddress(host);
    radioPort_ = port;

    if (!socket_->bind(QHostAddress::Any, 0, QAbstractSocket::DefaultForPlatform)) {
        qDebug() << "Failed to bind UDP socket:" << socket_->errorString();
        return false;
    }
    qDebug() << "NetworkIO initialized for" << host << ":" << port;
    return true;
}

void NetworkIO::sendCommand(const QString& command) {
    QByteArray data = command.toUtf8();
    qint64 bytesSent = socket_->writeDatagram(data, radioAddress_, radioPort_);
    if (bytesSent == data.size()) {
        qDebug() << "Sent network command:" << command;
    } else {
        qDebug() << "Failed to send command:" << command << socket_->errorString();
    }
}

void NetworkIO::readPendingDatagrams() {
    while (socket_->hasPendingDatagrams()) {
        QByteArray datagram;
        datagram.resize(socket_->pendingDatagramSize());
        QHostAddress sender;
        quint16 senderPort;

        qint64 bytesRead = socket_->readDatagram(datagram.data(), datagram.size(), &sender, &senderPort);
        if (bytesRead == -1) {
            qDebug() << "Failed to read datagram:" << socket_->errorString();
            continue;
        }

        packetCount_++;
        if (packetCount_ <= 5) {
            qDebug() << "Received datagram #" << packetCount_ << "from" << sender.toString() << ":" << senderPort << "size:" << datagram.size();
        }

        if (datagram.startsWith("CTRL")) {
            QString response = QString::fromUtf8(datagram);
            qDebug() << "Received control response:" << response;
        } else {
            processIQData(datagram);
        }
    }
}

void NetworkIO::processIQData(const QByteArray& data) {
    if (data.size() % 8 != 0) {
        qDebug() << "Invalid IQ data size:" << data.size();
        return;
    }

    int samples = data.size() / 8;
    float* Iin = new float[samples];
    float* Qin = new float[samples];
    float* Iout = new float[samples];
    float* Qout = new float[samples];

    for (int i = 0; i < samples; i++) {
        Iin[i] = *(float*)(data.constData() + i * 8);
        Qin[i] = *(float*)(data.constData() + i * 8 + 4);
    }

    int error = 0;
    // fexchange2(console_->channel(), Iin, Qin, Iout, Qout, &error);
    if (error != 0) {
        qDebug() << "WDSP fexchange2 error:" << error;
    }

    QVector<float> spectrumData(256);
    for (int i = 0; i < 256 && i < samples; ++i) {
        spectrumData[i] = (Iout[i] * Iout[i] + Qout[i] * Qout[i]) * 0.5;
    }

    emit iqDataReceived(spectrumData);

    delete[] Iin;
    delete[] Qin;
    delete[] Iout;
    delete[] Qout;

    if (packetCount_ <= 5) {
        qDebug() << "Processed" << samples << "IQ samples";
    }
}

void NetworkIO::simulateIQData() {
    int samples = 256;
    float* Iin = new float[samples];
    float* Qin = new float[samples];
    float* Iout = new float[samples];
    float* Qout = new float[samples];

    for (int i = 0; i < samples; ++i) {
        Iin[i] = sin(i * 0.1 + packetCount_ * 0.1);
        Qin[i] = cos(i * 0.1 + packetCount_ * 0.1);
        Iout[i] = Iin[i];
        Qout[i] = Qin[i];
    }

    QVector<float> spectrumData(256);
    for (int i = 0; i < 256; ++i) {
        spectrumData[i] = (Iout[i] * Iout[i] + Qout[i] * Qout[i]) * 0.5;
    }

    emit iqDataReceived(spectrumData);

    delete[] Iin;
    delete[] Qin;
    delete[] Iout;
    delete[] Qout;

    packetCount_++;
    if (packetCount_ <= 5) {
        qDebug() << "Simulated" << samples << "IQ samples";
    }
}
