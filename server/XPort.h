#ifndef XPORT_H
#define XPORT_H

#include "TCPServer.h"
#include <qobject.h>
#include <QTime>

class XPort : public QObject {
    Q_OBJECT
public:
    XPort(ushort port, QObject *parent = nullptr) : QObject(parent) {
        server = new TCPServer(port, this);
        connect(server, &TCPServer::readyReadSignal, this, &XPort::handleNewMessage);
    };

    QByteArray decode(QByteArray data){
        if(data == QByteArray::fromHex("1B0700000007000000")) {
            currentStatus = 0;
        } else if ( data == QByteArray::fromHex("1B0700000000000000")){
            currentStatus = 1;
        }

        return data;
    };

    int getCurrentStatus() {
        return currentStatus;
    }

    ~XPort() {
        delete server;
    };

public slots:
    void handleNewMessage(QTcpSocket *client) {
        while (client->bytesAvailable() > 0) {
            QByteArray lineData = client->readLine();

            qDebug() << "[" + QTime::currentTime().toString() + "][" + QHostAddress(client->peerAddress().toIPv4Address()).toString() + "] => Received : " + lineData.toHex(' ').toUpper();

            QByteArray response = decode(lineData);

            if(!response.isEmpty()) {
                qDebug() << "[" + QTime::currentTime().toString() + "][" + QHostAddress(client->peerAddress().toIPv4Address()).toString() + "] => Sended : " + response.toHex(' ').toUpper();
                client->write(response);
                client->flush();
            }

        }
    };

private:
    TCPServer *server;
    int currentStatus = 0;
};

#endif

