#include "modifcons.h"

ModifCons::ModifCons()
{
    qDebug() << "modifcons";
    
    m_modifConsignes = new Consignes(QString("settings/consignes.ini"), QSettings::IniFormat);
    
    m_window2 = new QWidget;
    m_window2 ->setFixedSize(1000,600);
    m_window2 ->setWindowTitle("Modification Consignes Gainable");
    adjustSize();
    
    m_label2 = new QLabel(m_window2);
    m_label2 ->setPixmap(QPixmap("/home/ludo/Qt/test/build/images/modifCons.jpg"));
    m_label2 ->showMaximized();
    
    select = new QComboBox(m_window2);
    select ->setGeometry(50,50,900,120);
    for (int i(0); i < 16; ++i)
    {
        select ->addItem(QString::number(i));
    }
    select ->setStyleSheet("font-size: 30px;");
    select ->setItemText(0,"Consigne Ete/Hiver :");
    select ->setItemData(0, QColor(199, 21, 133),Qt::ForegroundRole);
    select ->setItemText(1,"Consigne Canicule :");
    select ->setItemData(1, QColor(255, 175, 0), Qt::ForegroundRole);
    select ->setItemText(2,"Consigne Bloquage Chauffage :");
    select ->setItemData(2, QColor(255, 0, 0), Qt::ForegroundRole);
    select ->setItemText(3,"Consigne Mode Degivrage (Naturel ou Electrique) Chauffage :");
    select ->setItemData(3, QColor(255, 0, 0), Qt::ForegroundRole);
    select ->setItemText(4,"Consigne Grande Vitesse Ventilation Exterieur Chauffage :");
    select ->setItemData(4, QColor(255, 0, 0), Qt::ForegroundRole);
    select ->setItemText(5,"Consigne Grande Vitesse Ventilation Exterieur Froid :");
    select ->setItemData(5, QColor(0, 0, 255), Qt::ForegroundRole);
    select ->setItemText(6,"Consigne Fin Degivrage Chauffage :");
    select ->setItemData(6, QColor(255, 0, 0), Qt::ForegroundRole);
    select ->setItemText(7,"Consigne Depart Chauffage en Mode Chauffage :");
    select ->setItemData(7, QColor(255, 0, 0), Qt::ForegroundRole);
    select ->setItemText(8,"Consigne Depart Froid en Mode Chauffage :");
    select ->setItemData(8, QColor(255, 0, 0), Qt::ForegroundRole);
    select ->setItemText(9,"Consigne Petite Vitesse Ventilation Interieur Chauffage :");
    select ->setItemData(9, QColor(255, 0, 0), Qt::ForegroundRole);
    select ->setItemText(10,"Consigne Depart Froid en Mode Froid :");
    select ->setItemData(10, QColor(0, 0, 255), Qt::ForegroundRole);
    select ->setItemText(11,"Consigne Depart Chauffage en Mode Froid :");
    select ->setItemData(11, QColor(0, 0, 255), Qt::ForegroundRole);
    select ->setItemText(12,"Consigne Petite Vitesse Ventilation Interieur Froid :");
    select ->setItemData(12, QColor(0, 0, 255), Qt::ForegroundRole); 
    select ->setItemText(13,"Consigne Depart Ventilation Interieur Chauffage :");
    select ->setItemData(13, QColor(255, 0, 0), Qt::ForegroundRole);
    select ->setItemText(14,"Consigne Lancement Degivrage Froid :");
    select ->setItemData(14, QColor(0, 0, 255), Qt::ForegroundRole);
    select ->setItemText(15,"Consigne Fin Degivrage Froid :");
    select ->setItemData(15, QColor(0, 0, 255), Qt::ForegroundRole);

    m_valid1 = new QPushButton("valid",m_window2);
    m_valid1 ->setGeometry(50,180,240,120);
    m_valid1 ->setStyleSheet("font-size: 30px;");
    
    m_quit1 = new QPushButton("quitter",m_window2);
    m_quit1 ->setGeometry(710,440,240,120);
    m_quit1 ->setStyleSheet("font-size: 30px;background-color: red");
    
    connect(m_valid1, &QPushButton::clicked, this, &ModifCons::selectCons);
    connect(m_quit1,&QPushButton::clicked,m_quit1,[this](){/*m_window2 ->close();*/ delete m_window2;});
    
    m_window2 ->show();
}

