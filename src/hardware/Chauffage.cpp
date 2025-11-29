#include "Chauffage.hpp"
#include <chrono>

using namespace std::chrono;

Chauffage::Chauffage(Logger& logger, Consignes* consignes, Temporisations* tempos)
    : m_logger(logger),
      m_ventExt(13), m_vitVentExt(16), m_compresseur(5),
      m_ventInt(19), m_vitVentInt(20), m_vanne4V(6),
      m_consignes(consignes), m_tempos(tempos),
      m_state(HeatingState::IDLE)
{
    auto now = steady_clock::now();
    m_departTempoComp = now;
    m_departCompresseur = now;
    m_debutDegivrage = now;
    m_debutEgouttage = now;
    stop(); // s'assure que tout est bien coupé au démarrage
}

void Chauffage::stop()
{
    setVentExt(false, false);
    setVentInt(false, false);
    m_compresseur.off();
    m_vanne4V.off();

    m_ventExtEnMarche.store(false);
    m_vitesseVentExtEnMarche.store(false);
    m_ventIntEnMarche.store(false);
    m_vitesseVentIntEnMarche.store(false);
    m_compresseurEnMarche.store(false);
    m_degivrageEnMarche.store(false);
    m_egouttageEnMarche.store(false);

    m_state = HeatingState::IDLE;
}

