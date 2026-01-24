#include "Chauffage.hpp"
#include "utils/Log.hpp"
#include <chrono>

using namespace std::chrono;

Chauffage::Chauffage(RelayManager& relays, Consignes* consignes, Temporisations* tempos)
    : m_relay(relays), m_consignes(consignes),
      m_tempos(tempos), m_state(HeatingState::IDLE)
{
    stop();
}

void Chauffage::stop()
{
    m_relay.setVentExt(VentExtSpeed::OFF);
    m_relay.setVentInt(VentIntSpeed::OFF);
    m_memoryFanInt = false;
    m_relay.setCompresseur(false);
    m_relay.setVanne4V(false);
    
    m_state = HeatingState::IDLE;
}

void Chauffage::update(double tempUExt, double tempEExt, double tempUInt, double tempEInt)
{
    // calcul du blocage chauffage (hystérésis)
    double hystBlocChauf = 0.2;
    double consigneBlocage = m_consignes->get("ConsigneBlocageChauffage");
    bool blocageChauffage = m_blocageChauffage;

    if (!m_blocageChauffage && tempUExt >= consigneBlocage + hystBlocChauf) {
        blocageChauffage = true;
    } else if (m_blocageChauffage && tempUExt <= consigneBlocage - hystBlocChauf) {
        blocageChauffage = false;
    }
    
    if (blocageChauffage != m_blocageChauffage) {
        m_blocageChauffage = blocageChauffage;

    if (m_blocageChauffage) {
        LOG_WARN("[CHAUFFAGE] Blocage chauffage actif");
    } else {
        LOG_INFO("[CHAUFFAGE] Fin blocage chauffage");
    }
}

    // On ne force plus l'entrée en PREHEAT depuis update().
    // La décision d'entrer en PREHEAT se fait dans actionIdle().
    handleState(tempUExt, tempEExt, tempUInt, tempEInt);
}

void Chauffage::handleState(double tempUExt, double tempEExt, double tempUInt, double tempEInt)
{
    switch (m_state) {
        case HeatingState::IDLE:      actionIdle(); break;
        case HeatingState::PREHEAT:   actionPreheat(tempEExt); break;
        case HeatingState::HEATING:   actionHeating(tempUExt, tempEExt, tempUInt, tempEInt); break;
        case HeatingState::DEFROST:   actionDefrost(tempEExt); break;
        case HeatingState::DRAINING:  actionDraining(); break;
    }
}

void Chauffage::controlExternalFan(double tempEExt) {
    
    double consigneStopVentExt = m_consignes->get("ConsigneStopVentilateurExterieurChauffage");
    double consigneGrandeVitExt = m_consignes->get("ConsigneGrandeVitesseExterieurChauffage");
    double hystFanExt = 0.3;
    
    VentExtSpeed current = m_relay.ventExt.speed();
    
    if (current == VentExtSpeed::OFF) {
        if (tempEExt < consigneStopVentExt - hystFanExt) {
            m_relay.setVentExt(VentExtSpeed::V1);
        }
    } else if (current == VentExtSpeed::V1) {
        if (tempEExt <= consigneGrandeVitExt) {
            m_relay.setVentExt(VentExtSpeed::V2);
        } else if (tempEExt >= consigneStopVentExt + hystFanExt) {
            m_relay.setVentExt(VentExtSpeed::OFF);
        }
    } else if (current == VentExtSpeed::V2) {
        if (tempEExt > consigneGrandeVitExt + hystFanExt) {
            m_relay.setVentExt(VentExtSpeed::V1);
        }
    }
}

// ---------------- IDLE ----------------
// IDLE s'assure que tout est arrêté. Si blocage -> on reste en IDLE.
// Si pas de blocage et que PAC demande chauffage (c'est-à-dire update() appelé),
// on bascule en PREHEAT depuis IDLE.
void Chauffage::actionIdle()
{
    // si blocage -> on reste en IDLE
    if (m_blocageChauffage) {
        return;
    }

    // Sinon, autoriser PREHEAT
    enterState(HeatingState::PREHEAT);
}

