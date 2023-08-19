#ifndef AMPLIFIER_H
#define AMPLIFIER_H

#include <TCPClient.h>

class Amplifier {
public:
    Amplifier(){
        client = new TCPClient();
    };

    bool join(QString ip, ushort port) {
        return client->join(ip, port);
    }

    void disconnect() {
        client->close();
    }

    void init() {
        QString command = "rf00000000\r\n";
        client->send(command.toUtf8());
        client->read();
        client->send(command.toUtf8());
        client->read();
    }

    void setChannelValue(int channel, int value) {
        QString command = QString("rf50%1%2\r\n").arg(channel).arg(value, 5, 10, QChar('0'));
        client->send(command.toUtf8());
        client->read();
    }

    void setFrequencyBand(int n) {
        QString command = QString("rf5300000%1\r\n").arg(n);
        client->send(command.toUtf8());
        client->read();
    }

    void setDiode1(QString c) {
        QString command;
        if(c == 'A')
            command = QString("rf54100000\r\n");
        else if(c == 'B')
            command = QString("rf54100001\r\n");

        client->send(command.toUtf8());
        client->read();
    }

    void setDiode2(QString c) {
        QString command;
        if(c == 'A')
            command = QString("rf54200000\r\n");
        else if(c == 'B')
            command = QString("rf54200001\r\n");

        client->send(command.toUtf8());
        client->read();
    }

    void sendRequestToGetChannelValue(int channel) {
        QString command = QString("rf52%1%2\r\n").arg(channel).arg("00000");
        client->send(command.toUtf8());
        channelValue[channel - 1] = client->read().midRef(5,5).toInt();
    }

    int getChannelValue(int channel) {
        return channelValue[channel];
    }

    bool getConnectionStatus() {
        return client->getConnectionStatus();
    }

    ~Amplifier(){
        client->close();
        delete client;
    }

private:
    TCPClient *client;
    int channelValue[6] = {0, 0, 0, 0, 0, 0};
};

#endif // AMPLIFIER_H
