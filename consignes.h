#ifndef CONSIGNES_H
#define CONSIGNES_H

#include <QtWidgets>

class Consignes: public QSettings
{
    Q_OBJECT

public:
    Consignes(const QString &nomDuFichier, QSettings::Format format);
    
    double consigneEteHiver;
    double consigneCanicule;
    double consigneBlocChauf;
    double consigneModeDegCh;
    double consigneGrVitExtCh;
    double consigneGrVitExtFr;
    double consignePeVitIntCh;
    double consigneFinDegCh;
    double consigneDepartChauffageEnChauffage;
    double consigneDepartFroidEnChauffage;
    double consignePeVitIntFr;
    double consigneDepartFroidEnFroid;
    double consigneDepartChauffageEnFroid;
    double consigneDepartVentIntCh;
    double consigneDegFr;
    double consigneFinDegFr;
    
    double consigneIntCanicule;
    double consigneVentIntCanicule;
    
    void controleConsignes();
    
    void ecritConsigneEteHiver();
    void ecritConsigneGrVitExtCh();
    void ecritConsigneGrVitExtFr();
    void ecritConsigneCanicule();
    void ecritConsigneBlocCh();
    void ecritConsigneModeDegCh();
    void ecritConsigneFinDegCh();
    void ecritConsigneDepartChauffageEnChauffage();
    void ecritConsigneDepartFroidEnChauffage();
    void ecritConsignePeVitIntCh();
    void ecritConsignePeVitIntFr();
    void ecritConsigneDepartFroidEnFroid();
    void ecritConsigneDepartChauffageEnFroid();
    void ecritConsigneDepartVentIntCh();
    void ecritConsigneDegFr();
    void ecritConsigneFinDegFr();
    
    void ecritConsigneIntCanicule();
    void ecritConsigneVentIntCanicule();
    
    void lireConsigneIntCanicule();
    void lireConsigneVentIntCanicule();

    void lireConsignesGainable();

private:
    double consigneEteHiverDefaut = 13.5;
    double consigneCaniculeDefaut = 30.0;
    double consigneBlocChaufDefaut = 11.0;
    double consigneModeDegChDefaut = 5.0;
    double consigneGrVitExtChDefaut = 5.0;
    double consigneGrVitExtFrDefaut = 20.0;
    double consigneFinDegChDefaut = 12.5;
    double consigneDepartChauffageEnChauffageDefaut = 21.0;
    double consigneDepartFroidEnChauffageDefaut = 23.0;
    double consignePeVitIntChDefaut = 22.5;
    double consigneDepartFroidEnFroidDefaut = 23.0;
    double consigneDepartChauffageEnFroidDefaut = 18.0;
    double consignePeVitIntFrDefaut = 22.5;
    double consigneDepartVentIntChDefaut = 35.0;
    double consigneDegFrDefaut = -3.0;
    double consigneFinDegFrDefaut = 15.0;
    
    void ecritConsignesGainableDefaut();
    
    QMutex m_mutexConsignes;
};

#endif // CONSIGNES_H