void ModifCons::adjustCons()
{
    m_selCons = new QDoubleSpinBox (m_window2);
    m_selCons ->setStyleSheet("QDoubleSpinBox::down-button{width: 150} QDoubleSpinBox::up-button{width: 150}");
    m_selCons ->setFont(QFont("Times", 20, QFont::Bold));
    m_selCons ->setValue(consSelect);
    m_selCons ->setRange(consMini,consMaxi);
    m_selCons ->setSingleStep(0.1);
    m_selCons ->setPrefix("Valeur: ");
    m_selCons ->setSuffix(" °C");
    m_selCons ->setGeometry(50,310,400,120);
    m_selCons ->show();

    m_valid2 = new QPushButton("ok",m_window2);
    m_valid2 ->setGeometry(50,440,240,120);
    m_valid2 ->setStyleSheet("font-size: 30px;");
    m_valid2 ->show();
    
    connect(m_valid2,&QPushButton::clicked,m_valid2,[this](){saveCons(); m_selCons ->close();});
}

void ModifCons::selectCons()
{
    m_modifConsignes ->lireConsignesGainable();
    
    switch (select ->currentIndex())
    {
        case 0:
            consSelect = m_modifConsignes ->consigneEteHiver; // 13.5
            consMini = 10.0;
            consMaxi = 16.0;
            adjustCons();
            m_valid1 ->hide();
        break;
        
        case 1:
            consSelect = m_modifConsignes ->consigneCanicule; // 30.0
            consMini = 28.0;
            consMaxi = 32.0;
            adjustCons();
            m_valid1 ->hide();
        break;
        
        case 2:
            consSelect = m_modifConsignes ->consigneBlocChauf; // 11.0
            consMini = 9.0;
            consMaxi = 13.0;
            adjustCons();
            m_valid1 ->hide();
        break;
        
        case 3:
            consSelect = m_modifConsignes ->consigneModeDegCh; // 5.0
            consMini = 3.0;
            consMaxi = 7.0;
            adjustCons();
            m_valid1 ->hide();
        break;
        
        case 4:
            consSelect = m_modifConsignes ->consigneGrVitExtCh; // 5.0
            consMini = 3.0;
            consMaxi = 7.0;
            adjustCons();
            m_valid1 ->hide();
        break;
        
        case 5:
            consSelect = m_modifConsignes ->consigneGrVitExtFr; // 20.0
            consMini = 16.0;
            consMaxi = 22.0;
            adjustCons();
            m_valid1 ->hide();
        break;
        
        case 6:
            consSelect = m_modifConsignes ->consigneFinDegCh; // 12.5
            consMini = 10.5;
            consMaxi = 14.5;
            adjustCons();
            m_valid1 ->hide();
        break;
        
        case 7:
            consSelect = m_modifConsignes ->consigneDepartChauffageEnChauffage; // 
            consMini = 17.0;
            consMaxi = 23.0;
            adjustCons();
            m_valid1 ->hide();
        break;
        
        case 8:
            consSelect = m_modifConsignes ->consigneDepartFroidEnChauffage; // 
            consMini = 23.5;
            consMaxi = 25.0;
            adjustCons();
            m_valid1 ->hide();
        break;
        
        case 9:
            consSelect = m_modifConsignes ->consignePeVitIntCh; // 23.5
            consMini = 21.5;
            consMaxi = 25.5;
            adjustCons();
            m_valid1 ->hide();
        break;
        
        case 10:
            consSelect = m_modifConsignes ->consigneDepartFroidEnFroid; // 
            consMini = 23.0;
            consMaxi = 29.0;
            adjustCons();
            m_valid1 ->hide();
        break;
        
        case 11:
            consSelect = m_modifConsignes ->consigneDepartChauffageEnFroid; // 
            consMini = 17.0;
            consMaxi = 23.0;
            adjustCons();
            m_valid1 ->hide();
        break;
        
        case 12:
            consSelect = m_modifConsignes ->consignePeVitIntFr; // 23.0
            consMini = 21.0;
            consMaxi = 25.0;
            adjustCons();
            m_valid1 ->hide();
        break;
        
        case 13:
            consSelect = m_modifConsignes ->consigneDepartVentIntCh; // 35.0
            consMini = 30.0;
            consMaxi = 40.0;
            adjustCons();
            m_valid1 ->hide();
        break;
        
        case 14:
            consSelect = m_modifConsignes ->consigneDegFr; // -3.0
            consMini = -5;
            consMaxi = -1;
            adjustCons();
            m_valid1 ->hide();
        break;
        
        case 15:
            consSelect = m_modifConsignes ->consigneFinDegFr; // 15.0
            consMini = 13.0;
            consMaxi = 17.0;
            adjustCons();
            m_valid1 ->hide();
        break;

        default:
        break;
    }
}

