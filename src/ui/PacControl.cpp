#include "PacControl.hpp"
#include <QDebug>

PacControl::PacControl(PacController *pac, QWidget *parent)
    : QWidget(parent),
      m_pac(pac)
{
    setupButtons();
}

// ===================== Création des boutons =====================
void PacControl::setupButtons()
{
    // Bouton Démarrage
    marche = new QPushButton("Démarrage", this);
    marche->setGeometry(600, 544, 128, 50);
    marche->setStyleSheet("font-size:15px; background-color:lime;");
    marche->hide(); // masqué par défaut

    // Bouton Consignes
    consigne = new QPushButton("Consignes", this);
    consigne->setGeometry(735, 544, 128, 50);
    consigne->setStyleSheet("font-size:15px; background-color:lightgray;");
    consigne->show();

    // Bouton Stop
    stop = new QPushButton("Stop", this);
    stop->setGeometry(870, 544, 128, 50);
    stop->setStyleSheet("font-size:15px; background-color:red;");
    stop->show();

    // ===================== Connexions internes =====================
    connect(marche, &QPushButton::clicked, this, &PacControl::startPac);
    connect(stop, &QPushButton::clicked, this, &PacControl::stopPac);
    connect(consigne, &QPushButton::clicked, this, &PacControl::demandeAfficheCons);
}

// ===================== Slot Démarrage =====================
void PacControl::startPac()
{
    qDebug() << "[PacControl] Démarrage PAC demandé";
    if (m_pac) {
        m_pac->resumePac();
    }

    marche->hide();
    stop->show();

    emit pacDemarre();
}

// ===================== Slot Arrêt =====================
void PacControl::stopPac()
{
    qDebug() << "[PacControl] Arrêt PAC demandé";
    if (m_pac) {
        m_pac->pausePac();
    }

    stop->hide();
    marche->show();

    emit pacArrete();
}

