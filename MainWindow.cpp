#include "MainWindow.hpp"
#include <QPixmap>
#include <QDateTime>
#include <QDebug>

// ===================== Constructeur =====================
MainWindow::MainWindow(PacController *pac, QWidget *parent)
    : QWidget(parent),
      m_pac(pac),
      m_currentIndex(0),
      m_scrollOffset(0),
      m_textWidth(0),
      m_scrollSpeed(0.5)   // vitesse de défilement (px/tick)
{
    setFixedSize(1024,600);

    // ===================== Wallpaper =====================
    m_wallpaper = new QLabel(this);
    QPixmap bg("/home/ludo/Desktop/test4/images/mainWindow.jpg");
    m_wallpaper->setPixmap(bg.scaled(width(), height(), Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation));
    m_wallpaper->lower();

    // ===================== Timer / Date =====================
    setupDateTimeWidget();

    // ===================== Bandeau news =====================
    setupNewsTimer();
    setupNewsBanner();

    // ===================== Températures ====================
    setupTemperatureInterface();
    
    // ===================== Boutons =========================
    setupPacControl();
    
    // ===================== Connections =====================
    setupConnections();
    
    // ===================== Gifs ========================
    setupGifs();

    // ===================== Start News Thread =====================
    startNewsThread();
}

// ===================== Destructeur =====================
MainWindow::~MainWindow()
{
    if (m_newsThread && m_newsThread->isRunning()) {
        if (m_newsFetcher)
            QMetaObject::invokeMethod(m_newsFetcher, "deleteLater");
        m_newsThread->quit();
        m_newsThread->wait();
    }

    qDebug() << "MainWindow détruit proprement";
}


