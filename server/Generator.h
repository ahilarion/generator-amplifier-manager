#ifndef GENERATOR_H
#define GENERATOR_H

#include "TCPServer.h"
#include <qobject.h>
#include <QTime>

class Generator : public QObject{
    Q_OBJECT
    public:
        Generator(ushort port, QObject *parent = nullptr) : QObject(parent) {
            server = new TCPServer(port, this);
            connect(server, &TCPServer::readyReadSignal, this, &Generator::handleNewMessage);
        };

        QByteArray decode(QByteArray data){
            QString dataString = QString(data).trimmed();
            QString command;

            if (dataString.startsWith("FREQ")) {
                if (dataString.startsWith("FREQ?"))
                    command = QString("%1%2\r\n").arg(frequency).arg("000000");
                else {
                    int valueStartIndex = dataString.indexOf(' ') + 1;  // Index du premier caractère après l'espace
                    int valueEndIndex = dataString.indexOf('\n');      // Index du premier caractère de fin de ligne '\n'

                    QString value = dataString.mid(valueStartIndex, valueEndIndex - valueStartIndex);
                    frequency = value.toULongLong() / 1000000;
                }
            } else if (dataString.startsWith("POW")) {
                if (dataString.startsWith("POW?"))
                    command = QString("%1\r\n").arg(amplitude);
                else {
                    int valueStartIndex = dataString.indexOf(' ') + 1;  // Index du premier caractère après l'espace
                    int valueEndIndex = dataString.indexOf('\n');      // Index du premier caractère de fin de ligne '\n'

                    QString value = dataString.mid(valueStartIndex, valueEndIndex - valueStartIndex);
                    amplitude = value.toInt();
                }
            } else if (dataString.startsWith("OUTP")) {
                if (dataString.startsWith("OUTP?"))
                    command = QString("%1\r\n").arg(status ? "1" : "0");
                else {
                    int valueStartIndex = dataString.indexOf(' ') + 1;  // Index du premier caractère après l'espace
                    int valueEndIndex = dataString.indexOf('\n');      // Index du premier caractère de fin de ligne '\n'

                    QString value = dataString.mid(valueStartIndex, valueEndIndex - valueStartIndex);

                    if(value == "ON"){
                        status = true;
                    } else if(value == "OFF") {
                        status = false;
                    }
                }
            } else {
                command = "";
            }

            return command.toUtf8();
        };

        int getGeneratorFrequency() {
            return frequency;
        }

        int getGeneratorAmplitude() {
            return amplitude;
        }

        int getGeneratorStatus() {
            return status;
        }

        ~Generator() {
            delete server;
        };
    public slots:
        void handleNewMessage(QTcpSocket *client) {
            while (client->bytesAvailable() > 0) {
                QByteArray lineData = client->readLine();

                qDebug() << "[" + QTime::currentTime().toString() + "][" + QHostAddress(client->peerAddress().toIPv4Address()).toString() + "] => Received : " + lineData;

                QByteArray response = decode(lineData);

                if(!response.trimmed().isEmpty()) {
                    qDebug() << "[" + QTime::currentTime().toString() + "][" + QHostAddress(client->peerAddress().toIPv4Address()).toString() + "] => Sended : " + response;
                    client->write(response);
                    client->flush();
                }

            }
        }

    private:
        TCPServer *server;
        int frequency = 900;
        int amplitude = 10;
        bool status = false;
};

#endif // GENERATOR_H