void ModifCons::saveCons()
{
    qDebug() << "saveCons";
    
    m_valid1 ->show();
    m_valid2 ->hide();
    
    switch (select ->currentIndex())
    {
        case 0:
            m_modifConsignes ->consigneEteHiver = m_selCons ->value();
            m_modifConsignes ->ecritConsigneEteHiver();
        break;
        
        case 1:
            m_modifConsignes ->consigneCanicule = m_selCons ->value();
            m_modifConsignes ->ecritConsigneCanicule();
        break;
        
        case 2:
            m_modifConsignes ->consigneBlocChauf = m_selCons ->value();
            m_modifConsignes ->ecritConsigneBlocCh();
        break;
        
        case 3:
            m_modifConsignes ->consigneModeDegCh = m_selCons ->value();
            m_modifConsignes ->ecritConsigneModeDegCh();
        break;
        
        case 4:
            m_modifConsignes ->consigneGrVitExtCh = m_selCons ->value();
            m_modifConsignes ->ecritConsigneGrVitExtCh();
        break;
        
        case 5:
            m_modifConsignes ->consigneGrVitExtFr = m_selCons ->value();
            m_modifConsignes ->ecritConsigneGrVitExtFr();
        break;
        
        case 6:
            m_modifConsignes ->consigneFinDegCh = m_selCons ->value();
            m_modifConsignes ->ecritConsigneFinDegCh();
        break;
        
        case 7:
            m_modifConsignes ->consigneDepartChauffageEnChauffage = m_selCons ->value();
            m_modifConsignes ->ecritConsigneDepartChauffageEnChauffage();
        break;
        
        case 8:
            m_modifConsignes ->consigneDepartFroidEnChauffage = m_selCons ->value();
            m_modifConsignes ->ecritConsigneDepartFroidEnChauffage();
        break;
        
        case 9:
            m_modifConsignes ->consignePeVitIntCh = m_selCons ->value();
            m_modifConsignes ->ecritConsignePeVitIntCh();
        break;
        
        case 10:
            m_modifConsignes ->consigneDepartFroidEnFroid = m_selCons ->value();
            m_modifConsignes ->ecritConsigneDepartFroidEnFroid();
        break;
        
        case 11:
            m_modifConsignes ->consigneDepartChauffageEnFroid = m_selCons ->value();
            m_modifConsignes ->ecritConsigneDepartChauffageEnFroid();
        break;
        
        case 12:
            m_modifConsignes ->consignePeVitIntFr = m_selCons ->value();
            m_modifConsignes ->ecritConsignePeVitIntFr();
        break;
        
        case 13:
            m_modifConsignes ->consigneDepartVentIntCh = m_selCons ->value();
            m_modifConsignes ->ecritConsigneDepartVentIntCh();
        break;
        
        case 14:
            m_modifConsignes ->consigneDegFr = m_selCons ->value();
            m_modifConsignes ->ecritConsigneDegFr();
        break;
        
        case 15:
            m_modifConsignes ->consigneFinDegFr = m_selCons ->value();
            m_modifConsignes ->ecritConsigneFinDegFr();
        break;
        
        default:
        break;
    }
}
