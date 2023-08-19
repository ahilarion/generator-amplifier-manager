#include "window.h"
#include "ui_window.h"


window::window(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::window)
{
    ui->setupUi(this);

    timer = new QTimer(this);
    config = new Settings();

    portGenerator = config->getValue("port_generator", 5000).toUShort();
    portAmplifier4G = config->getValue("port_amplifier_4g", 5001).toUShort();
    portAmplifier5G = config->getValue("port_amplifier_5g", 5002).toUShort();
    portXPort = config->getValue("port_xport", 5003).toUShort();

    ui->portSpin_Generator->setValue(portGenerator);
    ui->portSpin_Amp4G->setValue(portAmplifier4G);
    ui->portSpin_Amp5G->setValue(portAmplifier5G);
    ui->portSpin_XPort->setValue(portXPort);

    ui->infosLabel->setText(QString("Serveur éteint | Ip : N/A | Port : [GEN : N/A] - [4G : N/A] - [5G : N/A] - [XPORT : N/A]").toUpper());

    currentAmpli = 0;

    connect(timer, SIGNAL(timeout()), this, SLOT(update()));
}

window::~window()
{
    delete generator;
    delete amplifier4G;
    delete amplifier5G;
    delete xport;
    delete ui;
}

void window::update() {
    ui->infosLabel->setText(QString("Serveur allumé | Ip : %1 | Port : [GEN : %2] - [4G : %3] - [5G : %4] - [XPORT : %5]").arg(ip_server).arg(portGenerator).arg(portAmplifier4G).arg(portAmplifier5G).arg(portXPort).toUpper());

        currentAmpli = xport->getCurrentStatus();

        ui->freqGenLabel->setText(QString::number(generator->getGeneratorFrequency()) + " MHz");
        ui->ampGenLabel->setText(QString::number(generator->getGeneratorAmplitude()) + " dB");

        if(generator->getGeneratorStatus() == 1) {
            ui->ledGen->setStyleSheet("background-color: #3BB933; border-radius: 10px");
        } else {
            ui->ledGen->setStyleSheet("background-color: #B93333; border-radius: 10px");
        }

        ui->diodeLabel_1->setText("[" + QString(amplifier4G->getDiode(0)) + "]");
        ui->diodeLabel_2->setText("[" + QString(amplifier4G->getDiode(1)) + "]");

        if(currentAmpli == 0) {
            ui->led2G->setStyleSheet("background-color: #3BB933; border-radius: 10px");
            ui->led5G->setStyleSheet("background-color: #B93333; border-radius: 10px");

            ui->freq2G->setText(amplifier4G->getFrequencyBand(0) + " Hz");
            ui->freq5G->setText("2.6-3.3 GHz");

            if(generator->getGeneratorStatus() == 1){
                ui->channel2G_1->setText(QString::number(amplifier4G->getChannelValue(1)) + " mW");
                ui->channel2G_2->setText(QString::number(amplifier4G->getChannelValue(2)) + " mW");
                ui->channel2G_3->setText(QString::number(amplifier4G->getChannelValue(3)) + " mW");
                ui->channel2G_4->setText(QString::number(amplifier4G->getChannelValue(4)) + " mW");
                ui->channel2G_5->setText(QString::number(amplifier4G->getChannelValue(5)) + " mW");
                ui->channel2G_6->setText(QString::number(amplifier4G->getChannelValue(6)) + " mW");
            } else {
                ui->channel2G_1->setText("0 mW");
                ui->channel2G_2->setText("0 mW");
                ui->channel2G_3->setText("0 mW");
                ui->channel2G_4->setText("0 mW");
                ui->channel2G_5->setText("0 mW");
                ui->channel2G_6->setText("0 mW");
            }

            ui->channel5G_1->setText("0 mW");
            ui->channel5G_2->setText("0 mW");
            ui->channel5G_3->setText("0 mW");
            ui->channel5G_4->setText("0 mW");

        } else if (currentAmpli == 1) {
            ui->led2G->setStyleSheet("background-color: #B93333; border-radius: 10px");
            ui->led5G->setStyleSheet("background-color: #3BB933; border-radius: 10px");

            ui->freq2G->setText("900 Hz");
            ui->freq5G->setText(amplifier5G->getFrequencyBand(1) + " GHz");

            if(generator->getGeneratorStatus() == 1){
                ui->channel5G_1->setText(QString::number(amplifier5G->getChannelValue(1)) + " mW");
                ui->channel5G_2->setText(QString::number(amplifier5G->getChannelValue(2)) + " mW");
                ui->channel5G_3->setText(QString::number(amplifier5G->getChannelValue(3)) + " mW");
                ui->channel5G_4->setText(QString::number(amplifier5G->getChannelValue(4)) + " mW");
            } else {
                ui->channel5G_1->setText("0 mW");
                ui->channel5G_2->setText("0 mW");
                ui->channel5G_3->setText("0 mW");
                ui->channel5G_4->setText("0 mW");
            }

            ui->channel2G_1->setText("0 mW");
            ui->channel2G_2->setText("0 mW");
            ui->channel2G_3->setText("0 mW");
            ui->channel2G_4->setText("0 mW");
            ui->channel2G_5->setText("0 mW");
            ui->channel2G_6->setText("0 mW");

        }
}

void window::on_startButton_clicked()
{
    generator = new Generator(portGenerator, this);
    amplifier4G = new Amplifier(portAmplifier4G, this);
    amplifier5G = new Amplifier(portAmplifier5G, this);
    xport = new XPort(portXPort, this);

    ip_server = TCPServer::getServerIP();

    timer->start(200);

    ui->stopButton->setEnabled(true);
    ui->startButton->setEnabled(false);
    ui->changeButton->setEnabled(false);
}


void window::on_changeButton_clicked()
{
    portGenerator = ui->portSpin_Generator->value();
    portAmplifier4G = ui->portSpin_Amp4G->value();
    portAmplifier5G = ui->portSpin_Amp5G->value();
    portXPort = ui->portSpin_XPort->value();

    config->setValue("port_generator", portGenerator);
    config->setValue("port_amplifier_4g", portAmplifier4G);
    config->setValue("port_amplifier_5g", portAmplifier5G);
    config->setValue("port_xport", portXPort);
}


void window::on_stopButton_clicked()
{
    delete generator;
    delete xport;
    delete amplifier4G;
    delete amplifier5G;

    timer->stop();

    ui->stopButton->setEnabled(false);
    ui->startButton->setEnabled(true);
    ui->changeButton->setEnabled(true);

    ui->channel5G_1->setText("0 mW");
    ui->channel5G_2->setText("0 mW");
    ui->channel5G_3->setText("0 mW");
    ui->channel5G_4->setText("0 mW");

    ui->channel2G_1->setText("0 mW");
    ui->channel2G_2->setText("0 mW");
    ui->channel2G_3->setText("0 mW");
    ui->channel2G_4->setText("0 mW");
    ui->channel2G_5->setText("0 mW");
    ui->channel2G_6->setText("0 mW");

    ui->led2G->setStyleSheet("background-color: #B93333; border-radius: 10px");
    ui->led5G->setStyleSheet("background-color: #B93333; border-radius: 10px");

    ui->infosLabel->setText(QString("Serveur éteint | Ip : N/A | Port : [GEN : N/A] - [4G : N/A] - [5G : N/A] - [XPORT : N/A]").toUpper());
}

