#ifndef AMPLIFIER_H
#define AMPLIFIER_H

#include "TCPServer.h"
#include <qobject.h>
#include <QTime>
#include <math.h>

class Amplifier : public QObject {
    Q_OBJECT
    public:
        Amplifier(ushort port, QObject *parent = nullptr) : QObject(parent) {
            server = new TCPServer(port, this);
            connect(server, &TCPServer::readyReadSignal, this, &Amplifier::handleNewMessage);
        };

        QByteArray decode(QByteArray data){
            QString dataString = QString(data).trimmed();
            QString command;

            if(dataString.startsWith("rf") && dataString.length() >= 10) {
                int cmd = dataString.midRef(2, 2).toInt();
                int path = dataString.midRef(4, 1).toInt();
                int value = dataString.midRef(5, 5).toInt();

                switch (cmd) {
                    case 50:
                        channelNumericWord[path] = value;
                        channelPowerValue[path] = (0.00006*pow(value, 2)) + (0.00006*value) - 0.7477;
                        command = dataString;
                        break;
                    case 52:
                        command = "rf" + QString::number(cmd) + QString::number(path) + QString("%1").arg(channelPowerValue[path], 5, 10, QChar('0'));
                        break;
                    case 53:
                        currentBand = value - 1;
                        command = dataString;
                        break;
                    case 54:
                        value == 1 ? diode[path - 1] = "B" : diode[path - 1] = "A";
                        command = dataString;
                        break;
                    default:
                        command = dataString;
                        break;
                }
            }

            return command.toUtf8();
        };

        QString getDiode(int n) {
            return diode[n];
        }

        int getChannelValue(int channel) {
            return channelPowerValue[channel];
        }

        QString getFrequencyBand(int n) {
            return frequencyBand[n][currentBand];
        }

        ~Amplifier() {
            delete server;
        };

    public slots:
        void handleNewMessage(QTcpSocket *client) {
            while (client->bytesAvailable() > 0) {
                QByteArray lineData = client->readLine();

                qDebug() << "[" + QTime::currentTime().toString() + "][" + QHostAddress(client->peerAddress().toIPv4Address()).toString() + "] => Received : " + lineData;

                QByteArray response = decode(lineData);

                if(!response.isEmpty()) {
                    qDebug() << "[" + QTime::currentTime().toString() + "][" + QHostAddress(client->peerAddress().toIPv4Address()).toString() + "] => Sended : " + response;
                    client->write(response);
                    client->flush();
                }

            }
        };

    private:
        TCPServer *server;
        int frequency = 900;
        int amplitude = 10;
        bool status = false;
        int channelNumericWord[6] = {0, 0, 0, 0, 0, 0};
        int channelPowerValue[6] = {0, 0, 0, 0, 0, 0};
        QString frequencyBand[2][6] = {{"700", "900", "1800", "2170", "2450", "2700"}, {"2.6-3.3", "3.4-3.6", "3.7-3.8"}};
        int currentBand = 0;
        QString diode[2] = {"A", "A"};
};

#endif
