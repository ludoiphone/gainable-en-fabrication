#include "affichecons.h"

AfficheCons::AfficheCons()
{
    qDebug() << "DispCons";

    m_window1 = new QWidget;
    m_window1 ->setFixedSize(1920,1080);
    m_window1 ->setWindowTitle("Consignes Gainable");
    adjustSize();
    
    m_label1 = new QLabel(m_window1);
    m_label1 ->setPixmap(QPixmap("/home/ludo/Qt/test/build/images/dispCons.jpeg"));
    m_label1 ->showMaximized();

// colone temp ext 
    m_disp6 = new QGroupBox("Cons°C été/hiver",m_window1);
    m_disp6 ->setFont(QFont("Times", 18, QFont::Bold));
    m_disp6 ->setStyleSheet("QGroupBox:title {color: mediumvioletred;}");
    m_disp6 ->setGeometry(60,10,240,120);
    m_affCons1 = new QLCDNumber(m_disp6);
    m_affCons1 ->setGeometry(0,35,240,80);
    
    m_disp7 = new QGroupBox("Cons°C canicule",m_window1);
    m_disp7 ->setFont(QFont("Times", 18, QFont::Bold));
    m_disp7 ->setStyleSheet("QGroupBox:title {color: orange;}");
    m_disp7 ->setGeometry(60,580,240,120);
    m_affCons2 = new QLCDNumber(m_disp7);
    m_affCons2 ->setGeometry(0,35,240,80);
    
    m_disp8 = new QGroupBox("delta °C",m_window1);
    m_disp8 ->setFont(QFont("Times", 18, QFont::Bold));
    m_disp8 ->setStyleSheet("QGroupBox:title {color: orange;}");
    m_disp8 ->setGeometry(60,720,240,120);
    m_affCons3 = new QLCDNumber(m_disp8);
    m_affCons3 ->setGeometry(0,35,240,80);
    m_affCons3 ->display(-6);
    
// colone temp unite ext
    m_disp9 = new QGroupBox("Cons°C blocage Chauffage",m_window1);
    m_disp9 ->setFont(QFont("Times", 18, QFont::Bold));
    m_disp9 ->setStyleSheet("QGroupBox:title {color: red;}");
    m_disp9 ->setGeometry(360,10,300,120);
    m_affCons4 = new QLCDNumber(m_disp9);
    m_affCons4 ->setGeometry(0,35,300,80);
    
    m_disp10 = new QGroupBox("Cons°C mode deg chauffage",m_window1);
    m_disp10 ->setFont(QFont("Times", 18, QFont::Bold));
    m_disp10 ->setStyleSheet("QGroupBox:title {color: red;}");
    m_disp10 ->setGeometry(360,150,300,120);
    m_affCons5 = new QLCDNumber(m_disp10);
    m_affCons5 ->setGeometry(0,35,300,80);
    
    m_disp11 = new QGroupBox("Cons°C ventil Ext Chauffage",m_window1);
    m_disp11 ->setFont(QFont("Times", 18, QFont::Bold));
    m_disp11 ->setStyleSheet("QGroupBox:title {color: red;}");
    m_disp11 ->setGeometry(360,300,300,120);
    m_affCons6 = new QLCDNumber(m_disp11);
    m_affCons6 ->setGeometry(0,35,300,80);
    
    m_disp12 = new QGroupBox("Cons°C ventil Ext froid",m_window1);
    m_disp12 ->setFont(QFont("Times", 18, QFont::Bold));
    m_disp12 ->setStyleSheet("QGroupBox:title {color: blue;}");
    m_disp12 ->setGeometry(390,720,240,120);
    m_affCons7 = new QLCDNumber(m_disp12);
    m_affCons7 ->setGeometry(0,35,240,80);
    
// colone temp echangeur ext
    m_disp13 = new QGroupBox("Cons°C fin deg Chauffage",m_window1);
    m_disp13 ->setFont(QFont("Times", 18, QFont::Bold));
    m_disp13 ->setStyleSheet("QGroupBox:title {color: red;}");
    m_disp13 ->setGeometry(720,10,300,120);
    m_affCons8 = new QLCDNumber(m_disp13);
    m_affCons8 ->setGeometry(0,35,300,80);
    
// colone temp unit int
    
    m_disp14 = new QGroupBox("Cons°C départ Chauffage en Chauffage",m_window1);
    m_disp14 ->setFont(QFont("Times", 18, QFont::Bold));
    m_disp14 ->setStyleSheet("QGroupBox:title {color: red;}");
    m_disp14 ->setGeometry(1085,10,420,120);
    m_affCons9 = new QLCDNumber(m_disp14);
    m_affCons9 ->setGeometry(0,35,420,80);
    
    m_disp15 = new QGroupBox("Cons°C départ Froid en Chauffage",m_window1);
    m_disp15 ->setFont(QFont("Times", 18, QFont::Bold));
    m_disp15 ->setStyleSheet("QGroupBox:title {color: red;}");
    m_disp15 ->setGeometry(1110,150,370,120);
    m_affCons10 = new QLCDNumber(m_disp15);
    m_affCons10 ->setGeometry(0,35,370,80);
    
    m_disp16 = new QGroupBox("Cons°C ventil Interieur Chauffage",m_window1);
    m_disp16 ->setFont(QFont("Times", 18, QFont::Bold));
    m_disp16 ->setStyleSheet("QGroupBox:title {color: red;}");
    m_disp16 ->setGeometry(1115,300,360,120);
    m_affCons11 = new QLCDNumber(m_disp16);
    m_affCons11 ->setGeometry(0,35,360,80);
    
    m_disp17 = new QGroupBox("Cons°C départ Froid en Froid",m_window1);
    m_disp17 ->setFont(QFont("Times", 18, QFont::Bold));
    m_disp17 ->setStyleSheet("QGroupBox:title {color: blue;}");
    m_disp17 ->setGeometry(1135,440,320,120);
    m_affCons12 = new QLCDNumber(m_disp17);
    m_affCons12 ->setGeometry(0,35,320,80);
    
    m_disp18 = new QGroupBox("Cons°C départ Chauffage en Froid",m_window1);
    m_disp18 ->setFont(QFont("Times", 18, QFont::Bold));
    m_disp18 ->setStyleSheet("QGroupBox:title {color: blue;}");
    m_disp18 ->setGeometry(1110,580,370,120);
    m_affCons13 = new QLCDNumber(m_disp18);
    m_affCons13 ->setGeometry(0,35,370,80);
    
    m_disp19 = new QGroupBox("Cons°C ventil Interieur Froid",m_window1);
    m_disp19 ->setFont(QFont("Times", 18, QFont::Bold));
    m_disp19 ->setStyleSheet("QGroupBox:title {color: blue;}");
    m_disp19 ->setGeometry(1135,720,320,120);
    m_affCons14 = new QLCDNumber(m_disp19);
    m_affCons14 ->setGeometry(0,35,320,80);
    
// colone temp echangeur int 
    m_disp20 = new QGroupBox("Cons°C bloc Vent Chauffage",m_window1);
    m_disp20 ->setFont(QFont("Times", 18, QFont::Bold));
    m_disp20 ->setStyleSheet("QGroupBox:title {color: red;}");
    m_disp20 ->setGeometry(1560,10,300,120);
    m_affCons15 =new QLCDNumber(m_disp20);
    m_affCons15 ->setGeometry(0,35,300,80);
    
    m_disp21 = new QGroupBox("Cons°C deg froid",m_window1);
    m_disp21 ->setFont(QFont("Times", 18, QFont::Bold));
    m_disp21 ->setStyleSheet("QGroupBox:title {color: blue;}");
    m_disp21 ->setGeometry(1590,580,240,120);
    m_affCons16 =new QLCDNumber(m_disp21);
    m_affCons16 ->setGeometry(0,35,240,80);
    
    m_disp22 = new QGroupBox("Cons°C fin deg froid",m_window1);
    m_disp22 ->setFont(QFont("Times", 18, QFont::Bold));
    m_disp22 ->setStyleSheet("QGroupBox:title {color: blue;}");
    m_disp22 ->setGeometry(1590,720,240,120);
    m_affCons17 =new QLCDNumber(m_disp22);
    m_affCons17 ->setGeometry(0,35,240,80);
    
    m_modifC = new QPushButton("Modif Consignes",m_window1);
    m_modifC ->setGeometry(1340,900,240,95);
    m_modifC ->setStyleSheet("font-size: 30px;");
    
    m_quitter = new QPushButton("Quitter",m_window1);
    m_quitter ->setGeometry(1600,900,240,95);
    m_quitter ->setStyleSheet("font-size: 30px;background-color: red");
    
    m_timer2 = new QTimer(this);
    m_timer2 ->start(m_timerCons);
    connect(m_timer2, &QTimer::timeout, this, &AfficheCons::readCons);
    
    connect(m_modifC, &QPushButton::clicked, this, &AfficheCons::modifConsignes);
    connect(m_quitter, &QPushButton::clicked, m_quitter, [this](){m_timer2 ->stop(); delete m_window1;});
    
    m_window1 ->show();
}

