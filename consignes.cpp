#include "consignes.h"

Consignes::Consignes(const QString &nomDuFichier, QSettings::Format format): QSettings(nomDuFichier, format)
{
    qDebug() << "Consignes";
}

void Consignes::controleConsignes()
{
    if (this ->childGroups().contains("Consignes")) {
        qDebug() << "Fichier Consignes OK";
    } else {
        qDebug() << "Creation Fichier Consignes DEFAUT ";
        ecritConsignesGainableDefaut();
    }
}

void Consignes::ecritConsignesGainableDefaut()
{
    this ->beginGroup("Consignes");
    this ->setValue("ConsigneEteHiver", consigneEteHiverDefaut);
    this ->setValue("ConsigneCanicule", consigneCaniculeDefaut);
    this ->setValue("ConsigneBlocageChauffage", consigneBlocChaufDefaut);
    this ->setValue("ConsigneModeDegivrageChauffage", consigneModeDegChDefaut);
    this ->setValue("ConsigneGrandeVitesseExterieurChauffage", consigneGrVitExtChDefaut);
    this ->setValue("ConsigneGrandeVitesseExterieurFroid", consigneGrVitExtFrDefaut);
    this ->setValue("ConsigneFinDegivrageChauffage", consigneFinDegChDefaut);
    this ->setValue("ConsigneDepartChauffageEnModeChauffage", consigneDepartChauffageEnChauffageDefaut);
    this ->setValue("ConsigneDepartFroidEnModeChauffage", consigneDepartFroidEnChauffageDefaut);
    this ->setValue("ConsignePetiteVitesseInterieurChauffage", consignePeVitIntChDefaut);
    this ->setValue("ConsigneDepartFroidEnModeFroid", consigneDepartFroidEnFroidDefaut);
    this ->setValue("ConsigneDepartChauffageEnModeFroid", consigneDepartChauffageEnFroidDefaut);
    this ->setValue("ConsignePetiteVitesseInterieurFroid", consignePeVitIntFrDefaut);
    this ->setValue("ConsigneDepartVentilateurInterieurChauffage", consigneDepartVentIntChDefaut);
    this ->setValue("ConsigneDegivrageFroid", consigneDegFrDefaut);
    this ->setValue("ConsigneFinDegivrageFroid", consigneFinDegFrDefaut);
    this ->endGroup();
}

void Consignes::ecritConsigneEteHiver()
{
    qDebug() << "ecritConsigneEteHiver()";
    
    this ->beginGroup("Consignes");
    this ->setValue("ConsigneEteHiver", consigneEteHiver);
    this ->endGroup();
}

void Consignes::ecritConsigneCanicule()
{
    qDebug() << "ecritConsigneCanicule()";
    
    this ->beginGroup("Consignes");
    this ->setValue("ConsigneCanicule", consigneCanicule);
    this ->endGroup();
}

void Consignes::ecritConsigneBlocCh()
{
    qDebug() << "ecritConsigneBlocCh()";
    
    this ->beginGroup("Consignes");
    this ->setValue("ConsigneBlocageChauffage", consigneBlocChauf);
    this ->endGroup();
}

void Consignes::ecritConsigneModeDegCh()
{
    qDebug() << "ecritConsigneModeDegCh()";
    
    this ->beginGroup("Consignes");
    this ->setValue("ConsigneModeDegivrageChauffage", consigneModeDegCh);
    this ->endGroup();
}

void Consignes::ecritConsigneGrVitExtCh()
{
    qDebug() << "ecritConsigneGrVitExtCh()";
    
    this ->beginGroup("Consignes");
    this ->setValue("ConsigneGrandeVitesseExterieurChauffage", consigneGrVitExtCh);
    this ->endGroup();
}

void Consignes::ecritConsigneGrVitExtFr()
{
    qDebug() << "ecritConsigneGrVitExtFr()";
    
    this ->beginGroup("Consignes");
    this ->setValue("ConsigneGrandeVitesseExterieurFroid", consigneGrVitExtFr);
    this ->endGroup();
}

void Consignes::ecritConsigneFinDegCh()
{
    qDebug() << "ecritConsigneFinDegCh()";
    
    this ->beginGroup("Consignes");
    this ->setValue("ConsigneFinDegivrageChauffage", consigneFinDegCh);
    this ->endGroup();
}

void Consignes::ecritConsigneDepartChauffageEnChauffage()
{
    qDebug() << "ecritConsigneDepartChauffageEnChauffage()";
    
    this ->beginGroup("Consignes");
    this ->setValue("ConsigneDepartChauffageEnModeChauffage", consigneDepartChauffageEnChauffage);
    this ->endGroup();

}

void Consignes::ecritConsigneDepartFroidEnChauffage()
{
    qDebug() << "ecritConsigneDepartFroidEnChauffage()";
    
    this ->beginGroup("Consignes");
    this ->setValue("ConsigneDepartFroidEnModeChauffage", consigneDepartFroidEnChauffage);
    this ->endGroup();
}

