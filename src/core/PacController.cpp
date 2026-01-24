#include "PacController.hpp"
#include "utils/Log.hpp"

using namespace std::chrono;
using namespace std::chrono_literals;

// ======================================
// UTILITAIRES
// ======================================
static std::string modeToString(ModePAC mode)
{
    switch (mode)
    {
        case MODE_ARRET: return "ARRET";
        case MODE_CHAUFFAGE: return "CHAUFFAGE";
        case MODE_FROID: return "FROID";
        case MODE_CANICULE: return "CANICULE";
        default: return "???";
    }
}

// ======================================
// CONSTRUCTEUR / DESTRUCTEUR
// ======================================
PacController::PacController(NTCReader& sondes,
                             RelayManager& relays,
                             SwitchManager& switchs,
                             Consignes& consignes,
                             Temporisations& temporisations)
    : m_sondes(sondes)
    , m_ntcManager(sondes)
    , m_switch(switchs)
    , m_relay(relays)
    , m_consignes(consignes)
    , m_tempos(temporisations)
    , m_chauffage(relays, &m_consignes, &m_tempos)
    , m_froid(relays, &m_consignes, &m_tempos)
    , m_filtreManager(m_tempos)
    , m_state(PacState::INIT_SONDES)
    , m_modeActuel(MODE_ARRET)
    , m_stopAdmin(false)
    , m_userPause(true)
    , m_ntcInitDone(false)
    , m_stabilisationNtc(true)
    , m_lastNtcFault(false)
    , m_lastUserRun(false)
    , m_lastThermostatRequest(false)
{
    m_stateStart = steady_clock::now();
    m_statePrev = static_cast<PacState>(-1);
    m_lastTime = m_stateStart;
    m_thread = std::thread(&PacController::loop, this);
    m_lastUserRun = m_userPause.load();
    m_lastThermostatRequest = m_switch.thermostatClosed();
}

PacController::~PacController()
{
    m_stopAdmin.store(true);
    if (m_thread.joinable())
        m_thread.join();
    stopPac();
}

// ======================================
// CONTRÔLE PAC
// ======================================
void PacController::pausePac()
{
    m_chauffage.stop();
    m_froid.stop();
}

void PacController::pauseUserPac()
{
    m_userPause.store(false);
}

void PacController::restartUserPac()
{
    m_userPause.store(true);
}

void PacController::stopPac()
{
    m_stopAdmin.store(true);
    m_relay.setEteHiver(false, false);
    m_chauffage.stop();
    m_froid.stop();
}

double PacController::getTemperature(const std::string& name) const
{
    return m_ntcManager.getTemperature(name);
}

// ======================================
// SERVICE TEST
// ======================================
void PacController::serviceEnable(bool enable)
{
    if (enable) {
        LOG_WARN("[PACCONTROLLER] Entée mode SERVICE");
        m_serviceTest.store(true);
        m_chauffage.stop();
        m_froid.stop();
        m_relay.allOff();
        enterState(PacState::SERVICE);
    } else {
        LOG_INFO("[PACCONTROLLER] Sortie mode SERVICE");
        m_serviceTest.store(false);
        m_chauffage.stop();
        m_froid.stop();
        m_relay.allOff();
        enterState(PacState::ATTENTE_VOLET);//:PAUSE);
    }
}

// ======================================
// RELAIS SAISON
// ======================================
void PacController::applySeasonRelay(ModePAC mode)
{
    if (mode == MODE_CHAUFFAGE)
        m_relay.setEteHiver(true, false);
    else if (mode == MODE_FROID || mode == MODE_CANICULE)
        m_relay.setEteHiver(false, false);
}

// ======================================
// TRANSITION VOLETS
// ======================================
bool PacController::needVoletTransition(ModePAC modeActuel, ModePAC modeVoulu) const
{
    if (modeActuel == modeVoulu)
        return false;

    // Transition réelle seulement si changement de "monde"
    return (modeActuel == MODE_CHAUFFAGE) != (modeVoulu == MODE_CHAUFFAGE);
}

