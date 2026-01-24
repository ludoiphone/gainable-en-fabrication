#pragma once
#include <QWidget>
#include <QStringList>
#include <QLabel>
#include <QPushButton>
#include <QScrollArea>
#include <QTimer>
#include "Qt/NewsBanner.hpp"
#include "Qt/TemperatureWidget.hpp"
#include "Qt/WeatherWidget.hpp"
#include "Qt/DateTime.hpp"
#include "Qt/Gifs.hpp"
#include "Qt/PacControl.hpp"
#include "Qt/Photos.hpp"
#include "core/PacController.hpp"

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    explicit MainWindow(PacController *pac, QWidget *parent = nullptr);
    
signals:
    void demandeAfficheCons();

private:
    PacController *m_pac;
    
    // ========= Wallpaper =========
    QLabel *m_wallpaper;
    
    // ========= Widgets météo =======
    WeatherWidget *m_weatherWidget;
    
    // ========= Gifs Animée =========
    Gifs *m_gifs;

    // ========= Date & Heure =========
    DateTime *m_dateTime;

    // ========= Bandeau News =========
    NewsBanner *m_news;

    // ========= Températures =========
    TemperatureWidget *m_temperatureWidget;
    
    // === Photos panoramic ===
    Photos *m_photos;
    
    // === États ===
    bool m_pauseFlag = false;

    // ========= Boutons PAC =========
    PacControl *m_pacControl;
};
