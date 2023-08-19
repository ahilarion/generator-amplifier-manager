#ifndef SETTINGS_H
#define SETTINGS_H

#include <QSettings>
#include <QString>
#include <QFileInfo>

class Settings
{
public:
    Settings() {
        settings = new QSettings("config.ini", QSettings::IniFormat);
        settings->sync();
    };
    void setValue(QString name, QVariant value) {
        settings->setValue(name, value);
        settings->sync();
    };
    QString getValue(QString name, QVariant defaultValue) {
        return settings->value(name, defaultValue).toString();
    };
    ~Settings(){
        delete settings;
    };
private:
    QSettings *settings;
};

#endif // SETTINGS_H
