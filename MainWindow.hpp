#pragma once
#include <QWidget>      // hérites de QWidget
#include <QStringList>  // utilisé par valeur
#include <QLabel>
#include <QPushButton>
#include <QScrollArea>
#include <QTimer>
#include <QThread>
#include "core/PacController.hpp"
#include "logic/newsfetcher.hpp"

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    explicit MainWindow(PacController *pac, QWidget *parent = nullptr);
    ~MainWindow();
    
signals:
    void demandeAfficheCons();  // Émis lorsque l'utilisateur clique sur "Consignes"

private:
    PacController *m_pac;
    
    // ========= Wallpaper =========
    QLabel *m_wallpaper;
    
    // ========= Gifs Animée =========
    QLabel *m_ventExtLabel;
    QMovie *m_ventExtMovie;
    
    QLabel *m_ventIntLabel;
    QMovie *m_ventIntMovie;
    
    QLabel *m_vitesseVent1LabelExt;
    QMovie *m_vitesseVent1MovieExt;
    QLabel *m_vitesseVent1LabelInt;
    QMovie *m_vitesseVent1MovieInt;
    QLabel *m_vitesseVent2Label;
    QMovie *m_vitesseVent2Movie;
    QLabel *m_vitesseVent4Label;
    QMovie *m_vitesseVent4Movie;
    
    QLabel *m_compresseurFroidLabel;
    QMovie *m_compresseurFroidMovie;
    QLabel *m_compresseurChauffageLabel;
    QMovie *m_compresseurChauffageMovie;
    
    QLabel *m_degivrageLabel;
    QMovie *m_degivrageMovie;
    
    QLabel *m_egouttageLabel;
    QMovie *m_egouttageMovie;

    // ========= Date & Heure =========
    QLabel *m_dateLabel;
    QLabel *m_timeLabel;
    QTimer *m_timeTimer;

    // ========= Bandeau News =========
    QWidget *m_banner;           
    QLabel *m_cLabel;
    QLabel *m_newsLabelTitle;
    QLabel *m_newsLabel;
    QScrollArea *m_newsScrollArea;
    QTimer *m_newsTimer;
    QTimer *m_scrollTimer;
    QStringList m_newsCache;
    int m_currentIndex;
    double m_scrollOffset;
    int m_textWidth;
    double m_scrollSpeed;

    // ========= Thread News =========
    QThread *m_newsThread;
    NewsFetcher *m_newsFetcher;

    // ========= Températures =========
    struct TempDisplay {
        QLabel *text;
    };
    QLabel *m_titreTemps[5];
    TempDisplay m_tempDisplays[5];
    QTimer *m_refreshTimer;
    
    // === États ===
    bool m_pauseFlag = false;

    
    // ========= Boutons PAC =========
    QPushButton *marche;
    QPushButton *consigne;
    QPushButton *stop;

    // ========= Méthodes internes =========
    void setupDateTimeWidget();
    void setupNewsBanner();
    void setupNewsTimer();
    void setupTemperatureInterface();
    void setupPacControl();
    void setupConnections();
    void startNewsThread();
    
    // === Slots internes ===
    void updateTemperature(int index, double value);
    void updateNewsLabel(const QString &news);

    // === PAC ===
    void startPac();
    void stopPac();

    // === GIFs ===
    void setupGifs();
    void updateGifs();
};
