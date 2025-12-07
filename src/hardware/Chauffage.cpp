#include "Chauffage.hpp"
#include <chrono>

using namespace std::chrono;

Chauffage::Chauffage(Consignes* consignes, Temporisations* tempos)
    : m_ventExt(13), m_vitVentExt(16), m_compresseur(5),
      m_ventInt(19), m_vitVentInt(20), m_vanne4V(6),
      m_consignes(consignes), m_tempos(tempos),
      m_state(HeatingState::IDLE)
{
    stop();
}

void Chauffage::stop()
{
    setVentExt(false, false);
    setVentInt(false, false);
    m_compresseur.off();
    m_vanne4V.off();
    m_state = HeatingState::IDLE;
}

void Chauffage::update(double tempUExt, double tempEExt, double tempUInt, double tempEInt)
{
    // calcul du blocage chauffage (hystérésis)
    double hystBlocChauf = 0.2;
    double consigneBlocage = m_consignes->get("ConsigneBlocageChauffage");

    if (!m_blocageChauffage && tempUExt >= consigneBlocage + hystBlocChauf) {
        m_blocageChauffage = true;
    } else if (m_blocageChauffage && tempUExt <= consigneBlocage - hystBlocChauf) {
        m_blocageChauffage = false;
    }

    // On ne force plus l'entrée en PREHEAT depuis update().
    // La décision d'entrer en PREHEAT se fait dans actionIdle().
    handleState(tempUExt, tempEExt, tempUInt, tempEInt);
}

void Chauffage::handleState(double tempUExt, double tempEExt, double tempUInt, double tempEInt)
{
    switch (m_state) {
        case HeatingState::IDLE:      actionIdle(); break;
        case HeatingState::PREHEAT:   actionPreheat(); break;
        case HeatingState::HEATING:   actionHeating(tempUExt, tempEExt, tempUInt, tempEInt); break;
        case HeatingState::DEGIVRAGE: actionDegivrage(tempEExt); break;
        case HeatingState::EGOUTTAGE: actionEgouttage(); break;
    }
}

// ---------------- IDLE ----------------
// IDLE s'assure que tout est arrêté. Si blocage -> on reste en IDLE.
// Si pas de blocage et que PAC demande chauffage (c'est-à-dire update() appelé),
// on bascule en PREHEAT depuis IDLE.
void Chauffage::actionIdle()
{
    // s'assurer que tout est bien coupé
    stop();

    // si blocage -> on reste en IDLE
    if (m_blocageChauffage) {
        return;
    }

    // Sinon, autoriser PREHEAT
    enterState(HeatingState::PREHEAT);
}

// ---------------- PREHEAT ----------------
// PREHEAT : ventilateur extérieur ON (petite vitesse) + tempo avant HEATING.
void Chauffage::actionPreheat()
{
    if (m_blocageChauffage) {
        enterState(HeatingState::IDLE);
        return;
    }

    if (!m_ventExt.isOn()) setVentExt(true, false);

    auto now = steady_clock::now();
    int tempoComp = static_cast<int>(m_tempos->get("tempoComp"));
    int ecoule = static_cast<int>(duration_cast<seconds>(now - m_departTempoComp).count());

    if (ecoule >= tempoComp) {
        enterState(HeatingState::HEATING);
    }
}

// ---------------- HEATING ----------------
void Chauffage::actionHeating(double tempUExt, double tempEExt, double tempUInt, double tempEInt)
{
    if (m_blocageChauffage) {
        enterState(HeatingState::IDLE);
        return;
    }

    // Ventilateur extérieur: petite/grande vitesse selon consigne
    double consigneGrandeVitesse = m_consignes->get("ConsigneGrandeVitesseExterieurChauffage");
    if (tempUExt < consigneGrandeVitesse) setVentExt(true, false);
    else setVentExt(true, true);

    // Démarrage compresseur si besoin (on respecte tempoComp déjà réglé par PREHEAT)
    if (!m_compresseur.isOn()) {
        m_compresseur.on();
        m_departCompresseur = steady_clock::now();
    }

    // Détection dégivrage
    auto now = steady_clock::now();
    int tempoDepartDegivrage = static_cast<int>(m_tempos->get("tempoDepartDegCh"));
    int dureeComp = static_cast<int>(duration_cast<seconds>(now - m_departCompresseur).count());

    bool conditionTemps = (dureeComp >= tempoDepartDegivrage);
    bool conditionTemp = (tempUExt >= 5.0 && tempEExt < -3.0) ||
                         (tempEExt < (0.4 * tempUExt - 5.0));

    if (conditionTemps && conditionTemp) {
        enterState(HeatingState::DEGIVRAGE);
        return;
    }

    // Ventilateur intérieur : démarrage si compresseur en marche et TEInt > consigne
    double consigneDepartVentInt = m_consignes->get("ConsigneDepartVentilationInterieurChauffage");
    double consignePetiteVitesse = m_consignes->get("ConsignePetiteVitesseInterieurChauffage");
    double hystVentInt = 0.3;
    bool memoryFanInt = false;

    if(tempEInt > consigneDepartVentInt) setVentInt(true, true);
    if(m_ventInt.isOn()) {
        if(tempUInt <= consignePetiteVitesse - hystVentInt) setVentInt(true, true), memoryFanInt = true;
        else if(tempUInt >= consignePetiteVitesse + hystVentInt) setVentInt(true, false), memoryFanInt = false;
        else setVentInt(true, memoryFanInt);
    }
}

