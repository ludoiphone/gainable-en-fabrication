#include "affichecons.hpp"

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
#include <QDebug>

AfficheCons::AfficheCons(QWidget *parent)
    : QWidget(parent)
{
    m_configCons.reset(new Consignes("consignes.ini"));
    m_configCons->update();

    setFixedSize(1024, 600);

    // --- Background ---
    m_labelBackground = new QLabel(this);
    QPixmap bg("/home/ludo/Desktop/test4/images/afficheCons.jpg");
    m_labelBackground->setPixmap(bg.scaled(this->size(), Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation));
    m_labelBackground->lower();

    // --- Fonts ---
    int id = QFontDatabase::addApplicationFont("/home/ludo/Desktop/test4/fonts/digital-7.ttf");
    QString family;
    if (id != -1) {
        auto families = QFontDatabase::applicationFontFamilies(id);
        if (!families.isEmpty())
            family = families.at(0);
    }
    QFont digitalFont(family.isEmpty() ? "Monospace" : family, 30);

    int idTitre = QFontDatabase::addApplicationFont("/home/ludo/Desktop/test4/fonts/Coolvetica.ttf");
    QString titleFamily;
    if (idTitre != -1) {
        auto titleFamilies = QFontDatabase::applicationFontFamilies(idTitre);
        if (!titleFamilies.isEmpty())
            titleFamily = titleFamilies.at(0);
    }
    QFont titreFont(titleFamily.isEmpty() ? "Monospace" : titleFamily, 11);

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
        << "ConsigneGrandeVitesseInterieurFroid"
        << "ConsigneDepartVentilateurInterieurChauffage"
        << "ConsigneDegivrageFroid"
        << "ConsigneFinDegivrageFroid"
        << "ConsigneVentilationInterieurCanicule"
        << "ConsignePetiteVitesseExterieurFroid";

    QStringList titres = {
        "Eté / Hiver",
        "Canicule",
        "Delta",
        "Blocage Chauffage",
        "GV Ext Chauffage",
        "GV Ext Froid",
        "Fin Dégivrage Chauffage",
        "PV Int Chauffage",
        "GV Int Froid",
        "Départ Ventil Int Chauffage",
        "Dégivrage Froid",
        "Fin Dégivrage Froid",
        "Ventil Canicule",
        "PV Ext Froid"
    };

    QList<QColor> couleurs = {
        QColor("mediumvioletred"), QColor("orange"), QColor("white"),
        QColor("red"), QColor("red"), QColor("blue"),
        QColor("red"), QColor("red"), QColor("blue"),
        QColor("red"), QColor("blue"), QColor("blue"),
        QColor("orange"), QColor("blue")
    };

    // --- Géométrie inspirée de ton ancien affichage ---
    QList<QRect> geometriesTitre = {
        {51, 6, 150, 22},   // Ete/Hiver
        {51, 267, 150, 22}, // Canicule
        {51, 478, 150, 22}, // Delta
        {165, 6, 180, 22},  // Blocage Chauffage
        {165, 87, 240, 22}, // GV Ext Chauffage
        {165, 170, 240, 22},// GV Ext Froid
        {335, 6, 180, 22},  // Fin Deg Chauffage
        {512, 6, 240, 22},  // GV Int Chauffage
        {512, 82, 240, 22}, // GV Int Froid
        {773, 6, 240, 22},  // Ventil Int Chauffage
        {773, 324, 180, 22},// Degivrage Froid
        {773, 401, 180, 22},// Fin Degivrage Froid
        {773, 478, 200, 22},// Ventil Canicule
        {512, 165, 240, 22} // PV Ext Froid
    };

    QList<QRect> geometriesCons = {
        {51, 28, 120, 44}, {51, 289, 128, 44}, {51, 500, 128, 44},
        {165, 28, 120, 44}, {165, 109, 165, 44}, {165, 187, 165, 44},
        {335, 28, 120, 44}, {512, 28, 120, 44}, {512, 109, 165, 44},
        {773, 28, 120, 44}, {773, 346, 165, 44}, {773, 423, 165, 44},
        {773, 500, 165, 44}, {512, 187, 165, 44}
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
        qDebug() << "AfficheCons timer démarré";
    }
}

void AfficheCons::hideEvent(QHideEvent *event)
{
    QWidget::hideEvent(event);
    if (m_updateTimer && m_updateTimer->isActive()) {
        m_updateTimer->stop();
        qDebug() << "AfficheCons timer arrêté";
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

