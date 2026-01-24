#include "MainWindow.hpp"
#include <QPixmap>
#include <QDateTime>

// ===================== Constructeur =====================
MainWindow::MainWindow(PacController *pac, QWidget *parent)
    : QWidget(parent),
      m_pac(pac)
{
    setFixedSize(1024,600);
    
    // ===================== Wallpaper (background) =====================
    m_wallpaper = new QLabel(this);
    m_wallpaper->setGeometry(0,0,width(),height());
    m_wallpaper->setPixmap(
        QPixmap("/home/ludo/Desktop/PAC2/images/mainWindow.jpg")
        .scaled(size(), Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation)
    );
    m_wallpaper->lower();
    
    bool enableNews = true;
    bool enableDateTime = true;
    bool enableTemperature = true;
    bool enablePacControl = true;
    bool enablePhotos = true;
    bool enableGifs = true;
    bool enableWeather = true;
    
    // ===================== Bandeau NEWS =====================
    if (enableNews) {
        m_news = new NewsBanner(this);
        m_news->setGeometry(0, 0, width(), 33);
        m_news->raise();
        m_news->show();
    }
    
    // ===================== Timer / Date =====================
    if (enableDateTime) {
        m_dateTime = new DateTime(this);
        m_dateTime->setPosition(810, 36);
        m_dateTime->raise();
        m_dateTime->show();
    }
    
    // ===================== Températures =====================
    if (enableTemperature) {
        m_temperatureWidget = new TemperatureWidget(m_pac, this);
        m_temperatureWidget->raise();
        m_temperatureWidget->show();
    }
    
    // ===================== Widget météo =====================
    if (enableWeather) {
        m_weatherWidget = new WeatherWidget(this);
        m_weatherWidget->raise();
        m_weatherWidget->show();
    }
    
    // ===================== Boutons =========================
    if (enablePacControl) {
        m_pacControl = new PacControl(m_pac, this);
        m_pacControl->setGeometry(0, 0, width(), height());
        m_pacControl->raise();
        m_pacControl->show();
        connect(m_pacControl, &PacControl::demandeAfficheCons, this, &MainWindow::demandeAfficheCons);
    }
    
    // ===================== Photos ==========================
    if (enablePhotos) {
        m_photos = new Photos("/home/ludo/Desktop/PAC2/photos", this);
        m_photos->setGeometry(600, 100, 400, 400);m_photos->show();
    }
    
    // ===================== Gifs ============================
    if (enableGifs) {
        m_gifs = new Gifs(m_pac, this);
        m_gifs->setup();
        m_gifs->startTimer();
    }
}