// ---------------- DEGIVRAGE ----------------
void Chauffage::actionDegivrage(double tempEExt)
{
    setVentExt(false, false);
    setVentInt(false, false);
    
    if (!m_vanne4V.isOn()) {
        m_vanne4V.on();
    }

    if (m_debutDegivrage.time_since_epoch().count() == 0)
        m_debutDegivrage = steady_clock::now();

    double consigneFinDeg = m_consignes->get("ConsigneFinDegivrageChauffage");
    auto now = steady_clock::now();
    int tempoFinDegivrage = static_cast<int>(m_tempos->get("tempoFinDegCh"));
    int ecoule = static_cast<int>(duration_cast<seconds>(now - m_debutDegivrage).count());

    if (ecoule >= tempoFinDegivrage || tempEExt >= consigneFinDeg) {
        m_debutEgouttage = now;
        enterState(HeatingState::EGOUTTAGE);
    }
}

// ---------------- EGOUTTAGE ----------------
void Chauffage::actionEgouttage()
{
    setVentExt(true, true);
    
    if (m_compresseur.isOn()) {
        m_compresseur.off();
    }
    if (m_vanne4V.isOn()) {
        m_vanne4V.off();
    }

    if (m_debutEgouttage.time_since_epoch().count() == 0)
        m_debutEgouttage = steady_clock::now();

    auto now = steady_clock::now();
    int tempoEgouttage = static_cast<int>(m_tempos->get("tempoEgouttage"));
    int ecoule = static_cast<int>(duration_cast<seconds>(now - m_debutEgouttage).count());

    if (ecoule >= tempoEgouttage) {
        // reset pour le prochain cycle
        m_departTempoComp = now;
        enterState(HeatingState::HEATING);
    }
}

// ---------------- enterState / helpers ----------------
void Chauffage::enterState(HeatingState newState)
{
    m_state = newState;
    auto now = steady_clock::now();

    switch(newState) {
        case HeatingState::PREHEAT:
            m_departTempoComp = now;
            break;
        case HeatingState::HEATING:
            break;
        case HeatingState::DEGIVRAGE:
            m_debutDegivrage = now;
            break;
        case HeatingState::EGOUTTAGE:
            m_debutEgouttage = now;
            break;
        default:
            break;
    }
}

void Chauffage::setVentExt(bool on, bool grandeVitesse)
{
    if (on) m_ventExt.on(); else m_ventExt.off();
    if (grandeVitesse) m_vitVentExt.on(); else m_vitVentExt.off();
}

void Chauffage::setVentInt(bool on, bool grandeVitesse)
{
    if (on) m_ventInt.on(); else m_ventInt.off();
    if (grandeVitesse) m_vitVentInt.on(); else m_vitVentInt.off();
}

// ---------------- getters ----------------
bool Chauffage::isVentExtEnMarche() const { return m_ventExt.isOn(); }
bool Chauffage::isVitesseVentExtEnMarche() const { return m_vitVentExt.isOn(); }
bool Chauffage::isVentIntEnMarche() const { return m_ventInt.isOn(); }
bool Chauffage::isVitesseVentIntEnMarche() const { return m_vitVentInt.isOn(); }
bool Chauffage::isCompresseurEnMarche() const { return m_compresseur.isOn(); }
bool Chauffage::isDegivrageEnMarche() const { return m_state == HeatingState::DEGIVRAGE; }
bool Chauffage::isEgouttageEnMarche() const { return m_state == HeatingState::EGOUTTAGE; }


