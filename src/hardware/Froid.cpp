#include "Froid.hpp"

Froid::Froid(Consignes* consignes, Temporisations* tempos)
    : m_compresseur(5), m_ventExt(13), m_vitVentExt(16),
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
    double hyst = 0.3;
    
    auto now = std::chrono::steady_clock::now();
    
    // === Ventilateur extérieur ===
    if(tempUExt <= consignePVext) setVentExt(true, false), m_memoryFanExt = false;
    else if(tempUExt >= consigneGVext) setVentExt(true, true), m_memoryFanExt = true;
    else setVentExt(true, m_memoryFanExt);

    // === Ventilateur intérieur ===
    if(tempUInt >= consignePVint + hyst) setVentInt(true, true), m_memoryFanInt = true;
    else if(tempUInt <= consignePVint - hyst) setVentInt(true, false), m_memoryFanInt = false;
    else setVentInt(true, m_memoryFanInt);

    // === Vanne 4V : tempo ===
    int tempoV4V = static_cast<int>(m_tempos->get("tempoV4V"));
    int ecouleV4V = std::chrono::duration_cast<std::chrono::seconds>(now - m_departTempoV4V).count();
    
    bool vanne4VOn = m_vanne4V.isOn();
    
    if(!m_vanne4V.isOn() && ecouleV4V >= tempoV4V) {
        m_vanne4V.on();
        if(!vanne4VOn)
            m_departTempoComp = now;
    }
    
    // === Compresseur : tempo ===
    int tempoComp = m_tempos->get("tempoComp");
    int ecouleComp = std::chrono::duration_cast<std::chrono::seconds>(now - m_departTempoComp).count();
        
    if(!m_compresseur.isOn() && m_vanne4V.isOn() && ecouleComp >= tempoComp)
        m_compresseur.on();

    // === Détection givre ===
    double consigneDeg = m_consignes->get("ConsigneDepartDegivrageFroid");
    if(tempEInt <= consigneDeg)
        enterState(CoolingState::DEGIVRAGE);
}

void Froid::actionDegivrage(double tempEInt)
{
    setVentExt(false, false);
    setVentInt(false, false);
    m_compresseur.off();
    m_vanne4V.off();

    double consigneFinDeg = m_consignes->get("ConsigneFinDegivrageFroid");
    auto now = std::chrono::steady_clock::now();
    int delay = m_tempos->get("tempoFinDegFr");
    int ecoule = std::chrono::duration_cast<std::chrono::seconds>(now - m_debutDegivrage).count();

    if(ecoule >= delay || tempEInt >= consigneFinDeg)
        enterState(CoolingState::EGOUTTAGE);
}

void Froid::actionEgouttage()
{
    setVentExt(false, false);
    setVentInt(false, false);

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

    if(newState == CoolingState::COOLING)    m_departTempoV4V = now;
    if(newState == CoolingState::DEGIVRAGE)  m_debutDegivrage = now;
    if(newState == CoolingState::EGOUTTAGE)  m_debutEgouttage = now;
}

void Froid::setVentExt(bool on, bool gv)
{
    if (on) m_ventExt.on(); else m_ventExt.off();
    if (gv) m_vitVentExt.on(); else m_vitVentExt.off();
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

