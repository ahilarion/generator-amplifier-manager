#ifndef XPORT_H
#define XPORT_H

#include <TCPClient.h>

class XPort {
public:
    XPort(){
        client = new TCPClient();
    };

    bool join(QString ip, ushort port) {
        bool r = client->join(ip, port);
        QByteArray configFrameOne = QByteArray::fromHex("190700000007000000");
        QByteArray configFrameTwo = QByteArray::fromHex("1A0700000007000000");

        if(r) {
            client->send(configFrameOne);
            client->read();
            client->send(configFrameTwo);
            client->read();
        }

        return r;
    }

    void disconnect() {
        client->close();
    }

    void setXPortOutput(int n) {
        QByteArray command;
        if(n == 1)
            command = QByteArray::fromHex("1B0700000000000000");
        else if(n == 0)
            command = QByteArray::fromHex("1B0700000007000000");

        client->send(command);
        client->read();
    }

    bool getConnectionStatus() {
        return client->getConnectionStatus();
    }

    ~XPort(){
        client->close();
        delete client;
    }
private:
    TCPClient *client;
};

#endif // XPORT_H
