#ifndef RADIO_H
#define RADIO_H

#include <QObject>
#include <QString>

class Console;

class Radio : public QObject {
    Q_OBJECT

public:
    explicit Radio(Console* console, QObject* parent = nullptr);
    ~Radio();

    bool initialize();
    void setFrequency(qint64 freq, bool vfoA = true);
    void setMode(const QString& mode);
    void setFilter(int low, int high);
    void start();
    void stop();
    QString getHardwareType() const;

signals:
    void frequencyChanged(qint64 freq);
    void modeChanged(const QString& mode);

private:
    Console* console_;
    QString hardwareType_;
    bool initialized_;
};

#endif // RADIO_H