// ======================================
// CHANGEMENT D'ÉTAT
// ======================================
void PacController::enterState(PacState newState)
{
    if (newState == m_state)
        return;

    m_statePrev = m_state;
    m_state = newState;
    m_stateStart = steady_clock::now();
}

// ======================================
// DÉTERMINATION MODE
// ======================================
ModePAC PacController::determineMode() const
{
    double tExt = m_ntcManager.getTemperature("Ext");
    double consEteHiver = m_consignes.get("ConsigneEteHiver");
    double consCanicule = m_consignes.get("ConsigneCanicule");
    double hystEteHiver = 0.3;

    if (tExt < consEteHiver - hystEteHiver)
        return MODE_CHAUFFAGE;
    else if (tExt < consCanicule - hystEteHiver)
        return MODE_FROID;
    else if (tExt > consCanicule + hystEteHiver)
        return MODE_CANICULE;
    else
        return MODE_ARRET;
}

// ======================================
// LOOP PRINCIPAL
// ======================================
void PacController::loop()
{
    while (!m_stopAdmin.load())
    {
        update();
        std::this_thread::sleep_for(500ms);
    }
}

// ======================================
// UPDATE
// ======================================
void PacController::update()
{
    auto now = steady_clock::now();
    double dt = duration<double>(now - m_lastTime).count();
    m_lastTime = now;

    m_consignes.update();
    m_tempos.update();
    m_ntcManager.update();

    m_filtreManager.update(dt,
                           m_relay.ventInt.isOn(),
                           m_state != PacState::MARCHE,
                           m_switch.filtreOpen());

    // --------- SERVICE TEST ----------
    if (m_serviceTest.load())
    {
        return;
    }

    // ---------- SECURITE NTC ----------
    bool ntcFault = m_ntcManager.hasFault();
    if (!m_lastNtcFault && ntcFault)
    {
        LOG_ERROR("[PACCONTROLLER] Défaut sonde NTC détecté → SECURITE");
        pausePac();
        enterState(PacState::SECURITE);
    }

    if (m_lastNtcFault && !ntcFault)
    {
        LOG_INFO("[PACCONTROLLER] ✅ Défaut NTC levé");
        enterState(PacState::PAUSE);
    }

    m_lastNtcFault = ntcFault;

    // ---------- ENTRETIEN FILTRE ----------
    if (m_filtreManager.nettoyageEnCours())
    {
        pausePac();
        if (m_state != PacState::PAUSE)
            enterState(PacState::PAUSE);
        return;
    }

    if (m_filtreManager.arretAbsolu())
    {
        pausePac();
        if (m_state != PacState::SECURITE)
            enterState(PacState::SECURITE);
        return;
    }

    // ---------- LOG EVENEMENTS UTILISATEUR ----------
    bool userRun = m_userPause.load();
    if (!m_lastUserRun && userRun)
        LOG_INFO("[PACCONTROLLER] Marche utilisateur");
    else if (m_lastUserRun && !userRun)
        LOG_INFO("[PACCONTROLLER] Arrêt utilisateur");

    m_lastUserRun = userRun;

    // ---------- LOG EVENEMENTS THERMOSTAT ----------
    bool thermostatRequest = m_switch.thermostatClosed();
    if (!m_lastThermostatRequest && thermostatRequest)
        LOG_INFO("[PACCONTROLLER] Demande thermostat");
    else if (m_lastThermostatRequest && !thermostatRequest)
        LOG_INFO("[PACCONTROLLER] Fin demande thermostat");

    m_lastThermostatRequest = thermostatRequest;

    // ---------- FSM ----------
    handleState();
}

