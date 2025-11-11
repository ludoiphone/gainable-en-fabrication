#include "MainWindow.hpp"
#include <QMovie>
#include <QLabel>
#include <QDebug>

// ===================== Initialisation du GIF Ventilateur =====================
void MainWindow::setupGifs()
{
    // --- Positions adaptées pour 1024x600 ---
    const QRect geometryVentExt(490, 470, 120, 120);
    const QRect geometryVentInt(350, 490, 120, 120);
    const QRect geometryVitesseExt1(450, 520, 60, 60);
    const QRect geometryVitesseInt1(300, 520, 60, 60);
    const QRect geometryVitesseExt2(450, 520, 60, 60);
    const QRect geometryVitesseInt4(300, 520, 60, 60);
    const QRect geometryCompresseurFroid(600, 480, 120, 120);
    const QRect geometryCompresseurChauffage(610, 480, 100, 100);
    const QRect geometryDegivrage(610, 480, 120, 120);
    const QRect geometryEgouttage(525, 410, 200, 210);

    // --- Ventilateur Extérieur ---
    m_ventExtLabel = new QLabel(this);
    m_ventExtLabel->setGeometry(geometryVentExt);
    m_ventExtMovie = new QMovie("/home/ludo/Desktop/test4/gifs/ventilateurExt.gif", QByteArray(), m_ventExtLabel);
    m_ventExtMovie->setCacheMode(QMovie::CacheAll);
    m_ventExtLabel->setMovie(m_ventExtMovie);
    m_ventExtMovie->start(); // démarre une fois
    m_ventExtLabel->hide();

    // --- Ventilateur Intérieur ---
    m_ventIntLabel = new QLabel(this);
    m_ventIntLabel->setGeometry(geometryVentInt);
    m_ventIntMovie = new QMovie("/home/ludo/Desktop/test4/gifs/ventilateurInt.gif", QByteArray(), m_ventIntLabel);
    m_ventIntMovie->setCacheMode(QMovie::CacheAll);
    m_ventIntLabel->setMovie(m_ventIntMovie);
    m_ventIntMovie->start();
    m_ventIntLabel->hide();

    // --- Vitesse 1 ---
    m_vitesseVent1LabelExt = new QLabel(this);
    m_vitesseVent1LabelExt->setGeometry(geometryVitesseExt1);
    m_vitesseVent1MovieExt = new QMovie("/home/ludo/Desktop/test4/gifs/vitesse1.gif", QByteArray(), m_vitesseVent1LabelExt);
    m_vitesseVent1MovieExt->setCacheMode(QMovie::CacheAll);
    m_vitesseVent1LabelExt->setMovie(m_vitesseVent1MovieExt);
    m_vitesseVent1MovieExt->start();
    m_vitesseVent1LabelExt->hide();

    m_vitesseVent1LabelInt = new QLabel(this);
    m_vitesseVent1LabelInt->setGeometry(geometryVitesseInt1);
    m_vitesseVent1MovieInt = new QMovie("/home/ludo/Desktop/test4/gifs/vitesse1.gif", QByteArray(), m_vitesseVent1LabelInt);
    m_vitesseVent1MovieInt->setCacheMode(QMovie::CacheAll);
    m_vitesseVent1LabelInt->setMovie(m_vitesseVent1MovieInt);
    m_vitesseVent1MovieInt->start();
    m_vitesseVent1LabelInt->hide();

    // --- Vitesse 2 (Extérieur) ---
    m_vitesseVent2Label = new QLabel(this);
    m_vitesseVent2Label->setGeometry(geometryVitesseExt2);
    m_vitesseVent2Movie = new QMovie("/home/ludo/Desktop/test4/gifs/vitesse2.gif", QByteArray(), m_vitesseVent2Label);
    m_vitesseVent2Movie->setCacheMode(QMovie::CacheAll);
    m_vitesseVent2Label->setMovie(m_vitesseVent2Movie);
    m_vitesseVent2Movie->start();
    m_vitesseVent2Label->hide();

    // --- Vitesse 4 (Intérieur) ---
    m_vitesseVent4Label = new QLabel(this);
    m_vitesseVent4Label->setGeometry(geometryVitesseInt4);
    m_vitesseVent4Movie = new QMovie("/home/ludo/Desktop/test4/gifs/vitesse4.gif", QByteArray(), m_vitesseVent4Label);
    m_vitesseVent4Movie->setCacheMode(QMovie::CacheAll);
    m_vitesseVent4Label->setMovie(m_vitesseVent4Movie);
    m_vitesseVent4Movie->start();
    m_vitesseVent4Label->hide();
    
    // --- Compresseur (Froid, Chauffage) ---
    m_compresseurFroidLabel = new QLabel(this);
    m_compresseurFroidLabel->setGeometry(geometryCompresseurFroid);
    m_compresseurFroidMovie = new QMovie("/home/ludo/Desktop/test4/gifs/froid.gif", QByteArray(), m_compresseurFroidLabel);
    m_compresseurFroidMovie->setCacheMode(QMovie::CacheAll);
    m_compresseurFroidLabel->setMovie(m_compresseurFroidMovie);
    m_compresseurFroidMovie->start();
    m_compresseurFroidLabel->hide();
    
    m_compresseurChauffageLabel = new QLabel(this);
    m_compresseurChauffageLabel->setGeometry(geometryCompresseurChauffage);
    m_compresseurChauffageMovie = new QMovie("/home/ludo/Desktop/test4/gifs/chauffe.gif", QByteArray(), m_compresseurChauffageLabel);
    m_compresseurChauffageMovie->setCacheMode(QMovie::CacheAll);
    m_compresseurChauffageLabel->setMovie(m_compresseurChauffageMovie);
    m_compresseurChauffageMovie->start();
    m_compresseurChauffageLabel->hide();
    
    m_degivrageLabel = new QLabel(this);
    m_degivrageLabel->setGeometry(geometryDegivrage);
    m_degivrageMovie = new QMovie("/home/ludo/Desktop/test4/gifs/degivrage.gif", QByteArray(), m_degivrageLabel);
    m_degivrageMovie->setCacheMode(QMovie::CacheAll);
    m_degivrageLabel->setMovie(m_degivrageMovie);
    m_degivrageMovie->start();
    m_degivrageLabel->hide();
    
    m_egouttageLabel = new QLabel(this);
    m_egouttageLabel->setGeometry(geometryEgouttage);
    m_egouttageMovie = new QMovie("/home/ludo/Desktop/test4/gifs/egouttage.gif", QByteArray(), m_egouttageLabel);
    m_egouttageMovie->setCacheMode(QMovie::CacheAll);
    m_egouttageLabel->setMovie(m_egouttageMovie);
    m_egouttageMovie->start();
    m_egouttageLabel->hide();
}

