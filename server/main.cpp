
#include "window.h"

#include <QApplication>
#include <QLockFile>
#include <QMessageBox>
#include <QDir>
#include <QScreen>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    window w;
    QLockFile lockFile(QDir::temp().absoluteFilePath("simulator.lock"));

    if(!lockFile.tryLock(100)) {
        QMessageBox::critical(NULL, "Erreur", "Cette application est déjà en cours d'execution !");
        return 1;
    }

    int x, y;

    QRect screenSize = QGuiApplication::primaryScreen()->geometry();

    w.setFixedSize(840, 490);
    w.setWindowIcon(QIcon(":/server.png"));

    x = (screenSize.width() - w.width()) / 2;
    y = (screenSize.height() - w.height()) / 2;

    w.move(x, y);
    w.setWindowTitle("Simulateur");


    w.show();
    return a.exec();
}
