#include "Froid.hpp"
#include "utils/Log.hpp"

using namespace std::chrono;

Froid::Froid(RelayManager& relays, Consignes* consignes, Temporisations* tempos)
    : m_relay(relays), m_consignes(consignes),
      m_tempos(tempos), m_state(CoolingState::IDLE)
{
    stop();
}

void Froid::stop()
{
    m_relay.ventExt.set(VentExtSpeed::OFF);
    m_memoryFanExt = true;
    m_relay.ventInt.set(VentIntSpeed::OFF);
    m_memoryFanInt = true;
    m_relay.setCompresseur(false);
    m_relay.setVanne4V(false);
    
    m_state = CoolingState::IDLE;
}

// ===========================================
//              UPDATE PRINCIPAL
// ===========================================
void Froid::update(double tempUExt, double tempUInt, double tempEInt)
{
    if(m_state == CoolingState::IDLE) {
        enterState(CoolingState::COOLING);
    }

    handleState(tempUExt, tempUInt, tempEInt);
}

void Froid::handleState(double tempUExt, double tempUInt, double tempEInt)
{
    switch(m_state)
    {
        case CoolingState::IDLE:       actionIdle(); break;
        case CoolingState::COOLING:    actionCooling(tempUExt, tempUInt, tempEInt); break;
        case CoolingState::DEGIVRAGE:  actionDegivrage(tempEInt); break;
        case CoolingState::EGOUTTAGE:  actionEgouttage(); break;
    }
}

// ===========================================
//             ACTIONS PAR ETAT
// ===========================================
void Froid::actionIdle()
{
    stop();
}

void Froid::actionCooling(double tempUExt, double tempUInt, double tempEInt)
{    
    double consignePVext = m_consignes->get("ConsignePetiteVitesseExterieurFroid");
    double consigneGVext = m_consignes->get("ConsigneGrandeVitesseExterieurFroid");
    double consignePVint = m_consignes->get("ConsignePetiteVitesseInterieurFroid");
    double hystVentInt = 0.3;
    
    auto now = std::chrono::steady_clock::now();
    
    // === Ventilateur extérieur ===
    if (tempUExt >= consigneGVext) {
        m_relay.setVentExt(VentExtSpeed::V2);
        m_memoryFanExt = true;
    } else if (tempUExt <= consignePVext) {
        m_relay.setVentExt(VentExtSpeed::V1);
        m_memoryFanExt = false;
    } else {
        m_relay.setVentExt(m_memoryFanExt ? VentExtSpeed::V2 : VentExtSpeed::V1);
    }

    // === Ventilateur intérieur ===
    if (tempUInt >= consignePVint + hystVentInt) {
        m_relay.setVentInt(VentIntSpeed::V4);
        m_memoryFanInt = true;
    } else if (tempUInt <= consignePVint - hystVentInt) {
        m_relay.setVentInt(VentIntSpeed::V1);
        m_memoryFanInt = false;
    } else {
        m_relay.setVentInt(m_memoryFanInt ? VentIntSpeed::V4 : VentIntSpeed::V1);
    }

    // === Vanne 4V : tempo ===
    int tempoV4V = static_cast<int>(m_tempos->get("tempoV4V"));
    int ecouleV4V = static_cast<int>(duration_cast<seconds>(now - m_departTempoV4V).count());
    
    //bool vanne4VOn = m_vanne4V.isOn();
    
    if(!m_relay.vanne4V.isOn() && ecouleV4V >= tempoV4V) {
        m_relay.setVanne4V(true);
        m_departTempoComp = now;
        LOG_INFO("[FROID] Début temporisation compresseur (" + std::to_string((int)m_tempos->get("tempoComp")) + " s)");
    }
    
    // === Compresseur : tempo ===
    int tempoComp = static_cast<int>(m_tempos->get("tempoComp"));
    int ecouleComp = static_cast<int>(duration_cast<seconds>(now - m_departTempoComp).count());
        
    if(!m_relay.compresseur.isOn() && m_relay.vanne4V.isOn() && ecouleComp >= tempoComp) {
        m_relay.setCompresseur(true);
        m_departTempoDegFr = now;
        LOG_INFO("[FROID] Début temporisation départ dégivrage (" + std::to_string((int)m_tempos->get("tempoDepartDegFr")) + " s)");
    }

    // === Détection givre ===
    double consigneDeg = m_consignes->get("ConsigneDegivrageFroid");
    int tempoDepartDegivrage = static_cast<int>(m_tempos->get("tempoDepartDegFr"));
    int ecoule = static_cast<int>(duration_cast<seconds>(now - m_departTempoDegFr).count());
    
    if (m_relay.compresseur.isOn() && tempEInt <= consigneDeg && ecoule >= tempoDepartDegivrage) {
        LOG_INFO("[FROID] Fin temporisation départ dégivrage et condition température atteinte -> dégivrage");
        enterState(CoolingState::DEGIVRAGE);
    }
}

