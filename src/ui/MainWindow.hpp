#pragma once
#include <QWidget>
#include <QStringList>
#include <QLabel>
#include <QPushButton>
#include <QScrollArea>
#include <QTimer>
#include <QThread>
#include "ui/NewsBanner.hpp"
#include "ui/TemperatureWidget.hpp"
#include "ui/DateTime.hpp"
#include "ui/Gifs.hpp"
#include "ui/PacControl.hpp"
#include "ui/Photos.hpp"
#include "core/PacController.hpp"
#include "logic/newsfetcher.hpp"

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
    
    // ========= Gifs Animée =========
    Gifs *m_gifs;

    // ========= Date & Heure =========
    DateTime *m_dateTime;

    // ========= Bandeau News =========
    NewsBanner *m_news;

    // ========= Thread News =========
    QThread *m_newsThread;
    NewsFetcher *m_newsFetcher;

    // ========= Températures =========
    TemperatureWidget *m_temperatureWidget;
    
    // === Photos panoramic ===
    Photos *m_photos;
    
    // === États ===
    bool m_pauseFlag = false;

    // ========= Boutons PAC =========
    PacControl *m_pacControl;
};
