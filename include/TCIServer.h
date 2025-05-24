#ifndef TCISERVER_H
#define TCISERVER_H

#include <QTcpServer>
#include <QTcpSocket>
#include <QThread>
#include <QObject>
#include <QString>
#include <QList>

class Console; // Forward declaration

class TCPIPtciSocketListener : public QObject {
    Q_OBJECT
public:
    explicit TCPIPtciSocketListener(int port, Console* console, QObject* parent = nullptr);
    ~TCPIPtciSocketListener();
    void Start();
    void Stop();
signals:
    void errorOccurred(const QString& error);
private slots:
    void handleNewConnection();
    void handleClientDisconnected();
    void handleClientData();
private:
    void listen();
    void talk(QTcpSocket* client);
    QTcpServer* server_;
    int port_;
    Console* console_;
    QList<QTcpSocket*> clients_;
    bool running_;
};

class ClientThread : public QThread {
    Q_OBJECT
public:
    ClientThread(QTcpSocket* socket, Console* console, QObject* parent = nullptr);
    ~ClientThread();
protected:
    void run() override;
private:
    QTcpSocket* socket_;
    Console* console_;
};

#endif // TCISERVER_H
