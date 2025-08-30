#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent): QWidget (parent)
{    
    window = new QWidget;
    window ->setFixedSize(1920,1080);
    window ->setWindowTitle("Gainable");
    adjustSize();
    
    label = new QLabel(window);
    label ->setFixedSize(1920,1080);
    label ->setPixmap(QPixmap("/home/ludo/Qt/test/build/images/abigael.jpg"));
    
    labelFroid = new QLabel(window);
    labelFroid ->setFixedSize(1920,1080);
    movieFroid = new QMovie("/home/ludo/Qt/test/build/gif/refroidissement.gif");
    labelFroid ->setMovie(movieFroid);
    movieFroid ->setSpeed(125);
    
    labelChauffage = new QLabel(window);
    labelChauffage ->setFixedSize(1920,1080);
    movieChauffage = new QMovie("/home/ludo/Qt/test/build/gif/chauffage.gif");
    labelChauffage ->setMovie(movieChauffage);
    movieChauffage ->setSpeed(125);
    
    labelCanicule = new QLabel(window);
    labelCanicule ->setFixedSize(1920,1080);
    movieCanicule = new QMovie("/home/ludo/Qt/test/build/gif/canicule.gif");
    labelCanicule ->setMovie(movieCanicule);
    movieCanicule ->setSpeed(125);
    
    disp1 = new QGroupBox("Temp°C Ext",window);
    disp1 ->setStyleSheet("QGroupBox:title {color: orangered;}");
    disp1 ->setFont(QFont("Times", 18, QFont::Bold));
    disp1 ->setGeometry(200,40,240,120);
    tempExtLue = new QLCDNumber(disp1);
    tempExtLue ->setGeometry(0,35,240,80);

    disp2 = new QGroupBox("Temp°C UnitExt",window);
    disp2 ->setStyleSheet("QGroupBox:title {color: orangered;}");
    disp2 ->setFont(QFont("Times", 18, QFont::Bold));
    disp2 ->setGeometry(520,40,240,120);
    tempUnitExtLue = new QLCDNumber(disp2);
    tempUnitExtLue ->setGeometry(0,35,240,80);
    
    disp3 = new QGroupBox("Temp°C EcUnitExt",window);
    disp3 ->setStyleSheet("QGroupBox:title {color: orangered;}");
    disp3 ->setFont(QFont("Times", 18, QFont::Bold));
    disp3 ->setGeometry(840,40,240,120);
    tempEcExtLue = new QLCDNumber(disp3);
    tempEcExtLue ->setGeometry(0,35,240,80);

    disp4 = new QGroupBox("Temp°C UnitInt",window);
    disp4 ->setStyleSheet("QGroupBox:title {color: orangered;}");
    disp4 ->setFont(QFont("Times", 18, QFont::Bold));
    disp4 ->setGeometry(1160,40,240,120);
    tempUnitIntLue = new QLCDNumber(disp4);
    tempUnitIntLue ->setGeometry(0,35,240,80);
    
    disp5 = new QGroupBox("Temp°C EcUnitInt",window);
    disp5 ->setStyleSheet("QGroupBox:title {color: orangered;}");
    disp5 ->setFont(QFont("Times", 18, QFont::Bold));
    disp5 ->setGeometry(1480,40,240,120);
    tempEcIntLue = new QLCDNumber(disp5);
    tempEcIntLue ->setGeometry(0,35,240,80);
    
    affichageTemperatures();
    
    labelGifNettoyageEnCours = new QLabel(window);
    labelGifNettoyageEnCours ->setGeometry(720,300,480,480);
    movieGifNettoyageEnCours = new QMovie("/home/ludo/Qt/test/build/gif/nettoyageEnCours.gif");
    labelGifNettoyageEnCours ->setMovie(movieGifNettoyageEnCours);
    labelGifNettoyageFiltre = new QLabel(window);
    labelGifNettoyageFiltre ->setGeometry(20,840,200,200);
    movieGifNettoyageFiltre = new QMovie("/home/ludo/Qt/test/build/gif/nettoyageFiltre.gif");
    labelGifNettoyageFiltre ->setMovie(movieGifNettoyageFiltre);
    labelGifArretProgramme = new QLabel(window);
    labelGifArretProgramme ->setGeometry(720,300,480,480);
    movieGifArretProgramme = new QMovie("/home/ludo/Qt/test/build/gif/arretProgrammeFiltre.gif");
    labelGifArretProgramme ->setMovie(movieGifArretProgramme);
    
    labelGifVentInt = new QLabel(window);
    labelGifVentInt ->setGeometry(1105,845,200,200);
    movieGifVentInt = new QMovie("/home/ludo/Qt/test/build/gif/ventilateurInt.gif");
    labelGifVentInt ->setMovie(movieGifVentInt);
    labelGifVitesse1Int = new QLabel(window);
    labelGifVitesse1Int ->setGeometry(1040,900,60,60);
    movieGifVitesse1Int = new QMovie("/home/ludo/Qt/test/build/gif/vitesse1.gif");
    labelGifVitesse1Int ->setMovie(movieGifVitesse1Int);
    labelGifVitesse4Int = new QLabel(window);
    labelGifVitesse4Int ->setGeometry(1040,900,60,60);
    movieGifVitesse4Int = new QMovie("/home/ludo/Qt/test/build/gif/vitesse4.gif");
    labelGifVitesse4Int ->setMovie(movieGifVitesse4Int);
    
    labelGifVentExt = new QLabel(window);
    labelGifVentExt ->setGeometry(910,830,200,200);
    movieGifVentExt = new QMovie("/home/ludo/Qt/test/build/gif/ventilateurExt.gif");
    labelGifVentExt ->setMovie(movieGifVentExt);
    labelGifVitesse1Ext = new QLabel(window);
    labelGifVitesse1Ext ->setGeometry(860,900,60,60);
    movieGifVitesse1Ext = new QMovie("/home/ludo/Qt/test/build/gif/vitesse1.gif");
    labelGifVitesse1Ext ->setMovie(movieGifVitesse1Ext);
    labelGifVitesse2Ext = new QLabel(window);
    labelGifVitesse2Ext ->setGeometry(860,900,60,60);
    movieGifVitesse2Ext = new QMovie("/home/ludo/Qt/test/build/gif/vitesse2.gif");
    labelGifVitesse2Ext ->setMovie(movieGifVitesse2Ext);
    
    labelGifFroid = new QLabel(window);
    labelGifFroid ->setGeometry(1210,845,200,200);
    movieGifFroid = new QMovie("/home/ludo/Qt/test/build/gif/froid.gif");
    labelGifFroid ->setMovie(movieGifFroid);
    
    labelGifChauffage = new QLabel(window);
    labelGifChauffage ->setGeometry(1220,840,200,200);
    movieGifChauffage = new QMovie("/home/ludo/Qt/test/build/gif/chauffe.gif");
    labelGifChauffage ->setMovie(movieGifChauffage);
    
    labelGifDegivrage = new QLabel(window);
    labelGifDegivrage ->setGeometry(1220,850,200,200);
    movieGifDegivrage = new QMovie("/home/ludo/Qt/test/build/gif/degivrage.gif");
    labelGifDegivrage ->setMovie(movieGifDegivrage);
    
    labelGifEclaire = new QLabel(window);
    labelGifEclaire ->setGeometry(1160,830,200,200);
    movieGifEclaire = new QMovie("/home/ludo/Qt/test/build/gif/eclaire.gif");
    labelGifEclaire ->setMovie(movieGifEclaire);
    
    labelGifEgouttage = new QLabel(window);
    labelGifEgouttage ->setGeometry(1120,820,200,200);
    movieGifEgouttage = new QMovie("/home/ludo/Qt/test/build/gif/egouttage.gif");
    labelGifEgouttage ->setMovie(movieGifEgouttage);
    
    marche = new QPushButton("Démarrage ??",window);
    marche ->setGeometry(1080,900,240,95);
    marche ->setStyleSheet("font-size: 30px;background-color: lime");
    marche ->hide();
    
    consigne = new QPushButton("Consignes",window);
    consigne ->setGeometry(1340,900,240,95);
    consigne ->setStyleSheet("font-size: 30px;");
    consigne ->show();
    
    stop = new QPushButton("Stop ??",window);
    stop ->setGeometry(1600,900,240,95);
    stop ->setStyleSheet("font-size: 30px;background-color: red");
    stop ->show();
    
    timerTemperatures = new QTimer(this);
    timerTemperatures ->start(timerTemps);
    connect(timerTemperatures, &QTimer::timeout, this, &MainWindow::affichageTemperatures); // mise a jour de la temperature toute les 10 secondes

    connect(marche, &QPushButton::clicked, this, &MainWindow::reStartPac);
    connect(consigne, &QPushButton::clicked, this, &MainWindow::afficheConsignes);
    connect(stop, &QPushButton::clicked, this, &MainWindow::modeOff);
    
    window->show();
}

void MainWindow::affichageTemperatures()
{    
    tempExtLue ->display(read_all_sensors.get_tempExt());
    tempUnitExtLue ->display(read_all_sensors.get_tempUnitExt());
    tempEcExtLue ->display(read_all_sensors.get_tempEchExt());
    tempUnitIntLue ->display(read_all_sensors.get_tempUnitInt());
    tempEcIntLue ->display(read_all_sensors.get_tempEchInt());
}

void MainWindow::afficheConsignes()
{
    TRACE("afficheConsignes()\n");
    afficheCons = new AfficheCons();
    afficheCons ->readCons();
}

void MainWindow::modeOff()
{
    TRACE("modeOf()\n");
    stop ->hide();
    marche ->show();
}

void MainWindow::reStartPac()
{
    TRACE("restartPac()\n");
    marche ->hide();
    stop ->show();
}
