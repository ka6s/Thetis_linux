#ifndef CAT_H
#define CAT_H

#include <QObject>
#include <QSerialPort>
#include <QString>

class Console; // Forward declaration

class CAT : public QObject {
    Q_OBJECT

public:
    explicit CAT(Console* console, QObject* parent = nullptr);
    ~CAT();

    bool openSerialPort(const QString& portName, qint32 baudRate = 9600);
    void closeSerialPort();
    void sendCommand(const QString& command);

private slots:
    void handleSerialData();
    void handleSerialError(QSerialPort::SerialPortError error);

private:
    void parseCommand(const QString& command);

    Console* console_;
    QSerialPort* serialPort_;
    QString buffer_;
};

#endif // CAT_H