// ======================================
// FSM
// ======================================
void PacController::handleState()
{
    switch (m_state)
    {
        case PacState::INIT_SONDES: actionInitSondes(); break;
        case PacState::ATTENTE_VOLET: actionAttentVolet(); break;
        case PacState::MARCHE: actionMarche(); break;
        case PacState::PAUSE: actionPause(); break;
        case PacState::SECURITE: actionSecurite(); break;
        case PacState::SERVICE: actionService(); break;
        default: break;
    }

    if (m_state != m_statePrev)
    {
        auto stateToString = [](PacState s) -> std::string
        {
            switch (s)
            {
                case PacState::INIT_SONDES: return "INIT_SONDES";
                case PacState::ATTENTE_VOLET: return "ATTENTE_VOLET";
                case PacState::MARCHE: return "MARCHE";
                case PacState::PAUSE: return "PAUSE";
                case PacState::SECURITE: return "SECURITE";
                case PacState::SERVICE: return "SERVICE";
                default: return "???";
            }
        };

        LOG_INFO("[PACCONTROLLER] Etat -> " + stateToString(m_state));

        if (m_state == PacState::ATTENTE_VOLET)
            LOG_INFO("[PACCONTROLLER] Initialisation volets en cours");

        m_statePrev = m_state;
    }
}

// ======================================
// ACTIONS
// ======================================
void PacController::actionInitSondes()
{
    auto elapsed = duration_cast<seconds>(steady_clock::now() - m_stateStart).count();

    if (m_stabilisationNtc && elapsed >= 1 && !m_ntcInitDone)
    {
        LOG_INFO("[PACCONTROLLER] Stabilisation sondes en cours");
        m_stabilisationNtc = false;
    }

    if (!m_ntcInitDone && elapsed >= 20)
    {
        LOG_INFO("[PACCONTROLLER] Stabilisation sondes OK");
        m_modeActuel = determineMode();

        LOG_INFO("[PACCONTROLLER] Mode actuel: -> " +
                 std::string((m_modeActuel == MODE_ARRET) ? "ARRET" :
                             (m_modeActuel == MODE_CHAUFFAGE) ? "CHAUFFAGE" :
                             (m_modeActuel == MODE_FROID) ? "FROID" :
                             (m_modeActuel == MODE_CANICULE) ? "CANICULE" : "???"));

        applySeasonRelay(m_modeActuel);
        m_ntcInitDone = true;
        enterState(PacState::ATTENTE_VOLET);
    }
}

void PacController::actionAttentVolet()
{
    int tempoVolet = (int)m_tempos.get("tempoVolets");
    if (duration_cast<seconds>(steady_clock::now() - m_stateStart).count() < tempoVolet)
        return;

    LOG_INFO("[PACCONTROLLER] Initialisation volets OK");

    if (m_userPause.load() && m_switch.thermostatClosed())
        enterState(PacState::MARCHE);
    else
        enterState(PacState::PAUSE);
}

void PacController::actionMarche()
{
    if (m_ntcManager.hasFault())
    {
        LOG_ERROR("[PACCONTROLLER] Défaut NTC en MARCHE → SECURITE");
        pausePac();
        enterState(PacState::SECURITE);
        return;
    }

    if (!m_userPause.load() || !m_switch.thermostatClosed())
    {
        pausePac();
        enterState(PacState::PAUSE);
        return;
    }

    ModePAC newMode = determineMode();
    if (newMode != m_modeActuel)
    {
        LOG_INFO("[PACCONTROLLER] Changement de mode detécté : -> " + modeToString(newMode));
        bool transition = needVoletTransition(m_modeActuel, newMode);
        pausePac();
        m_modeActuel = newMode;
        applySeasonRelay(m_modeActuel);
        if (transition)
        {
            enterState(PacState::ATTENTE_VOLET);
            return;
        }
    }

    double tUExt = m_ntcManager.getTemperature("UExt");
    double tEExt = m_ntcManager.getTemperature("EExt");
    double tUInt = m_ntcManager.getTemperature("UInt");
    double tEInt = m_ntcManager.getTemperature("EInt");
    double tExt = m_ntcManager.getTemperature("Ext");
    double consigneDelta = tExt - m_consignes.get("ConsigneDelta");

    switch (m_modeActuel)
    {
        case MODE_CHAUFFAGE:
            m_chauffage.update(tUExt, tEExt, tUInt, tEInt);
            break;
        case MODE_FROID:
            m_froid.update(tUExt, tUInt, tEInt);
            break;
        case MODE_CANICULE:
            if (tUInt > consigneDelta)
                m_froid.update(tUExt, tUInt, tEInt);
            else
                m_froid.stop();
            break;
        default:
            pausePac();
            break;
    }
}

