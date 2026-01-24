#include "PacControl.hpp"

PacControl::PacControl(PacController *pac, QWidget *parent)
    : QWidget(parent),
      m_pac(pac)
{
    setAttribute(Qt::WA_AcceptTouchEvents);
    // === Zones tactiles ===
    zone1 = QRect(600, 544, 128, 50);
    zone2 = QRect(735, 544, 128, 50);
    
    setupButtons();
}

// ===================== Création des boutons =====================
void PacControl::setupButtons()
{
    // Bouton Démarrage
    marche = new QPushButton("Démarrage", this);
    marche->setGeometry(870, 544, 128, 50);
    marche->setStyleSheet("font-size:15px; background-color:lime;");
    marche->hide(); // masqué par défaut

    // Bouton Stop
    stop = new QPushButton("Stop", this);
    stop->setGeometry(870, 544, 128, 50);
    stop->setStyleSheet("font-size:15px; background-color:red;");
    stop->show();

    // ===================== Connexions internes =====================
    connect(marche, &QPushButton::clicked, this, &PacControl::startPac);
    connect(stop, &QPushButton::clicked, this, &PacControl::stopPac);
}

// ===================== Slot Démarrage =====================
void PacControl::startPac()
{
    if (m_pac) {
        m_pac->restartUserPac();
    }

    marche->hide();
    stop->show();

    emit pacDemarre();
}

// ===================== Slot Arrêt =====================
void PacControl::stopPac()
{
    if (m_pac) {
        m_pac->pauseUserPac();
    }
    
    openAfficheCons = false;
    timerStarted = false;

    stop->hide();
    marche->show();
    
    emit pacArrete();
}

// ===================== MULTI-TOUCH RÉEL =====================
bool PacControl::event(QEvent *event)
{
    if (event->type() == QEvent::TouchBegin ||
        event->type() == QEvent::TouchUpdate)
    {
        // Sécurité : PAC DOIT ÊTRE ARRÊTÉE
        if (stop->isVisible()) {
            return QWidget::event(event);;
        }

        QTouchEvent *touch = static_cast<QTouchEvent *>(event);

        bool zone1Touched = false;
        bool zone2Touched = false;

        for (const QTouchEvent::TouchPoint &p : touch->touchPoints())
        {
            QPoint pos = p.pos().toPoint();

            if (zone1.contains(pos))
                zone1Touched = true;

            if (zone2.contains(pos))
                zone2Touched = true;
        }

        // Démarrage du timer quand les 2 doigts sont posés
        if (zone1Touched && zone2Touched)
        {
            if (!timerStarted) {
                touchTimer.start();
                timerStarted = true;
            }

            // 1 seconde obligatoire
            if (touchTimer.elapsed() >= 1000 && !openAfficheCons)
            {
                openAfficheCons = true;
                emit demandeAfficheCons();
                return true;
            }
        }
        else
        {
            timerStarted = false;
        }
    }

    if (event->type() == QEvent::TouchEnd)
    {
        timerStarted = false;
        openAfficheCons = false;
    }

    return QWidget::event(event);
}
