#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QDebug>
#include <QTime>
#include <QNetworkInterface>

class TCPServer : public QObject {
    Q_OBJECT
public:
    explicit TCPServer(ushort port, QObject *parent = nullptr) : QObject(parent), server(nullptr) {
        server = new QTcpServer(this);

        if (getServerIP().isEmpty()) {
            qDebug() << "Impossible de trouver l'adresse IPv4 de la machine !";
        } else {
            if (!server->listen(QHostAddress::Any, port)) {
                qDebug() << "[" + QTime::currentTime().toString() + "] => Impossible de lancer le serveur !";
            } else {
                qDebug() << "[" + QTime::currentTime().toString() + "][" + getServerIP() + "] => Serveur allumé !";
            }
        }

        connect(server, &QTcpServer::newConnection, this, &TCPServer::handleNewConnection);
    }

    static QString getServerIP() {
        const QHostAddress &localhost = QHostAddress(QHostAddress::LocalHost);
        for (const QHostAddress &address: QNetworkInterface::allAddresses()) {
            if (address.protocol() == QAbstractSocket::IPv4Protocol && address != localhost && address.toString() != "192.168.56.1")
                return address.toString();
        }

        return QString();
    }

    ~TCPServer() {
        qDebug() << "[" + QTime::currentTime().toString() + "][" + getServerIP() + "]" + " => Serveur éteint !";
    }

signals:
    void readyReadSignal(QTcpSocket *clientSocket);

public slots:
    void handleNewConnection() {
        QTcpSocket* clientSocket = server->nextPendingConnection();
        countClient++;
        connect(clientSocket, &QTcpSocket::readyRead, this, &TCPServer::handleReadyRead);
        connect(clientSocket, &QTcpSocket::disconnected, this, &TCPServer::handleDisconnection);
        qDebug() << "[" + QTime::currentTime().toString() + "][" + QHostAddress(clientSocket->peerAddress().toIPv4Address()).toString() + "] => Client connecté !";
    }

    void handleDisconnection() {
        QTcpSocket *clientSocket = qobject_cast<QTcpSocket*>(sender());
        if(clientSocket) {
            clientSocket->deleteLater();
            countClient--;
            qDebug() << "[" + QTime::currentTime().toString() + "][" + QHostAddress(clientSocket->peerAddress().toIPv4Address()).toString() + "] => Client déconnecté !";
        }
    }

    void handleReadyRead() {
        QTcpSocket* clientSocket = qobject_cast<QTcpSocket*>(sender());
        if (clientSocket) {
            emit readyReadSignal(clientSocket);
        }
    }

private:
    QTcpServer* server;
    int countClient = 0;
};

#endif // TCPSERVER_H