void PacController::actionPause()
{
    ModePAC newMode = determineMode();
    if (newMode != m_modeActuel)
    {
        LOG_INFO("[PACCONTROLLER] Mode voulu: -> " + modeToString(newMode));
        bool transition = needVoletTransition(m_modeActuel, newMode);
        m_modeActuel = newMode;
        applySeasonRelay(m_modeActuel);
        if (transition)
        {
            LOG_INFO("[PACCONTROLLER] Changement de mode -> ATTENTE_VOLET");
            enterState(PacState::ATTENTE_VOLET);
            return;
        }
    }

    if (!m_ntcManager.hasFault() && m_userPause.load() && m_switch.thermostatClosed())
        enterState(PacState::MARCHE);
}

void PacController::actionSecurite()
{
    pausePac();
    if (m_ntcManager.hasFault())
        enterState(PacState::PAUSE);
}

void PacController::actionService() 
{
    pausePac();
}

// ======================================
// GETTERS UI
// ======================================

PacStatusUI PacController::getStatusUI() const 
{
    PacStatusUI statusUI;
    statusUI.mode = m_modeActuel;
    statusUI.stabilisationActif = (m_state == PacState::INIT_SONDES);
    statusUI.compresseurActif = m_relay.compresseur.isOn();
    statusUI.ventExtActif = m_relay.ventExt.isOn();
    statusUI.ventExtVitesse = m_relay.ventExt.speed();
    statusUI.ventIntActif = m_relay.ventInt.isOn();
    statusUI.ventIntVitesse = m_relay.ventInt.speed();
    statusUI.degivrageActif = m_chauffage.isDefrostActif() || m_froid.isDefrostActif();;
    statusUI.egouttageActif = m_chauffage.isDrainingActif() || m_froid.isDrainingActif();;
    statusUI.filtreANettoyer = m_filtreManager.alerteNettoyage();
    statusUI.filtreNettoyageEnCours = m_filtreManager.nettoyageEnCours();
    statusUI.filtreArretAbsolu = m_filtreManager.arretAbsolu();
    statusUI.transitionSaisons = (m_state == PacState::ATTENTE_VOLET);
    statusUI.canicule = (m_modeActuel == MODE_CANICULE);
    statusUI.ete = (m_modeActuel == MODE_FROID);
    statusUI.hiver = (m_modeActuel == MODE_CHAUFFAGE);
    return statusUI;
}

// ======================================
// SETTER SERVICE
// ======================================

void PacController::serviceSetRelayEteHiver(bool on)
{
    if (!m_serviceTest.load()) return;
    on ? m_relay.setEteHiver(true, true) : m_relay.setEteHiver(false, true);
}

void PacController::serviceSetRelayCompresseur(bool on)
{
    if (!m_serviceTest.load()) return;
    on ? m_relay.setCompresseur(true) : m_relay.setCompresseur(false);
}

void PacController::serviceSetV4V(bool on)
{
    if (!m_serviceTest.load()) return;
    on ? m_relay.setVanne4V(true) : m_relay.setVanne4V(false);
}

void PacController::serviceSetVentilateurExt(VentExtSpeed speed)
{
    if (!m_serviceTest.load()) return;
    m_relay.setVentExt(speed);
}

void PacController::serviceSetVentilateurInt(VentIntSpeed speed)
{
    if (!m_serviceTest.load()) return;
    m_relay.setVentInt(speed);
}

void PacController::serviceAllOff()
{
    m_relay.allOff();
}

// ======================================
// UTILITAIRES SUPPLÉMENTAIRES
// ======================================

bool PacController::thermostatOn() const
{
    return m_switch.thermostatClosed();
}

bool PacController::isFiltreOpen() const
{
    return m_switch.filtreOpen();
}

