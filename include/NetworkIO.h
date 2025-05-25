#ifndef NETWORKIO_H
#define NETWORKIO_H

#include <QObject>
#include <QUdpSocket>
#include <QHostAddress>

class Console;

class NetworkIO : public QObject {
    Q_OBJECT

public:
    explicit NetworkIO(Console* console, QObject* parent = nullptr);
    ~NetworkIO();

    bool initialize(const QString& host, quint16 port);
    void sendCommand(const QString& command);
    void simulateIQData();

signals:
    void iqDataReceived(const QVector<float>& spectrumData);

private slots:
    void readPendingDatagrams();

private:
    void processIQData(const QByteArray& data);

    Console* console_;
    QUdpSocket* socket_;
    QHostAddress radioAddress_;
    quint16 radioPort_;
    qint64 packetCount_;
};

#endif // NETWORKIO_H
