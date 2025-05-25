#ifndef CONSOLE_H
#define CONSOLE_H

#include <QObject>
#include <QString>
#include <QTcpSocket>

class Console : public QObject {
    Q_OBJECT

public:
    explicit Console(QObject* parent = nullptr);
    ~Console();

    void setSampleRate(int rate);
    int getSampleRate() const;
    QString getRX1DSPMode() const;
    double getVFOAFreq() const;
    QString getAppDataPath() const;
    void setWavePlayback(bool enabled);
    void setWaveRecord(bool enabled);
    void setWavePreamp(double gain);
    void setFrequency(qint64 freq);
    void setMode(const QString& mode);
    void setFilterBandwidth(int bandwidth);
    void tci_cmd(const QString& command, QTcpSocket* client); // Added

private:
    int sampleRate_;
    QString appDataPath_;
    qint64 frequency_;
    QString rx1DSPMode_;
    int filterBandwidth_;
};

#endif // CONSOLE_H
