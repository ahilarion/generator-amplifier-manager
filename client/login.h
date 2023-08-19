#ifndef LOGIN_H
#define LOGIN_H

#include <QDialog>
#include <QRegExp>
#include <QIntValidator>
#include "settings.h"

namespace Ui {
class Login;
}

class Login : public QDialog
{
    Q_OBJECT

public:
    explicit Login(QWidget *parent = nullptr);

    QString getGeneratorIp();
    QString getXPortIp();
    QString getAmplifier4GIp();
    QString getAmplifier5GIp();

    ushort getGeneratorPort();
    ushort getAmplifier4GPort();
    ushort getAmplifier5GPort();
    ushort getXPortPort();

    ~Login();

private:
    Ui::Login *ui;
    Settings *config;
};

#endif // LOGIN_H
