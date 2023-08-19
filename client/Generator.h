#ifndef GENERATOR_H
#define GENERATOR_H

#include <TCPClient.h>

class Generator {
    public:
        Generator(){
            client = new TCPClient();
        };

        bool join(QString ip, ushort port) {
            return client->join(ip, port);
        }

        void disconnect() {
            client->close();
        }

        void setGeneratorOutput(int status) {
            QString command;
            status ? command = "OUTP ON\r\n" : command = "OUTP OFF\r\n";
            client->send(command.toUtf8());
        }

        void setGeneratorFrequency(int frequency) {
            QString command = QString("FREQ %1%2\r\n").arg(frequency).arg("000000");
            client->send(command.toUtf8());
        }

        void setGeneratorAmplitude(int amplitude) {
            QString command = QString("POW %1\r\n").arg(amplitude);
            client->send(command.toUtf8());
        }

        bool getGeneratorOutput() {
            client->send("OUTP?\r\n");
            return client->read().toInt();
        }

        int getGeneratorFrequency() {
            client->send("FREQ?\r\n");
            return client->read().toFloat() / 1000000;
        }

        int getGeneratorAmplitude() {
            client->send("POW?\r\n");
            return client->read().toFloat();
        }

        bool getConnectionStatus() {
            return client->getConnectionStatus();
        }

        ~Generator(){
            client->close();
            delete client;
        }
    private:
        TCPClient *client;
};

#endif // GENERATOR_H
