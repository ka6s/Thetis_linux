#ifndef VFO_H
#define VFO_H

#include <QObject>
#include <QStringList>

class Console;

class VFO : public QObject {
    Q_OBJECT

public:
    explicit VFO(Console* console, QObject* parent = nullptr);
    ~VFO();

    void setFrequency(qint64 freq);
    qint64 getFrequency() const;
    void setVFOMode(const QString& mode);
    QStringList getVFOModes() const;
    void setStepSize(int step);
    int getStepSize() const;

private:
    Console* console_;
    qint64 frequency_;
    QString vfoMode_;
    QStringList vfoModes_;
    int stepSize_;
};

#endif // VFO_H
