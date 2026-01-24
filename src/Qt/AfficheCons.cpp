#include "AfficheCons.hpp"
#include <QTimer>
#include <QLabel>
#include <QPushButton>
#include <QGraphicsDropShadowEffect>
#include <QShowEvent>
#include <QHideEvent>
#include <QFontDatabase>
#include <QFont>
#include <QColor>
#include <QPixmap>

AfficheCons::AfficheCons(QWidget *parent)
    : QWidget(parent)
{
    m_configCons.reset(new Consignes("/home/ludo/Desktop/PAC2/config/consignes.ini"));
    m_configCons->update();

    setFixedSize(1024, 600);

    // --- Background ---
    m_labelBackground = new QLabel(this);
    QPixmap bg("/home/ludo/Desktop/PAC2/images/afficheCons.jpg");
    m_labelBackground->setPixmap(bg.scaled(this->size(), Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation));
    m_labelBackground->lower();

    // --- Fonts ---
    int id = QFontDatabase::addApplicationFont("/home/ludo/Desktop/PAC2/fonts/digital-7.ttf");
    QString family;
    if (id != -1) {
        auto families = QFontDatabase::applicationFontFamilies(id);
        if (!families.isEmpty())
            family = families.at(0);
    }
    QFont digitalFont(family.isEmpty() ? "Monospace" : family, 30);

    int idTitre = QFontDatabase::addApplicationFont("/home/ludo/Desktop/PAC2/fonts/Coolvetica.ttf");
    QString titleFamily;
    if (idTitre != -1) {
        auto titleFamilies = QFontDatabase::applicationFontFamilies(idTitre);
        if (!titleFamilies.isEmpty())
            titleFamily = titleFamilies.at(0);
    }
    QFont titreFont(titleFamily.isEmpty() ? "Monospace" : titleFamily, 13);

    // --- Liste des clés et titres ---
    keys = QStringList()
        << "ConsigneEteHiver"
        << "ConsigneCanicule"
        << "ConsigneDelta"
        << "ConsigneBlocageChauffage"
        << "ConsigneGrandeVitesseExterieurChauffage"
        << "ConsigneGrandeVitesseExterieurFroid"
        << "ConsigneFinDegivrageChauffage"
        << "ConsignePetiteVitesseInterieurChauffage"
        << "ConsigneDepartDegivrageFroid"
        << "ConsignePetiteVitesseInterieurFroid"
        << "ConsigneDepartVentilationInterieurChauffage"
        << "ConsigneFinDegivrageFroid"
        << "ConsignePetiteVitesseExterieurFroid";

    QStringList titres = {
        "Eté / Hiver",
        "Canicule",
        "Delta",
        "Blocage Chauffage",
        "Grande Vitesse Ext Chauffage",
        "Grande Vitesse Ext Froid",
        "Fin Dégivrage Chauffage",
        "Petite Vitesse Int Chauffage",
        "Départ Dégivrage Froid",
        "Petite Vitesse Int Froid",
        "Départ Ventilation Int Chauffage",
        "Fin Dégivrage Froid",
        "Petite Vitesse Ext Froid"
    };

    QList<QColor> couleurs = {
        QColor("mediumvioletred"), QColor("orange"), QColor("orange"),
        QColor("red"), QColor("red"), QColor("blue"),
        QColor("red"), QColor("red"), QColor("blue"),
        QColor("blue"), QColor("red"), QColor("blue"),
        QColor("blue")
    };

    // --- Géométrie inspirée de ton ancien affichage ---
    QList<QRect> geometriesTitre = {
        {21, 6, 150, 22},   // Ete/Hiver
        {21, 87, 150, 22}, // Canicule
        {21, 170, 150, 22}, // Delta
        {165, 324, 180, 22},  // Blocage Chauffage
        {165, 401, 240, 22}, // GV Ext Chauffage
        {165, 87, 240, 22},// GV Ext Froid
        {330, 324, 240, 22},  // Fin Deg Chauffage
        {530, 324, 240, 22},  // PV Int Chauffage
        {775, 6, 240, 22}, // Depart Degivrage froid
        {530, 6, 180, 22}, // PV Int Froid
        {775, 324, 240, 22},  // Depart Ventil Int Chauffage
        {775, 87, 180, 22},// Fin Degivrage Froid
        {165, 6, 240, 22} // PV Ext Froid
    };

    QList<QRect> geometriesCons = {
        {21, 28, 120, 44}, {21, 109, 128, 44}, {21, 192, 128, 44},
        {165, 346, 120, 44}, {165, 423, 165, 44}, {165, 109, 165, 44},
        {335, 346, 120, 44}, {530, 346, 120, 44}, {775, 28, 120, 44}, 
        {530, 28, 165, 44}, {775, 346, 165, 44}, {775, 109, 165, 44},
        {165, 28, 165, 44}
    };

    // --- Création des labels ---
    for (int i = 0; i < titres.size(); ++i) {
        m_effects[i] = new QGraphicsDropShadowEffect(this);
        m_effects[i]->setBlurRadius(20);
        m_effects[i]->setOffset(0, 0);
        m_effects[i]->setColor(couleurs[i]);

        m_titreCons[i] = new QLabel(titres[i], this);
        m_titreCons[i]->setGeometry(geometriesTitre[i]);
        m_titreCons[i]->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        m_titreCons[i]->setFont(titreFont);
        m_titreCons[i]->setStyleSheet(QString("color:%1; background:transparent;").arg(couleurs[i].name()));

        m_afficheCons[i] = new QLabel(this);
        m_afficheCons[i]->setGeometry(geometriesCons[i]);
        m_afficheCons[i]->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        m_afficheCons[i]->setStyleSheet("color:black; background: transparent;");
        m_afficheCons[i]->setFont(digitalFont);
        m_afficheCons[i]->setGraphicsEffect(m_effects[i]);
    }

    readCons();

    // --- Boutons ---
    m_modifC = new QPushButton("Modif Consignes", this);
    m_modifC->setGeometry(735, 544, 128, 50);
    m_modifC->setStyleSheet("font-size: 15px;");

    m_quitter = new QPushButton("Quitter", this);
    m_quitter->setGeometry(870, 544, 128, 50);
    m_quitter->setStyleSheet("font-size: 15px; background-color: red;");

    connect(m_modifC, &QPushButton::clicked, this, &AfficheCons::demandeModif);
    connect(m_quitter, &QPushButton::clicked, this, &AfficheCons::demandeQuitter);

    // --- Timer ---
    m_updateTimer = new QTimer(this);
    connect(m_updateTimer, &QTimer::timeout, this, &AfficheCons::readCons);
}

// -------------------------------------------------
void AfficheCons::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);
    if (m_updateTimer && !m_updateTimer->isActive()) {
        m_updateTimer->start(m_timerCons);
    }
}

void AfficheCons::hideEvent(QHideEvent *event)
{
    QWidget::hideEvent(event);
    if (m_updateTimer && m_updateTimer->isActive()) {
        m_updateTimer->stop();
    }
}

void AfficheCons::readCons()
{
    m_configCons->update();
    
    for (int i = 0; i < keys.size(); ++i) {
        double val = m_configCons->get(keys[i].toStdString());
        m_afficheCons[i]->setText(QString("%1 °C").arg(val, 0, 'f', 1));
    }
}

void AfficheCons::modifConsignes()
{
    emit demandeModif();  // émet le signal vers AppWindow
}

