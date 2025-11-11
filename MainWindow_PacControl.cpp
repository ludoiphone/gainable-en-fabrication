#include "MainWindow.hpp"
#include <QPushButton>
#include <QDebug>

// ===================== Création des boutons =====================
void MainWindow::setupPacControl()
{
    // === Bouton Démarrage ===
    marche = new QPushButton("Démarrage", this);
    marche->setGeometry(600, 544, 128, 50);
    marche->setStyleSheet("font-size:15px; background-color:lime;");
    marche->hide();

    // === Bouton Consignes ===
    consigne = new QPushButton("Consignes", this);
    consigne->setGeometry(735, 544, 128, 50);
    consigne->setStyleSheet("font-size:15px; background-color:lightgray;");
    consigne->show();

    // === Bouton Stop ===
    stop = new QPushButton("Stop", this);
    stop->setGeometry(870, 544, 128, 50);
    stop->setStyleSheet("font-size:15px; background-color:red;");
    stop->show();
}

// ===================== Arrêt PAC =====================
void MainWindow::stopPac()
{
    qDebug() << "[UI] Arrêt PAC demandé";
    if (m_pac) {
        m_pac->pausePac();
    }

    // Inversion des boutons
    stop->hide();
    marche->show();
}

// ===================== Démarrage PAC =====================
void MainWindow::startPac()
{
    qDebug() << "[UI] Démarrage PAC demandé";
    if (m_pac) {
        m_pac->resumePac();
    }

    // Inversion des boutons
    marche->hide();
    stop->show();
}
