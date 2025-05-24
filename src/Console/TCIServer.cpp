#include "TCIServer.h"
#include "Console.h"
#include <QDebug>

TCPIPtciSocketListener::TCPIPtciSocketListener(int port, Console* console, QObject* parent)
    : QObject(parent), port_(port), console_(console), running_(false) {
    server_ = new QTcpServer(this);
    connect(server_, &QTcpServer::newConnection, this, &TCPIPtciSocketListener::handleNewConnection);
}

TCPIPtciSocketListener::~TCPIPtciSocketListener() {
    Stop();
    for (QTcpSocket* client : clients_) {
        client->deleteLater();
    }
}

void TCPIPtciSocketListener::Start() {
    if (!running_) {
        running_ = true;
        listen();
    }
}

void TCPIPtciSocketListener::Stop() {
    if (running_) {
        running_ = false;
        for (QTcpSocket* client : clients_) {
            client->disconnectFromHost();
            client->deleteLater();
        }
        clients_.clear();
        server_->close();
    }
}

void TCPIPtciSocketListener::listen() {
    if (!server_->listen(QHostAddress::Any, port_)) {
        emit errorOccurred("Failed to start server on port " + QString::number(port_));
        running_ = false;
        return;
    }
    qDebug() << "Server listening on port" << port_;
}

void TCPIPtciSocketListener::handleNewConnection() {
    while (server_->hasPendingConnections()) {
        QTcpSocket* client = server_->nextPendingConnection();
        clients_.append(client);
        connect(client, &QTcpSocket::disconnected, this, &TCPIPtciSocketListener::handleClientDisconnected);
        connect(client, &QTcpSocket::readyRead, this, &TCPIPtciSocketListener::handleClientData);
        qDebug() << "New client connected:" << client->peerAddress().toString();

        // Start a new thread for the client
        ClientThread* thread = new ClientThread(client, console_, this);
        thread->start();
    }
}

void TCPIPtciSocketListener::handleClientDisconnected() {
    QTcpSocket* client = qobject_cast<QTcpSocket*>(sender());
    if (client) {
        clients_.removeAll(client);
        client->deleteLater();
        qDebug() << "Client disconnected:" << client->peerAddress().toString();
    }
}

void TCPIPtciSocketListener::handleClientData() {
    QTcpSocket* client = qobject_cast<QTcpSocket*>(sender());
    if (client) {
        talk(client);
    }
}

void TCPIPtciSocketListener::talk(QTcpSocket* client) {
    while (client->bytesAvailable() > 0) {
        QByteArray data = client->readAll();
        QString command = QString::fromUtf8(data).trimmed();
        qDebug() << "Received command:" << command;

        // Process the command via Console
        console_->tci_cmd(command, client);
    }
}

ClientThread::ClientThread(QTcpSocket* socket, Console* console, QObject* parent)
    : QThread(parent), socket_(socket), console_(console) {
}

ClientThread::~ClientThread() {
    socket_->deleteLater();
}

void ClientThread::run() {
    // Thread loop for signal processing
    exec();
}
