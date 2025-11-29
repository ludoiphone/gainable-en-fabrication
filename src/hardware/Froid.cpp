#include "Froid.hpp"

Froid::Froid(Logger& logger, Consignes* consignes, Temporisations* tempos)
    : m_logger(logger),
      m_compresseur(5), m_ventExt(13), m_vitVentExt(16),
      m_ventInt(19), m_vitVentInt(20), m_vanne4V(6),
      m_consignes(consignes), m_tempos(tempos),
      m_state(CoolingState::IDLE)
{
    stop();
}

void Froid::stop()
{
    setVentExt(false, false);
    setVentInt(false, false);
    m_compresseur.off();
    m_vanne4V.off();
    m_state = CoolingState::IDLE;
    
    m_logger.info("[IDLE] → ");
}

// ===========================================
//              UPDATE PRINCIPAL
// ===========================================
void Froid::update(double tempUExt, double tempUInt, double tempEInt)
{
    // 🚀 AUTO-START : si PAC appelle update, on démarre le froid
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
    // normal : tout est OFF
    stop();
}

void Froid::actionCooling(double tempUExt, double tempUInt, double tempEInt)
{
    double consignePVext = m_consignes->get("ConsignePetiteVitesseExterieurFroid");
    double consigneGVext = m_consignes->get("ConsigneGrandeVitesseExterieurFroid");
    double consigneGVint = m_consignes->get("ConsigneGrandeVitesseInterieurFroid");
    double hyst = 0.2;

    // === Ventilateur extérieur ===
    if(tempUExt >= consigneGVext) setVentExt(true, true);
    else if(tempUExt <= consignePVext) setVentExt(true, false);

    // === Ventilateur intérieur ===
    if(tempUInt >= consigneGVint + hyst) setVentInt(true, true);
    else if(tempUInt <= consigneGVint - hyst) setVentInt(true, false);

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

    m_logger.info("[STATE] → " + std::to_string((int)newState));
}

void Froid::setVentExt(bool on, bool gv)
{
    if(on) m_ventExt.on(); else m_ventExt.off();
    if(gv) m_vitVentExt.on(); else m_vitVentExt.off();
}

void Froid::setVentInt(bool on, bool gv)
{
    if(on) m_ventInt.on(); else m_ventInt.off();
    if(gv) m_vitVentInt.on(); else m_vitVentInt.off();
}

// ===========================================
//               GETTERS
// ===========================================
bool Froid::isVentExtEnMarche() const { return m_ventExt.isOn(); }
bool Froid::isVitesseVentExtEnMarche() const { return m_vitVentExt.isOn(); }
bool Froid::isVentIntEnMarche() const { return m_ventInt.isOn(); }
bool Froid::isVitesseVentIntEnMarche() const { return m_vitVentInt.isOn(); }
bool Froid::isCompresseurEnMarche() const { return m_compresseur.isOn(); }
bool Froid::isDegivrageEnMarche() const { return m_state == CoolingState::DEGIVRAGE; }
bool Froid::isEgouttageEnMarche() const { return m_state == CoolingState::EGOUTTAGE; }
