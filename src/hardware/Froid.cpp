#include "Froid.hpp"

Froid::Froid(Consignes* consignes, Temporisations* tempos)
    : m_compresseur(5), m_ventExt(13), m_vitVentExt(16),
      m_ventInt(19), m_vitVentInt(20), m_vanne4V(6),
      m_consignes(consignes), m_tempos(tempos),
      m_state(CoolingState::IDLE), m_lastVentExtSpeed(VentSpeedCool::VENT_OFF),
      m_lastVentIntSpeed(VentSpeedCool::VENT_OFF)
{
    stop();
}

void Froid::stop()
{
    setVentExt(false, false);
    setVentInt(false, false);
    m_compresseur.off();
    m_vanne4V.off();
    m_lastVentExtSpeed = VentSpeedCool::VENT_OFF;
    m_lastVentIntSpeed = VentSpeedCool::VENT_OFF;
    m_state = CoolingState::IDLE;
}

// ===========================================
//              UPDATE PRINCIPAL
// ===========================================
void Froid::update(double tempUExt, double tempUInt, double tempEInt)
{
    // AUTO-START : si PAC appelle update, on démarre le froid
    if(m_state == CoolingState::IDLE)
        enterState(CoolingState::COOLING);

    handleState(tempUExt, tempUInt, tempEInt);
}

void Froid::handleState(double tempUExt, double tempUInt, double tempEInt)
{
    switch(m_state)
    {
        case CoolingState::IDLE:       actionIdle(); break;
        case CoolingState::COOLING:    actionCooling(tempUExt, tempUInt, tempEInt); break;
        case CoolingState::DEGIVRAGE:  actionDegivrage(); break;
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
    double consigneGVint = m_consignes->get("ConsigneGrandeVitesseInterieurFroid");
    double hyst = 0.2;
    
    // === Ventilateur extérieur ===
    // Initialisation au premier cycle si aucune mémoire
    if (m_lastVentExtSpeed == VentSpeedCool::VENT_OFF) {
        // on décide d'une vitesse initiale selon la température
        if (tempUExt <= consignePVext) m_lastVentExtSpeed = VentSpeedCool::VENT_LOW;
        else if (tempUExt >= consigneGVext) m_lastVentExtSpeed = VentSpeedCool::VENT_HIGH;
        else m_lastVentExtSpeed = VentSpeedCool::VENT_LOW; // par défaut, petite vitesse si entre
    }

    // Mise à jour selon seuils en conservant la mémoire entre les seuils
    if (tempUExt <= consignePVext) {
        // force petite vitesse
        m_lastVentExtSpeed = VentSpeedCool::VENT_LOW;
        setVentExt(true, false);
    }
    else if (tempUExt >= consigneGVext) {
        // force grande vitesse
        m_lastVentExtSpeed = VentSpeedCool::VENT_HIGH;
        setVentExt(true, true);
    }
    else {
        // entre PV et GV -> garder la vitesse précédente et appliquer
        bool gv = (m_lastVentExtSpeed == VentSpeedCool::VENT_HIGH);
        bool on = (m_lastVentExtSpeed != VentSpeedCool::VENT_OFF);
        setVentExt(on, gv);
    }

    // === Ventilateur intérieur ===
    if(tempUInt >= consigneGVint + hyst) {
        m_lastVentIntSpeed = VentSpeedCool::VENT_HIGH;
        setVentInt(true, true);
    }
    else if(tempUInt <= consigneGVint - hyst) {
        m_lastVentIntSpeed = VentSpeedCool::VENT_LOW;
        setVentInt(true, false);
    }
    else {
        // Entre les seuils : conserver l’état précédent
        if(m_lastVentIntSpeed == VentSpeedCool::VENT_HIGH) setVentInt(true, true);
        else if(m_lastVentIntSpeed == VentSpeedCool::VENT_LOW) setVentInt(true, false);
        else setVentInt(false, false); // au cas où OFF
    }


    // === Vanne 4V ===
    if(!m_vanne4V.isOn()) m_vanne4V.on();

    // === Compresseur : tempo ===
    auto now = std::chrono::steady_clock::now();
    int tempoComp = m_tempos->get("tempoComp");
    int ecoule = std::chrono::duration_cast<std::chrono::seconds>(now - m_departTempoComp).count();

    if(!m_compresseur.isOn() && ecoule >= tempoComp)
        m_compresseur.on();

    // === Détection givre ===
    double consigneDeg = m_consignes->get("ConsigneDegivrageFroid");
    if(tempEInt <= consigneDeg)
        enterState(CoolingState::DEGIVRAGE);
}

void Froid::actionDegivrage()
{
    setVentExt(false, false);
    setVentInt(false, false);
    m_compresseur.off();
    m_vanne4V.off();

    auto now = std::chrono::steady_clock::now();
    int delay = m_tempos->get("tempoFinDegFr");
    int ecoule = std::chrono::duration_cast<std::chrono::seconds>(now - m_debutDegivrage).count();

    if(ecoule >= delay)
        enterState(CoolingState::EGOUTTAGE);
}

void Froid::actionEgouttage()
{
    setVentExt(true, true);
    setVentInt(false, false);
    m_compresseur.off();
    m_vanne4V.off();

    auto now = std::chrono::steady_clock::now();
    int delay = m_tempos->get("tempoEgouttage");
    int ecoule = std::chrono::duration_cast<std::chrono::seconds>(now - m_debutEgouttage).count();

    if(ecoule >= delay)
        enterState(CoolingState::IDLE);
}

// ===========================================
//                 HELPERS
// ===========================================
void Froid::enterState(CoolingState newState)
{
    m_state = newState;
    auto now = std::chrono::steady_clock::now();

    if(newState == CoolingState::COOLING)    m_departTempoComp = now;
    if(newState == CoolingState::DEGIVRAGE)  m_debutDegivrage = now;
    if(newState == CoolingState::EGOUTTAGE)  m_debutEgouttage = now;
}

void Froid::setVentExt(bool on, bool gv)
{
    if (on) m_ventExt.on(); else m_ventExt.off();
    if (gv) m_vitVentExt.on(); else m_vitVentExt.off();

    // Mettre la mémoire à jour seulement si on est sur ON (si off, on garde OFF)
    if (on) {
        m_lastVentExtSpeed = gv ? VentSpeedCool::VENT_HIGH : VentSpeedCool::VENT_LOW;
    } else {
        m_lastVentExtSpeed = VentSpeedCool::VENT_OFF;
    }
}

void Froid::setVentInt(bool on, bool gv)
{
    if(on) m_ventInt.on(); else m_ventInt.off();
    if(gv) m_vitVentInt.on(); else m_vitVentInt.off();
}

// ===========================================
//               GETTERS
// ===========================================
bool Froid::isVentExtEnMarche() const { return m_lastVentExtSpeed != VentSpeedCool::VENT_OFF; }
bool Froid::isVitesseVentExtEnMarche() const { return m_lastVentExtSpeed == VentSpeedCool::VENT_HIGH; }
bool Froid::isVentIntEnMarche() const { return m_ventInt.isOn(); }
bool Froid::isVitesseVentIntEnMarche() const { return m_vitVentInt.isOn(); }
bool Froid::isCompresseurEnMarche() const { return m_compresseur.isOn(); }
bool Froid::isDegivrageEnMarche() const { return m_state == CoolingState::DEGIVRAGE; }
bool Froid::isEgouttageEnMarche() const { return m_state == CoolingState::EGOUTTAGE; }
