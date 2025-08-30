#ifndef AFFICHECONS_H
#define AFFICHECONS_H
#include <QtWidgets>

#include "modifcons.h"
#include "consignes.h"

class AfficheCons: public QWidget
{
    Q_OBJECT

public:
    AfficheCons();

    void readCons();

private:
    ModifCons *m_modifCons;
    
    Consignes *m_configCons;
    
    QWidget *m_window1;
    
    QTimer *m_timer2;

    int m_timerCons = 1000; // maj ecran dispcons 
    
    QLabel *m_label1;
    
    QGroupBox *m_disp6;
    QLCDNumber *m_affCons1; //hiver/éte
   
    QGroupBox *m_disp7;
    QLCDNumber *m_affCons2; //canicule
    
    QGroupBox *m_disp8;
    QLCDNumber *m_affCons3; //delta
    
    QGroupBox *m_disp9;
    QLCDNumber *m_affCons4; //blocChauffage
    
    QGroupBox *m_disp10;
    QLCDNumber *m_affCons5; //mode Degivrage chauffage
    
    QGroupBox *m_disp11;
    QLCDNumber *m_affCons6; //ventExtChauffage
    
    QGroupBox *m_disp12;
    QLCDNumber *m_affCons7; //ventExtFroid
    
    QGroupBox *m_disp13;
    QLCDNumber *m_affCons8; //fin deg chauffage
    
    QGroupBox *m_disp14; //consigne depart chaufage en chauffage
    QLCDNumber *m_affCons9;
    
    QGroupBox *m_disp15;
    QLCDNumber *m_affCons10; //consigne depart froid en chauffage
    
    QGroupBox *m_disp16;
    QLCDNumber *m_affCons11; //consigne vitesse int chauffage
    
    QGroupBox *m_disp17; //consigne depart froid en froid
    QLCDNumber *m_affCons12;
    
    QGroupBox *m_disp18; //consigne depart chaufage en froid
    QLCDNumber *m_affCons13;
    
    QGroupBox *m_disp19;
    QLCDNumber *m_affCons14; //consigne vitesse int froid
    
    QGroupBox *m_disp20; //consigne depart ventil chauffage
    QLCDNumber *m_affCons15;
    
    QGroupBox *m_disp21; //deg froid
    QLCDNumber *m_affCons16;
    
    QGroupBox *m_disp22; //fin deg froid
    QLCDNumber *m_affCons17;
    
    QPushButton *m_modifC;
    QPushButton *m_quitter;
    
    void modifConsignes();
    void closeWindow();
};

#endif // AFFICHECONS_H
