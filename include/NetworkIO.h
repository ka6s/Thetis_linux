#ifndef NETWORKIO_H
#define NETWORKIO_H

#include <QObject>
#include <QUdpSocket>

class Console;

class NetworkIO : public QObject {
    Q_OBJECT

public:
    explicit NetworkIO(Console* console, QObject* parent = nullptr);
    ~NetworkIO();

    void setHost(const QString& host, int port);
    void setFrequency(double freq);
    void setGain(double gain);

public slots:
    void start();
    void stop();

signals:
    void spectrumDataAvailable(const float* data, int size);
    void errorOccurred(const QString& error);

private slots:
    void processPendingDatagrams();
    void processIQData(const QByteArray& data);

private:
    Console* console_;
    QUdpSocket* udpSocket_;
    QString host_;
    int port_;
    double frequency_;
    double gain_;
    std::vector<float> iqBuffer_;
    static const int BUFFER_SIZE = 8192;
    static const int FFT_SIZE = 1024;
    bool running_;
    void computeSpectrum(const float* iqData, int size);
};

#endif // NETWORKIO_H
