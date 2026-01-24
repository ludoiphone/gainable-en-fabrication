#include "ServiceTest.hpp"
#include "core/PacController.hpp"
#include <QPushButton>
#include <QLabel>
#include <QFont>
#include <QGroupBox>
#include <QVariant>
#include <QTimer>
#include <QMovie>

ServiceTest::ServiceTest(PacController* pac, QWidget* parent)
    : QWidget(parent), m_pac(pac), m_gifsRunning(false)
{
    setFixedSize(1024, 600);
    
    // ===== BACKGROUND ======
    m_labelBackgroundServiceTest = new QLabel(this);
    m_labelBackgroundServiceTest->setPixmap(QPixmap("/home/ludo/Desktop/PAC2/images/serviceTest.jpg"));
    m_labelBackgroundServiceTest->setGeometry(0, 0, 1024, 600);
    m_labelBackgroundServiceTest->lower();

    // ===== TITRE =====
    m_title = new QLabel("MODE SERVICE / MAINTENANCE", this);
    m_title->setGeometry(0, 20, 1024, 40);
    m_title->setAlignment(Qt::AlignCenter);
    m_title->setStyleSheet("color: red;");
    m_title->setFont(QFont("Arial", 18, QFont::Bold));

    // ===== BOUTONS MENU =====
    m_relaysBtn = new QPushButton("Test relais", this);
    m_relaysBtn->setGeometry(200, 100, 300, 60);

    m_gifsBtn = new QPushButton("Test gifs", this);
    m_gifsBtn->setGeometry(520, 100, 300, 60);

    m_switchsBtn = new QPushButton("Test entrée PAC", this);
    m_switchsBtn->setGeometry(200, 170, 300, 60);

    m_quitBtn = new QPushButton("Quitter", this);
    m_quitBtn->setGeometry(870, 544, 128, 50);
    m_quitBtn->setStyleSheet("background-color:red; font-size:15px;");
    
    // Crée le widget GIFs
    m_gifsWidget = new Gifs(m_pac, this);
    m_gifsWidget->setGeometry(0, 0, 1024, 600);
    m_gifsWidget->setup(); // positionne les GIFs
    m_gifsWidget->setAttribute(Qt::WA_TransparentForMouseEvents);
    m_gifsWidget->hide();
    
    // Crée la liste de tous les labels pour les afficher séquentiellement
    m_allGifLabels = {
	m_gifsWidget->m_ventExtLabel,
	m_gifsWidget->m_ventIntLabel,
	m_gifsWidget->m_vitesseVent1LabelExt,
	m_gifsWidget->m_vitesseVent1LabelInt,
	m_gifsWidget->m_vitesseVent2Label,
	m_gifsWidget->m_vitesseVent4Label,
	m_gifsWidget->m_compresseurFroidLabel,
	m_gifsWidget->m_compresseurChauffageLabel,
	m_gifsWidget->m_caniculeLabel,
	m_gifsWidget->m_degivrageLabel,
	m_gifsWidget->m_egouttageLabel,
	m_gifsWidget->m_filtreANettoyerLabel,
	m_gifsWidget->m_filtreNettoyageEnCourLabel,
	m_gifsWidget->m_filtreArretAbsoluLabel,
	m_gifsWidget->m_saisonsLabel,
	m_gifsWidget->m_eteLabel,
	m_gifsWidget->m_hiverLabel,
	m_gifsWidget->m_stabilisationLabel
    };
    
    // ===== AffICHAGE INPUTS ====
    m_inputsBox = new QGroupBox("Test entrées PAC", this);
    m_inputsBox->setGeometry(200, 260, 624, 200);
    m_inputsBox->hide();

    m_thermostatLabel = new QLabel("Thermostat : ---", m_inputsBox);
    m_thermostatLabel->setGeometry(40, 50, 540, 50);
    m_thermostatLabel->setFont(QFont("Arial", 16, QFont::Bold));

    m_filtreLabel = new QLabel("Filtre : ---", m_inputsBox);
    m_filtreLabel->setGeometry(40, 120, 540, 50);
    m_filtreLabel->setFont(QFont("Arial", 16, QFont::Bold));

    // ===== GROUPBOX RELAIS =====
    m_relaysBox = new QGroupBox("Test des relais (UI uniquement)", this);
    m_relaysBox->setGeometry(80, 260, 864, 280);
    m_relaysBox->hide();

    // ===== RELAIS - COLONNE GAUCHE =====
    m_relayEteHiver = new QPushButton("Relai Été / Hiver : OFF", m_relaysBox);
    m_relayEteHiver->setGeometry(30, 40, 380, 50);

    m_relayCompresseur = new QPushButton("Relai Compresseur : OFF", m_relaysBox);
    m_relayCompresseur->setGeometry(30, 100, 380, 50);

    m_relayVentilateurExt = new QPushButton("Relai Ventilateur Extérieur : OFF", m_relaysBox);
    m_relayVentilateurExt->setGeometry(30, 160, 380, 50);

    m_relayVitessesExt = new QPushButton("Relai Vitesses Ventilateur Extérieur : OFF", m_relaysBox);
    m_relayVitessesExt->setGeometry(30, 220, 380, 50);

    // ===== RELAIS - COLONNE DROITE =====
    m_relayV4V = new QPushButton("Relai V4V : OFF", m_relaysBox);
    m_relayV4V->setGeometry(450, 40, 380, 50);

    m_relayVentilateurInt = new QPushButton("Relai Ventilateur Intérieur : OFF", m_relaysBox);
    m_relayVentilateurInt->setGeometry(450, 100, 380, 50);

    m_relayVitessesInt = new QPushButton("Relai Vitesses Ventilateur Intérieur : OFF", m_relaysBox);
    m_relayVitessesInt->setGeometry(450, 160, 380, 50);
    
    m_relayAllOff = new QPushButton("TOUT RELAIS OFF (Sécurité)", m_relaysBox);
    m_relayAllOff->setGeometry(450, 220, 380, 50);
    m_relayAllOff->setStyleSheet("background-color:red; font-size:15px;");
    
    m_pac->serviceAllOff();

    connect(m_relayEteHiver, &QPushButton::clicked, this, [this]() {
        bool on = !m_relayEteHiver->property("on").toBool();
	setRelayBtn(m_relayEteHiver, "Relai Eté/Hiver", on);
	if (m_pac) m_pac->serviceSetRelayEteHiver(on);
    });
    
    connect(m_relayCompresseur, &QPushButton::clicked, this, [this]() {
        bool on = !m_relayCompresseur->property("on").toBool();
	setRelayBtn(m_relayCompresseur, "Relai Compresseur", on);
	if (m_pac) m_pac->serviceSetRelayCompresseur(on);
    });
    
    connect(m_relayVentilateurExt, &QPushButton::clicked, this, [this]() {
        bool on = !m_relayVentilateurExt->property("on").toBool();
	setRelayBtn(m_relayVentilateurExt, "Relai Ventilateur Extérieur", on);
	if (!m_pac) return;
	m_pac->serviceSetVentilateurExt(on ? VentExtSpeed::V1 : VentExtSpeed::OFF);
	setRelayBtn(m_relayVitessesExt, "Relai Vitesses Ventilateur Extérieur", false);
    });
    
    connect(m_relayVitessesExt, &QPushButton::clicked, this, [this]() {
        bool on = !m_relayVitessesExt->property("on").toBool();
	setRelayBtn(m_relayVitessesExt, "Relai Vitesses Ventilateur Extérieur", on);
	if (!m_pac) return;
	m_pac->serviceSetVentilateurExt(on ? VentExtSpeed::V2 : VentExtSpeed::OFF);
	setRelayBtn(m_relayVentilateurExt, "Relai Ventilateur Extérieur", on);
    });
    
    connect(m_relayV4V, &QPushButton::clicked, this, [this]() {
        bool on = !m_relayV4V->property("on").toBool();
	setRelayBtn(m_relayV4V, "Relai V4V", on);
	if (m_pac) m_pac->serviceSetV4V(on);
    });
    
    connect(m_relayVentilateurInt, &QPushButton::clicked, this, [this]() {
        bool on = !m_relayVentilateurInt->property("on").toBool();
	setRelayBtn(m_relayVentilateurInt, "Relai Ventilateur Intérieur", on);
	if (!m_pac) return;
	m_pac->serviceSetVentilateurInt(on ? VentIntSpeed::V1 : VentIntSpeed::OFF);
	setRelayBtn(m_relayVitessesInt, "Relai Vitesses Ventilateur Intérieur", false);
    });
    
    connect(m_relayVitessesInt, &QPushButton::clicked, this, [this]() {
        bool on = !m_relayVitessesInt->property("on").toBool();
	setRelayBtn(m_relayVitessesInt, "Relai Vitesses Ventilateur Intérieur", on);
	if (!m_pac) return;
	m_pac->serviceSetVentilateurInt(on ? VentIntSpeed::V4 : VentIntSpeed::OFF);
	setRelayBtn(m_relayVentilateurInt, "Relai Ventilateur Intérieur", on);
    });
    
    connect(m_relayAllOff, &QPushButton::clicked, this, [this]() {
	if(!m_pac) return;
	
	m_pac->serviceAllOff();
	
	setRelayBtn(m_relayEteHiver, "Relai Eté/Hiver", false);
	setRelayBtn(m_relayCompresseur, "Relai Compresseur", false);
	setRelayBtn(m_relayVentilateurExt, "Relai Ventilateur Extérieur", false);
	setRelayBtn(m_relayVitessesExt, "Relai Vitesses Ventilateur Extérieur", false);
	setRelayBtn(m_relayV4V, "Relai V4V", false);
	setRelayBtn(m_relayVentilateurInt, "Relai Ventilateur Intérieur", false);
	setRelayBtn(m_relayVitessesInt, "Relai Vitesses Ventilateur Intérieur", false);
    });
    
    // Initialisation
    m_gifIndex = 0;
    m_timerGifs = new QTimer(this);
    m_timerGifs->setInterval(500); // 0,5s entre chaque GIF
    connect(m_timerGifs, &QTimer::timeout, this, &ServiceTest::showNextGif);
    
    // Bouton Test GIFs
    connect(m_gifsBtn, &QPushButton::clicked, this, [this]() {
	
	// === CAS 1 : le test est actif ou affiché → on coupe tout ===
	if (m_gifsRunning) {
	    stopGifsTest();
	    return;
	}
	
	stopRelaysTest();
	stopInputsTest();

	hideAllGifs();
	m_gifsWidget->show();
	m_gifIndex = 0;
	m_gifsRunning = true;
	m_timerGifs->start();
	
	m_gifsBtn->setStyleSheet(
	    "background-color:#0275d8;"
	    "color:white;"
	    "font-weight:bold;"
	);
    });

    // ===== Bouton Test relais =====
    connect(m_relaysBtn, &QPushButton::clicked, this, [this]() {
	if (m_relaysBox->isVisible()) {
	    stopRelaysTest();
	    return;
	}
	
	stopGifsTest();
	stopInputsTest();
	
	m_relaysBox->show();	
	m_relaysBtn->setStyleSheet("background-color:#0275d8; color:white; font-weight:bold;");
    });
    
    // ===== Bouton Test Switch ======
    connect(m_switchsBtn, &QPushButton::clicked, this, [this]() {
	if (m_inputsBox->isVisible()) {
	    stopInputsTest();
	    return;
	}
	
	stopGifsTest();
	stopRelaysTest();
	
	m_inputsBox->show();
	m_switchsBtn->setStyleSheet("background-color:#0275d8; color:white; font-weight:bold;");
    });

    // ===== TIMER INPUTS =====
    m_timerInputs = new QTimer(this);
    connect(m_timerInputs, &QTimer::timeout, this, &ServiceTest::updateInputs);
    m_timerInputs->start(500);
    
    connect(m_quitBtn, &QPushButton::clicked, this, &ServiceTest::demandeQuitter);
}

