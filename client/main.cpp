
#include "window.h"

#include <QApplication>
#include <QScreen>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    window w;

    // Déclaration des variables pour la position de la fenêtre
    int x, y;

    // Récupération de la taille de l'écran principal
    QRect screenSize = QGuiApplication::primaryScreen()->geometry();

    // Paramétrage de la fenêtre
    w.setFixedSize(1020,750); // Taille fixe de la fenêtre
    w.setWindowTitle("Client PériTox"); // Titre de la fenêtre
    //w.setWindowFlags(Qt::WindowStaysOnTopHint); // La fenêtre reste au premier plan

    // Calcul de la position de la fenêtre au centre de l'écran
    x = (screenSize.width() - w.width()) / 2;
    y = (screenSize.height() - w.height()) / 2;

    // Déplacement de la fenêtre à la position calculée
    w.move(x, y);

    // Affichage de la fenêtre
    w.show();

    // Boucle d'événements de l'application
    return a.exec();
}