void Consignes::ecritConsignePeVitIntCh()
{
    qDebug() << "ecritConsignePeVitIntCh()";
    
    this ->beginGroup("Consignes");
    this ->setValue("ConsignePetiteVitesseInterieurChauffage", consignePeVitIntCh);
    this ->endGroup();
}

void Consignes::ecritConsigneDepartFroidEnFroid()
{
    qDebug() << "ecritConsigneDepartFroidEnFroid()";
    
    this ->beginGroup("Consignes");
    this ->setValue("ConsigneDepartFroidEnModeFroid", consigneDepartFroidEnFroid);
    this ->endGroup();
}

void Consignes::ecritConsigneDepartChauffageEnFroid()
{
    qDebug() << "ecritConsigneChauffageEnFroid()";
    
    this ->beginGroup("Consignes");
    this ->setValue("ConsigneDepartChauffageEnModeFroid", consigneDepartChauffageEnFroid);
    this ->endGroup();
}

void Consignes::ecritConsignePeVitIntFr()
{
    qDebug() << "ecritConsigneVentInCa()";
    
    this ->beginGroup("Consignes");
    this ->setValue("ConsignePetiteVitesseInterieurFroid", consignePeVitIntFr);
    this ->endGroup();
}

void Consignes::ecritConsigneDepartVentIntCh()
{    
    this ->beginGroup("Consignes");
    this ->setValue("ConsigneDepartVentilateurInterieurChauffage", consigneDepartVentIntCh);
    this ->endGroup();
}

void Consignes::ecritConsigneDegFr()
{
    qDebug() << "ecritConsigneDegFr()";
    
    this ->beginGroup("Consignes");
    this ->setValue("ConsigneDegivrageFroid", consigneDegFr);
    this ->endGroup();
}

void Consignes::ecritConsigneFinDegFr()
{
    qDebug() << "ecritConsigneFinDegFr()";
    
    this ->beginGroup("Consignes");
    this ->setValue("ConsigneFinDegivrageFroid", consigneFinDegFr);
    this ->endGroup();
}

void Consignes::ecritConsigneIntCanicule()
{
    qDebug() << "ecritConsigneIntCanicule()";
    
    m_mutexConsignes.lock();
    
    this ->beginGroup("Consignes");
    this ->setValue("ConsigneInterieurCanicule", consigneIntCanicule);
    this ->endGroup();
    
    m_mutexConsignes.unlock();
}

void Consignes::ecritConsigneVentIntCanicule()
{
    qDebug() << "ecritConsigneVentIntCanicule()";
    
    m_mutexConsignes.lock();
    
    this ->beginGroup("Consignes");
    this ->setValue("ConsigneVentilationInterieurCanicule", consigneVentIntCanicule);
    this ->endGroup();
    
    m_mutexConsignes.unlock();
}

void Consignes::lireConsignesGainable() // lire consignes
{
    qDebug() << "lireConsignesGainable()";
    
    this ->beginGroup("Consignes");
    consigneEteHiver = this ->value("ConsigneEteHiver").toDouble();
    consigneCanicule = this ->value("ConsigneCanicule").toDouble();
    consigneBlocChauf = this ->value("ConsigneBlocageChauffage").toDouble();
    consigneModeDegCh = this ->value("ConsigneModeDegivrageChauffage").toDouble();
    consigneGrVitExtCh = this ->value("ConsigneGrandeVitesseExterieurChauffage").toDouble();
    consigneGrVitExtFr = this ->value("ConsigneGrandeVitesseExterieurFroid").toDouble();
    consigneFinDegCh = this ->value("ConsigneFinDegivrageChauffage").toDouble();
    consigneDepartChauffageEnChauffage = this ->value("ConsigneDepartChauffageEnModeChauffage").toDouble();
    consigneDepartFroidEnChauffage = this ->value("ConsigneDepartFroidEnModeChauffage").toDouble();
    consignePeVitIntCh = this ->value("ConsignePetiteVitesseInterieurChauffage").toDouble();
    consigneDepartFroidEnFroid = this ->value("ConsigneDepartFroidEnModeFroid").toDouble();
    consigneDepartChauffageEnFroid = this ->value("ConsigneDepartChauffageEnModeFroid").toDouble();
    consignePeVitIntFr = this ->value("ConsignePetiteVitesseInterieurFroid").toDouble();
    consigneDepartVentIntCh = this ->value("ConsigneDepartVentilateurInterieurChauffage").toDouble();
    consigneDegFr = this ->value("ConsigneDegivrageFroid").toDouble();
    consigneFinDegFr = this ->value("ConsigneFinDegivrageFroid").toDouble();
    this ->endGroup();
}

void Consignes::lireConsigneIntCanicule()
{
    qDebug() << "lireConsigneIntCanicule()";
    
    this ->beginGroup("Consignes");
    consigneIntCanicule = this ->value("ConsigneInterieurCanicule").toDouble();
    this ->endGroup();
}

void Consignes::lireConsigneVentIntCanicule()
{
    qDebug() << "lireConsigneVentIntCanicule()";
    this ->beginGroup("Consignes");
    consigneVentIntCanicule = this ->value("ConsigneVentilationCanicule").toDouble();
    this ->endGroup();
}