void Froid::actionDegivrage(double tempEInt)
{
    m_relay.setVentExt(VentExtSpeed::OFF);
    m_relay.setVentInt(VentIntSpeed::OFF);
    m_relay.setCompresseur(false);
    m_relay.setVanne4V(false);

    double consigneFinDeg = m_consignes->get("ConsigneFinDegivrageFroid");
    auto now = std::chrono::steady_clock::now();
    int delay = static_cast<int>(m_tempos->get("tempoFinDegFr"));
    int ecoule = static_cast<int>(duration_cast<seconds>(now - m_debutDegivrage).count());

    if(ecoule >= delay || tempEInt >= consigneFinDeg) {
        LOG_INFO("[FROID] Fin temporisation ou si température échangeur intérieur atteinte fin dégivrage -> égouttage");
        enterState(CoolingState::EGOUTTAGE);
    }
}

void Froid::actionEgouttage()
{
    /*setVentExt(false, false);
    setVentInt(false, false);*/

    auto now = std::chrono::steady_clock::now();
    int delay = static_cast<int>(m_tempos->get("tempoEgouttage"));
    int ecoule = static_cast<int>(duration_cast<seconds>(now - m_debutEgouttage).count());

    if(ecoule >= delay) {
        LOG_INFO("[froid] Fin temporisation égouttage → retour stop");
        enterState(CoolingState::IDLE);
    }
}

// ===========================================
//                 HELPERS
// ===========================================
void Froid::enterState(CoolingState newState)
{
    if (newState == m_state) return;
    
    LOG_INFO("[FROID] Changement d’état → " +
             std::string(
                 (newState == CoolingState::IDLE)      ? "IDLE" :
                 (newState == CoolingState::COOLING)   ? "COOLING" :
                 (newState == CoolingState::DEGIVRAGE) ? "DEGIVRAGE" :
                 (newState == CoolingState::EGOUTTAGE) ? "EGOUTTAGE" :
                 "???"
             ));
             
    m_state = newState;
    auto now = std::chrono::steady_clock::now();
    
    switch(newState) {
        case CoolingState::COOLING:
            LOG_INFO("[FROID] Début temporisation V4V (" + std::to_string((int)m_tempos->get("tempoV4V")) + " s)");
            m_departTempoV4V = now;
            break;
        case CoolingState::DEGIVRAGE:
            LOG_INFO("[FROID] Début temporisation fin dégivrage (" + std::to_string((int)m_tempos->get("tempoFinDegFr")) + " s)");
            m_debutDegivrage = now;
            break;
        case CoolingState::EGOUTTAGE:
            LOG_INFO("[FROID] Début temporisation égouttage (" + std::to_string((int)m_tempos->get("tempoEgouttage")) + " s)");
            m_debutEgouttage = now;
            break;
        default:
            break;
    }
}

// ===========================================
//               GETTERS
// ===========================================
bool Froid::isDefrostActif() const { return m_state == CoolingState::DEGIVRAGE; }
bool Froid::isDrainingActif() const { return m_state == CoolingState::EGOUTTAGE; }

