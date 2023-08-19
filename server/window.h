#ifndef WINDOW_H
#define WINDOW_H

#include <QMainWindow>
#include <QTimer>
#include "Generator.h"
#include "XPort.h"
#include "Amplifier.h"
#include "settings.h"
#include "TCPServer.h"

QT_BEGIN_NAMESPACE
namespace Ui { class window; }
QT_END_NAMESPACE

class window : public QMainWindow

{
    Q_OBJECT

public:
    window(QWidget *parent = nullptr);
    ~window();

private slots:
    void on_startButton_clicked();
    void on_changeButton_clicked();
    void on_stopButton_clicked();
    void update();

private:
    Ui::window *ui;
    Generator *generator;
    XPort *xport;
    Amplifier *amplifier4G, *amplifier5G;
    QTimer *timer;
    Settings *config;
    ushort portGenerator, portAmplifier4G, portAmplifier5G, portXPort;
    int currentAmpli;
    QString ip_server;
};

#endif // WINDOW_H
