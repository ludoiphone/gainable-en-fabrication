#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtWidgets>

#include "ds18b20.h"
#include "temperatures.h"
#include "affichecons.h"
#include "Trace.h"

class MainWindow: public QWidget
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);

private:
    DS18B20 read_all_sensors;
    
    AfficheCons *afficheCons;
    
    QTimer *timerTemperatures;
    int timerTemps = 10000; // 10 secondes
    
    QWidget *window;
    
    QLabel *label;
    
    QLabel *labelFroid;
    QMovie *movieFroid;
    QLabel *labelChauffage;
    QMovie *movieChauffage;
    QLabel *labelCanicule;
    QMovie *movieCanicule;
    
    QLCDNumber *tempExtLue;
    QLCDNumber *tempUnitExtLue;
    QLCDNumber *tempEcExtLue;
    QLCDNumber *tempUnitIntLue;
    QLCDNumber *tempEcIntLue;
    
    QLabel *labelGifNettoyageFiltre;
    QMovie *movieGifNettoyageFiltre;
    
    QLabel *labelGifArretProgramme;
    QMovie *movieGifArretProgramme;
    
    QLabel *labelGifNettoyageEnCours;
    QMovie *movieGifNettoyageEnCours;
    
    QLabel *labelGifVentInt;
    QMovie *movieGifVentInt;
    QLabel *labelGifVitesse1Int;
    QMovie *movieGifVitesse1Int;
    QLabel *labelGifVitesse4Int;
    QMovie *movieGifVitesse4Int;
    
    QLabel *labelGifVentExt;
    QMovie *movieGifVentExt;
    QLabel *labelGifVitesse1Ext;
    QMovie *movieGifVitesse1Ext;
    QLabel *labelGifVitesse2Ext;
    QMovie *movieGifVitesse2Ext;
    
    QLabel *labelGifFroid;
    QMovie *movieGifFroid;
    
    QLabel *labelGifChauffage;
    QMovie *movieGifChauffage;
    
    QLabel *labelGifDegivrage;
    QMovie *movieGifDegivrage;
    
    QLabel *labelGifEgouttage;
    QMovie *movieGifEgouttage;
    
    QLabel *labelGifEclaire;
    QMovie *movieGifEclaire;

    QGroupBox *disp1; //tempExt
    QGroupBox *disp2; //tempUnitExt
    QGroupBox *disp3; //tempEcExt
    QGroupBox *disp4; //tempUnitInt
    QGroupBox *disp5; //tempEcInt
    
    QPushButton *marche;
    QPushButton *consigne;
    QPushButton *stop;
    
    void affichageTemperatures();
    
    void afficheConsignes();
    void modeOff();
    void reStartPac();
};
#endif // MAINWINDOW_H
