#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <QTcpSocket>

class TCPClient : public QObject {
    Q_OBJECT
public:
    TCPClient() {
        socket = new QTcpSocket();
        connect(socket, &QTcpSocket::connected, this, &TCPClient::onConnect);
        connect(socket, &QTcpSocket::disconnected, this, &TCPClient::onDisconnect);
    };

    ~TCPClient() {
        close();
        delete socket;
    };

    bool join(QString ip, ushort port) {
        socket->connectToHost(ip, port);
        if(socket->waitForConnected(300))
            return true;
        return false;
    }

    void close() {
        if(isConnected)
            socket->close();
    }

    bool send(QByteArray f) {
        socket->write(f);
        socket->flush();
        if(isConnected)
            return true;
        return false;
    }

    QString read() {
        if (isConnected && socket->waitForReadyRead(100)) {
            QByteArray data = socket->readLine();
            QString dataString(data);
            qDebug() << dataString;
            return dataString;
        }
        return QString();
    }

    bool getConnectionStatus() {return isConnected;}

public slots:
    void onDisconnect(){
        isConnected = false;
    };
    void onConnect(){
        isConnected = true;
    };
private:
    bool isConnected;
    QTcpSocket *socket;
};

#endif // TCPCLIENT_H