// ===================== Mise à jour des GIFS =====================
void MainWindow::updateGifs()
{
    if (!m_pac) return;

    bool actifExt  = m_pac->getVentilateurExt();
    bool actifInt  = m_pac->getVentilateurInt();
    int  vitesseExt = m_pac->getVitesseVentilateurExt();
    int  vitesseInt = m_pac->getVitesseVentilateurInt();
    bool actifComp = m_pac->getCompresseur();
    bool actifDeg = m_pac->getDegivrage();
    bool actifEgou = m_pac->getEgouttage();

    ModePAC mode = m_pac->getModeActuel();
    bool modeFroid = (mode == MODE_FROID);
    bool modeChauffage = (mode == MODE_CHAUFFAGE);

    // --- Ventilateurs  ---
    m_ventExtLabel->setVisible(actifExt);
    m_ventIntLabel->setVisible(actifInt);

    if (actifExt)  m_ventExtMovie->start();
    else           m_ventExtMovie->stop();

    if (actifInt)  m_ventIntMovie->start();
    else           m_ventIntMovie->stop();

    // --- Vitesses ventilateurs ---
    // Extérieur
    m_vitesseVent1LabelExt->setVisible(actifExt && vitesseExt == 1);
    m_vitesseVent2Label->setVisible(actifExt && vitesseExt == 2);

    if (actifExt && vitesseExt == 1) m_vitesseVent1MovieExt->start();
    else                              m_vitesseVent1MovieExt->stop();

    if (actifExt && vitesseExt == 2) m_vitesseVent2Movie->start();
    else                              m_vitesseVent2Movie->stop();

    // Intérieur
    m_vitesseVent1LabelInt->setVisible(actifInt && vitesseInt == 1);
    m_vitesseVent4Label->setVisible(actifInt && vitesseInt == 4);

    if (actifInt && vitesseInt == 1) m_vitesseVent1MovieInt->start();
    else                              m_vitesseVent1MovieInt->stop();

    if (actifInt && vitesseInt == 4) m_vitesseVent4Movie->start();
    else                              m_vitesseVent4Movie->stop();

    // --- Compresseur (selon le mode) ---
    if (modeFroid) {
        m_compresseurFroidLabel->setVisible(actifComp);
        m_compresseurChauffageLabel->setVisible(false);
        if (actifComp) m_compresseurFroidMovie->start();
        else            m_compresseurFroidMovie->stop();
        m_compresseurChauffageMovie->stop();
    } 
    else if (modeChauffage) {
        m_compresseurChauffageLabel->setVisible(actifComp);
        m_compresseurFroidLabel->setVisible(false);
        if (actifComp) m_compresseurChauffageMovie->start();
        else            m_compresseurChauffageMovie->stop();
        m_compresseurFroidMovie->stop();
    } 
    else {
        // Mode arrêt -> rien d’affiché
        m_compresseurFroidLabel->setVisible(false);
        m_compresseurChauffageLabel->setVisible(false);
        m_compresseurFroidMovie->stop();
        m_compresseurChauffageMovie->stop();
    }
    
    // Dégivrage
    m_degivrageLabel->setVisible(actifDeg);
    
    if (actifDeg)  m_degivrageMovie->start();
    else           m_degivrageMovie->stop();
    
    // Egouttage
    m_egouttageLabel->setVisible(actifEgou);
    
    if (actifEgou) m_egouttageMovie->start();
    else           m_egouttageMovie->stop();
}