void ServiceTest::setRelayBtn(QPushButton* btn, const QString& label, bool on)
{
    btn->setProperty("on", on);
    btn->setText(label + (on ? " : ON" : " : OFF"));

    if (on) {
        btn->setStyleSheet(
            "background-color: green;"
            "color: white;"
            "font-weight: bold;");
    } else {
        btn->setStyleSheet("");
    }
}

void ServiceTest::updateInputs()
{
    if (!m_pac) return;

    setState(
        m_thermostatLabel,
        "Thermostat",
        m_pac->thermostatOn(),   // ON = actif
        false
    );

    setState(
        m_filtreLabel,
        "Filtre",
        m_pac->isFiltreOpen(), // true = ouvert
        false
    );
}

void ServiceTest::setState(QLabel* lbl, const QString& label, bool on, bool inverted)
{
    bool active = inverted ? !on : on;

    lbl->setText(label + " : " + (active ? "ON" : "OFF"));

    if (active) {
        lbl->setStyleSheet(
            "background-color: green;"
            "color: white;"
            "padding:8px;"
            "border-radius:6px;"
        );
    } else {
        lbl->setStyleSheet(
            "background-color: #b22222;"
            "color: white;"
            "padding:8px;"
            "border-radius:6px;"
        );
    }
}

void ServiceTest::showNextGif()
{
    if (m_gifIndex >= m_allGifLabels.size()) {
        m_timerGifs->stop();
        return;
    }

    QLabel* nextLbl = m_allGifLabels[m_gifIndex];

    // Cacher les GIFs précédents à la même position
    QRect nextRect = nextLbl->geometry();
    for (int i = 0; i < m_gifIndex; ++i) {
        QLabel* prevLbl = m_allGifLabels[i];
        if (prevLbl->geometry() == nextRect) {
            prevLbl->hide();
            if (prevLbl->movie()) prevLbl->movie()->stop();
        }
    }

    // Affiche et lance le GIF courant
    nextLbl->show();
    if (nextLbl->movie()) nextLbl->movie()->start();

    m_gifIndex++;
}