void Chauffage::update(double tempUExt, double tempEExt, double tempUInt, double tempEInt)
{
    // calcul du blocage chauffage (hystérésis)
    double hystBlocChauf = 0.2;
    double consigneBlocage = m_consignes->get("ConsigneBlocageChauffage");

    if (!m_blocageChauffage.load() && tempUExt >= consigneBlocage + hystBlocChauf) {
        m_blocageChauffage.store(true);
        m_logger.info("[CHAUFFAGE] Blocage chauff. activé");
    } else if (m_blocageChauffage.load() && tempUExt <= consigneBlocage - hystBlocChauf) {
        m_blocageChauffage.store(false);
        m_logger.info("[CHAUFFAGE] Blocage chauff. désactivé");
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
        case HeatingState::DEGIVRAGE: actionDegivrage(); break;
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
    if (m_blocageChauffage.load()) {
        m_logger.info("[IDLE] Chauffage bloqué, restant en IDLE");
        return;
    }

    // Sinon, autoriser PREHEAT
    enterState(HeatingState::PREHEAT);
}

// ---------------- PREHEAT ----------------
// PREHEAT : ventilateur extérieur ON (petite vitesse) + tempo avant HEATING.
void Chauffage::actionPreheat()
{
    if (m_blocageChauffage.load()) {
        m_logger.info("[PREHEAT] Blocage détecté -> retour IDLE");
        enterState(HeatingState::IDLE);
        return;
    }

    // Ventilateur extérieur ON petite vitesse
    if (!m_ventExtEnMarche.load()) setVentExt(true, false);

    auto now = steady_clock::now();
    int tempoComp = static_cast<int>(m_tempos->get("tempoComp"));
    int ecoule = static_cast<int>(duration_cast<seconds>(now - m_departTempoComp).count());

    m_logger.info("[PREHEAT] ecoule=" + std::to_string(ecoule) + " tempoComp=" + std::to_string(tempoComp));

    if (ecoule >= tempoComp) {
        enterState(HeatingState::HEATING);
    }
}

// ---------------- HEATING ----------------
void Chauffage::actionHeating(double tempUExt, double tempEExt, double tempUInt, double tempEInt)
{
    if (m_blocageChauffage.load()) {
        m_logger.info("[HEATING] Blocage détecté -> passage en IDLE");
        enterState(HeatingState::IDLE);
        return;
    }

    // Ventilateur extérieur: petite/grande vitesse selon consigne
    double consigneGrandeVitesse = m_consignes->get("ConsigneGrandeVitesseExterieurChauffage");
    if (tempUExt < consigneGrandeVitesse) setVentExt(true, true);
    else setVentExt(true, false);

    // Démarrage compresseur si besoin (on respecte tempoComp déjà réglé par PREHEAT)
    if (!m_compresseurEnMarche.load()) {
        m_compresseur.on();
        m_compresseurEnMarche.store(true);
        m_departCompresseur = steady_clock::now();
        m_logger.info("[COMPRESSEUR] ON");
    }

    // Détection dégivrage
    auto now = steady_clock::now();
    int tempoDepartDegivrage = static_cast<int>(m_tempos->get("tempoDepartDegCh"));
    int dureeComp = static_cast<int>(duration_cast<seconds>(now - m_departCompresseur).count());

    bool conditionTemps = (dureeComp >= tempoDepartDegivrage);
    bool conditionTemp = (tempUExt >= 5.0 && tempEExt < -3.0) ||
                         (tempEExt < (0.4 * tempUExt - 5.0));

    if (!m_degivrageEnMarche.load() && !m_egouttageEnMarche.load() && conditionTemps && conditionTemp) {
        m_logger.info("[DÉGIVRAGE] Condition atteinte -> entrée DEGIVRAGE");
        enterState(HeatingState::DEGIVRAGE);
        return;
    }

    // Ventilateur intérieur : démarrage si compresseur en marche et TEInt > consigne
    double consigneDepartVentInt = m_consignes->get("ConsigneDepartVentilateurInterieurChauffage");
    double consignePetiteVitesse = m_consignes->get("ConsignePetiteVitesseInterieurChauffage");
    double hystVentInt = 0.2;

    if (!m_ventIntEnMarche.load() && m_compresseurEnMarche.load() && tempEInt > consigneDepartVentInt) {
        setVentInt(true, false);
        m_logger.info("[VENT INT] ON");
    }

    if (m_ventIntEnMarche.load()) {
        if (tempUInt <= consignePetiteVitesse - hystVentInt) setVentInt(true, true);
        else if (tempUInt >= consignePetiteVitesse + hystVentInt) setVentInt(true, false);
    }

    // Vanne 4 voies off en chauffage classique
    if (m_vanne4V.isOn()) {
        m_vanne4V.off();
        m_logger.info("[V4V] OFF (heating)");
    }
}

// ---------------- DEGIVRAGE ----------------
void Chauffage::actionDegivrage()
{
    // Pendant dégivrage : compresseur arrêté, ventilateurs off, V4V ON
    setVentExt(false, false);
    setVentInt(false, false);
    if (m_compresseurEnMarche.load()) {
        m_compresseur.off();
        m_compresseurEnMarche.store(false);
    }
    if (!m_vanne4V.isOn()) {
        m_vanne4V.on();
        m_logger.info("[V4V] ON (degivrage)");
    }

    if (m_debutDegivrage.time_since_epoch().count() == 0)
        m_debutDegivrage = steady_clock::now();

    auto now = steady_clock::now();
    int tempoFinDegivrage = static_cast<int>(m_tempos->get("tempoFinDegCh"));
    int ecoule = static_cast<int>(duration_cast<seconds>(now - m_debutDegivrage).count());

    m_logger.info("[DEGIVRAGE] " + std::to_string(ecoule) + "/" + std::to_string(tempoFinDegivrage));

    if (ecoule >= tempoFinDegivrage) {
        m_degivrageEnMarche.store(false);
        m_egouttageEnMarche.store(true);
        m_debutEgouttage = now;
        enterState(HeatingState::EGOUTTAGE);
    }
}

// ---------------- EGOUTTAGE ----------------
void Chauffage::actionEgouttage()
{
    setVentExt(true, true);   // grande vitesse forcée pour égouttage
    setVentInt(false, false);
    if (m_compresseurEnMarche.load()) {
        m_compresseur.off();
        m_compresseurEnMarche.store(false);
    }
    if (m_vanne4V.isOn()) {
        m_vanne4V.off();
        m_logger.info("[V4V] OFF (egouttage)");
    }

    if (m_debutEgouttage.time_since_epoch().count() == 0)
        m_debutEgouttage = steady_clock::now();

    auto now = steady_clock::now();
    int tempoEgouttage = static_cast<int>(m_tempos->get("tempoEgouttage"));
    int ecoule = static_cast<int>(duration_cast<seconds>(now - m_debutEgouttage).count());

    m_logger.info("[EGOUTTAGE] " + std::to_string(ecoule) + "/" + std::to_string(tempoEgouttage));

    if (ecoule >= tempoEgouttage) {
        m_egouttageEnMarche.store(false);
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
            m_logger.info("[STATE] → PREHEAT");
            break;
        case HeatingState::HEATING:
            m_logger.info("[STATE] → HEATING");
            break;
        case HeatingState::DEGIVRAGE:
            m_debutDegivrage = now;
            m_degivrageEnMarche.store(true);
            m_compresseurEnMarche.store(false);
            m_logger.info("[STATE] → DEGIVRAGE");
            break;
        case HeatingState::EGOUTTAGE:
            m_debutEgouttage = now;
            m_egouttageEnMarche.store(true);
            m_logger.info("[STATE] → EGOUTTAGE");
            break;
        default:
            m_logger.info("[STATE] → IDLE");
            break;
    }
}

void Chauffage::setVentExt(bool on, bool grandeVitesse)
{
    if (on) m_ventExt.on(); else m_ventExt.off();
    if (grandeVitesse) m_vitVentExt.on(); else m_vitVentExt.off();

    m_ventExtEnMarche.store(on);
    m_vitesseVentExtEnMarche.store(grandeVitesse);
}

void Chauffage::setVentInt(bool on, bool grandeVitesse)
{
    if (on) m_ventInt.on(); else m_ventInt.off();
    if (grandeVitesse) m_vitVentInt.on(); else m_vitVentInt.off();

    m_ventIntEnMarche.store(on);
    m_vitesseVentIntEnMarche.store(grandeVitesse);
}

// ---------------- getters ----------------
bool Chauffage::isVentExtEnMarche() const { return m_ventExtEnMarche.load(); }
bool Chauffage::isVitesseVentExtEnMarche() const { return m_vitesseVentExtEnMarche.load(); }
bool Chauffage::isVentIntEnMarche() const { return m_ventIntEnMarche.load(); }
bool Chauffage::isVitesseVentIntEnMarche() const { return m_vitesseVentIntEnMarche.load(); }
bool Chauffage::isCompresseurEnMarche() const { return m_compresseurEnMarche.load(); }
bool Chauffage::isDegivrageEnMarche() const { return m_degivrageEnMarche.load(); }
bool Chauffage::isEgouttageEnMarche() const { return m_egouttageEnMarche.load(); }