// ---------------- PREHEAT ----------------
// PREHEAT : ventilateur extérieur ON (petite vitesse) + tempo avant HEATING.
void Chauffage::actionPreheat(double tempEExt)
{
    if (m_blocageChauffage) {
        LOG_INFO("[CHAUFFAGE] Fin temporisation compresseur -> préchauffage");
        enterState(HeatingState::IDLE);
        return;
    }

    controlExternalFan(tempEExt);

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
    controlExternalFan(tempEExt);

    // Démarrage compresseur (on respecte tempoComp déjà réglé par PREHEAT)
    if (!m_relay.compresseur.isOn()) {
        m_relay.setCompresseur(true);
        m_departCompresseur = steady_clock::now();
        
        LOG_INFO("[CHAUFFAGE] Début temporisation départ dégivrage (" + std::to_string((int)m_tempos->get("tempoDepartDegCh")) + " s)");
    }

    // Détection dégivrage
    auto now = steady_clock::now();
    int tempoDepartDegivrage = static_cast<int>(m_tempos->get("tempoDepartDegCh"));
    int dureeComp = static_cast<int>(duration_cast<seconds>(now - m_departCompresseur).count());

    bool conditionTemps = (dureeComp >= tempoDepartDegivrage);
    bool conditionTemp = (tempUExt >= 5.0 && tempEExt < -3.0) ||
                         (tempEExt < (0.4 * tempUExt) - 5.0);

    if (conditionTemps && conditionTemp) {
        LOG_INFO("[CHAUFFAGE] Fin temporisation départ dégivrage et si conditions dégivrage atteinte -> démarrage du dégivrage");
        enterState(HeatingState::DEFROST);
        return;
    }

    // Ventilateur intérieur : démarrage si compresseur en marche et TEInt > consigne
    double consigneDepartVentInt = m_consignes->get("ConsigneDepartVentilationInterieurChauffage");
    double consignePetiteVitesseInt = m_consignes->get("ConsignePetiteVitesseInterieurChauffage");
    double hystVentInt = 0.3;
    
    if (tempEInt >= consigneDepartVentInt || m_relay.ventInt.isOn()) {
    // Activation du ventilateur si pas déjà ON
        if (!m_relay.ventInt.isOn()) m_relay.setVentInt(VentIntSpeed::V1);
        
        // Gestion hystérésis vitesse
        if (tempUInt >= consignePetiteVitesseInt + hystVentInt) {
            m_relay.setVentInt(VentIntSpeed::V1);
            m_memoryFanInt = false;
        } else if (tempUInt <= consignePetiteVitesseInt - hystVentInt) {
            m_relay.setVentInt(VentIntSpeed::V4);
            m_memoryFanInt = true;
        } else {
            m_relay.setVentInt(m_memoryFanInt ? VentIntSpeed::V4 : VentIntSpeed::V1);
        }
    }
}

// ---------------- DEGIVRAGE ----------------
void Chauffage::actionDefrost(double tempEExt)
{
    double consigneFinDeg = m_consignes->get("ConsigneFinDegivrageChauffage");
    auto now = steady_clock::now();
    int tempoFinDegivrage = static_cast<int>(m_tempos->get("tempoFinDegCh"));
    int ecoule = static_cast<int>(duration_cast<seconds>(now - m_debutDefrost).count());

    m_relay.setVentExt(VentExtSpeed::OFF);
    m_relay.setVentInt(VentIntSpeed::OFF);
    
    if (!m_relay.vanne4V.isOn()) {
        m_relay.setVanne4V(true);
    }

    if (ecoule >= tempoFinDegivrage || tempEExt >= consigneFinDeg) {
        LOG_INFO("[CHAUFFAGE] Fin temporisation ou si température échangeur extérieur atteinte fin dégivrage → égouttage");
        m_debutDraining = now;
        enterState(HeatingState::DRAINING);
    }
}

// ---------------- EGOUTTAGE ----------------
void Chauffage::actionDraining()
{
    m_relay.setVentExt(VentExtSpeed::V2);
    
    if (m_relay.compresseur.isOn()) {
        m_relay.setCompresseur(false);
    }
    if (m_relay.vanne4V.isOn()) {
        m_relay.setVanne4V(false);
    }

    if (m_debutDraining.time_since_epoch().count() == 0) {
        LOG_INFO("[CHAUFFAGE] Egouttage");
        m_debutDraining = steady_clock::now();
    }

    auto now = steady_clock::now();
    int tempoEgouttage = static_cast<int>(m_tempos->get("tempoEgouttage"));
    int ecoule = static_cast<int>(duration_cast<seconds>(now - m_debutDraining).count());

    if (ecoule >= tempoEgouttage) {
        LOG_INFO("[CHAUFFAGE] Fin temporisation égouttage -> retour stop");
        enterState(HeatingState::IDLE);
    }
}

// ---------------- enterState / helpers ----------------
void Chauffage::enterState(HeatingState newState)
{
    if (newState == m_state) return;
    
    LOG_INFO("[CHAUFFAGE] Changement d’état → " +
             std::string(
                 (newState == HeatingState::IDLE)      ? "IDLE" :
                 (newState == HeatingState::PREHEAT)   ? "PREHEAT" :
                 (newState == HeatingState::HEATING)   ? "HEATING" :
                 (newState == HeatingState::DEFROST) ? "DEGIVRAGE" :
                 (newState == HeatingState::DRAINING) ? "EGOUTTAGE" :
                 "???"
             ));
    
    m_state = newState;
    auto now = steady_clock::now();

    switch(newState) {
        case HeatingState::IDLE:
            stop();
            break;
        case HeatingState::PREHEAT:
            LOG_INFO("[CHAUFFAGE] Début temporisation compresseur (" + std::to_string((int)m_tempos->get("tempoComp")) + " s)");
            m_departTempoComp = now;
            break;
        case HeatingState::HEATING:
            //m_relay.vanne4V.off();
            break;
        case HeatingState::DEFROST:
            LOG_INFO("[CHAUFFAGE] Début temporisation fin dégivrage (" + std::to_string((int)m_tempos->get("tempoFinDegCh")) + " s)");
            m_debutDefrost = now;
            break;
        case HeatingState::DRAINING:
            LOG_INFO("[CHAUFFAGE] Début temporisation égouttage (" + std::to_string((int)m_tempos->get("tempoEgouttage")) + " s)");
            m_debutDraining = now;
            break;
        default:
            break;
    }
}

// ---------------- getters ----------------
bool Chauffage::isDefrostActif() const { return m_state == HeatingState::DEFROST; }
bool Chauffage::isDrainingActif() const { return m_state == HeatingState::DRAINING; }


