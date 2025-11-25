#include "MainWindow.hpp"
#include <QDebug>
#include <QTimer>

// =====================
// Connexions des signaux/slots
// =====================
void MainWindow::setupConnections()
{
    // --- Boutons principaux ---
    connect(marche, &QPushButton::clicked, this, [this]() {
        qDebug() << "[UI] Bouton 'Démarrage' cliqué";
        startPac();
    });

    connect(stop, &QPushButton::clicked, this, [this]() {
        qDebug() << "[UI] Bouton 'Stop' cliqué";
        stopPac();
    });

    connect(consigne, &QPushButton::clicked, this, &MainWindow::demandeAfficheCons);
    
    // --- Timer de rafraîchissement PAC ---
    m_refreshTimer = new QTimer(this);
    connect(m_refreshTimer, &QTimer::timeout, this, [this]() {
        if (!m_pac) return;

        // Met à jour les 5 températures affichées
        updateTemperature(0, m_pac->getTemperature("Ext"));
        updateTemperature(1, m_pac->getTemperature("UExt"));
        updateTemperature(2, m_pac->getTemperature("EExt"));
        updateTemperature(3, m_pac->getTemperature("UInt"));
        updateTemperature(4, m_pac->getTemperature("EInt"));
    });
    m_refreshTimer->start(1000);

    // --- Timer de date/heure ---
    connect(m_timeTimer, &QTimer::timeout, this, [this]() {
        QDateTime now = QDateTime::currentDateTime();
        m_dateLabel->setText(now.toString("dd-MM-yyyy"));
        m_timeLabel->setText(now.toString("HH:mm:ss"));
    });
    
    // === Mise à jour régulière du GIF Ventilo ===
    QTimer *gifsTimer = new QTimer(this);
    connect(gifsTimer, &QTimer::timeout, this, [this]() {
        if (m_pac)
            updateGifs();
    });
    gifsTimer->start(200); // rafraîchit toutes les 200 ms

    qDebug() << "[MainWindow] setupConnections() terminé";

    // === Timer pour défilement des photos ===
    connect(m_photoTimer, &QTimer::timeout, this, [this]() {
        updatePhotos();
    });
    m_photoTimer->start(5000); // toutes les 5 secondes

    qDebug() << "[MainWindow] setupConnections() terminé (photos OK)";
}
