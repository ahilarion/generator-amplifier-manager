#include "login.h"
#include "ui_login.h"

Login::Login(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Login)
{
    ui->setupUi(this);

    config = new Settings();

    QRegularExpression regex("^((25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)$");

    QRegularExpressionValidator *validatorGen = new QRegularExpressionValidator(regex, ui->lineEdit_login_ip_generator);
    QRegularExpressionValidator *validatorAmp4G = new QRegularExpressionValidator(regex, ui->lineEdit_login_ip_amplifier_4G);
    QRegularExpressionValidator *validatorAmp5G = new QRegularExpressionValidator(regex, ui->lineEdit_login_ip_amplifier_5G);
    QRegularExpressionValidator *validatorXPort = new QRegularExpressionValidator(regex, ui->lineEdit_login_ip_xport);

    ui->lineEdit_login_ip_generator->setValidator(validatorGen);
    ui->lineEdit_login_ip_amplifier_4G->setValidator(validatorAmp4G);
    ui->lineEdit_login_ip_amplifier_5G->setValidator(validatorAmp5G);
    ui->lineEdit_login_ip_xport->setValidator(validatorXPort);

    ui->lineEdit_login_ip_generator->setText(config->getValue("ip_generator", "192.0.1.111"));
    ui->lineEdit_login_ip_amplifier_4G->setText(config->getValue("ip_amplifier_4G", "192.0.1.112"));
    ui->lineEdit_login_ip_amplifier_5G->setText(config->getValue("ip_amplifier_5G", "192.0.1.113"));
    ui->lineEdit_login_ip_xport->setText(config->getValue("ip_xport", "192.0.1.114"));

    ui->spinBox_login_port_generator->setValue(config->getValue("port_generator", 18).toUShort());
    ui->spinBox_login_port_amplifier_4G->setValue(config->getValue("port_amplifier_4G", 10001).toUShort());
    ui->spinBox_login_port_amplifier_5G->setValue(config->getValue("port_amplifier_5G", 10001).toUShort());
    ui->spinBox_login_port_xport->setValue(config->getValue("port_xport", 30704).toUShort());
}

Login::~Login()
{
    delete config;
    delete ui;
}

QString Login::getGeneratorIp(){return ui->lineEdit_login_ip_generator->text();}
QString Login::getAmplifier4GIp(){return ui->lineEdit_login_ip_amplifier_4G->text();}
QString Login::getAmplifier5GIp(){return ui->lineEdit_login_ip_amplifier_5G->text();}
QString Login::getXPortIp(){return ui->lineEdit_login_ip_xport->text();}

ushort Login::getGeneratorPort(){return ui->spinBox_login_port_generator->value();}
ushort Login::getAmplifier4GPort(){return ui->spinBox_login_port_amplifier_4G->value();}
ushort Login::getAmplifier5GPort(){return ui->spinBox_login_port_amplifier_5G->value();}
ushort Login::getXPortPort(){return ui->spinBox_login_port_xport->value();}