void AfficheCons::modifConsignes()
{
    m_modifCons = new ModifCons();
}

void AfficheCons::readCons()
{
    qDebug() << "affiche DispCons";

    m_configCons = new Consignes(QString("settings/consignes.ini"), QSettings::IniFormat);
    m_configCons ->lireConsignesGainable();
    
    m_affCons1 ->display(m_configCons ->consigneEteHiver);
    m_affCons2 ->display(m_configCons ->consigneCanicule);
    m_affCons4 ->display(m_configCons ->consigneBlocChauf);
    m_affCons5 ->display(m_configCons ->consigneModeDegCh);
    m_affCons6 ->display(m_configCons ->consigneGrVitExtCh);
    m_affCons7 ->display(m_configCons ->consigneGrVitExtFr);
    m_affCons8 ->display(m_configCons ->consigneFinDegCh);
    m_affCons9 ->display(m_configCons ->consigneDepartChauffageEnChauffage);
    m_affCons10 ->display(m_configCons ->consigneDepartFroidEnChauffage);
    m_affCons11 ->display(m_configCons ->consignePeVitIntCh);
    m_affCons12 ->display(m_configCons ->consigneDepartFroidEnFroid);
    m_affCons13 ->display(m_configCons ->consigneDepartChauffageEnFroid);
    m_affCons14 ->display(m_configCons ->consignePeVitIntFr);
    m_affCons15 ->display(m_configCons ->consigneDepartVentIntCh);
    m_affCons16 ->display(m_configCons ->consigneDegFr);
    m_affCons17 ->display(m_configCons ->consigneFinDegFr);
}
