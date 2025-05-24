#ifndef CONSOLE_H
#define CONSOLE_H

#include <QObject>
#include <QTcpSocket>
#include <QString>

class Console : public QObject {
    Q_OBJECT

public:
    explicit Console(QObject* parent = nullptr);
    ~Console();

    void tci_cmd(const QString& cmd, QTcpSocket* client);
    void setCWTone(bool state);
    void setCWToneDuration(int duration);
    void setSampleRate(int rate);
    void setAudioMode(const QString& mode);
    void setFrequency(qint64 freq);
    void setMode(const QString& mode);
    void setFilterBandwidth(int bandwidth);
    int sampleRate() const { return sampleRate_; }
    int channel() const { return channel_; }

private:
    void initializeWDSP();
    int channel_;
    int sampleRate_;
    QString audioMode_;
    qint64 frequency_;
    QString mode_;
};

#endif // CONSOLE_H