void ServiceTest::hideAllGifs()
{
    m_timerGifs->stop();
    m_gifIndex = 0;
    m_gifsRunning = false;

    for (QLabel* lbl : m_allGifLabels) {
        lbl->hide();
        if (lbl->movie()) lbl->movie()->stop();
    }
}

void ServiceTest::stopGifsTest()
{
    if (!m_gifsRunning)
        return;

    hideAllGifs();
    m_gifsWidget->hide();
    m_gifsRunning = false;
    m_gifsBtn->setStyleSheet("");
}

void ServiceTest::stopRelaysTest()
{
    if (!m_relaysBox->isVisible())
        return;

    m_relaysBox->hide();
    m_relaysBtn->setStyleSheet("");
    
    setRelayBtn(m_relayEteHiver, "Relai Eté/Hiver", false);
    setRelayBtn(m_relayCompresseur, "Relai Compresseur", false);
    setRelayBtn(m_relayVentilateurExt, "Relai Ventilateur Extérieur", false);
    setRelayBtn(m_relayVitessesExt, "Relai Vitesses Ventilateur Extérieur", false);
    setRelayBtn(m_relayV4V, "Relai V4V", false);
    setRelayBtn(m_relayVentilateurInt, "Relai Ventilateur Intérieur", false);
    setRelayBtn(m_relayVitessesInt, "Relai Vitesses Ventilateur Intérieur", false);
    
    m_pac->serviceAllOff();
}

void ServiceTest::stopInputsTest()
{
    if (!m_inputsBox->isVisible())
        return;

    m_inputsBox->hide();
    m_switchsBtn->setStyleSheet("");
}
