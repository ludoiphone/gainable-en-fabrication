#include "PacController.hpp"
#include <sstream>
#include <cmath>
#include <QDebug>

using namespace std::chrono_literals;

PacController::PacController(NTCReader& sondes, Consignes& consignes, Temporisations& temporisations)
    : m_sondes(sondes),
      m_consignes(consignes),
      m_temporisations(temporisations),
      m_chauffage(&m_consignes, &m_temporisations),
      m_froid(&m_consignes, &m_temporisations),
      m_thermostat(22),
      m_relayEteHiver(26),
      m_stopFlag(false),
      m_pauseFlag(false),
      m_stopped(false),
      m_modeActuel(MODE_ARRET),
      m_voletBloque(false),
      m_securite(false),
      m_enAttenteVolets(false)
{
    m_thread = std::thread(&PacController::loop, this);
}

PacController::~PacController() {
    stopPac();
}

void PacController::stopPac() {
    m_stopFlag.store(true);
    if (m_thread.joinable()) m_thread.join();
    stopAll();
}

void PacController::stopAll() {
    // idempotent : ne logue qu'une seule fois
    bool expected = false;
    if (!m_stopped.compare_exchange_strong(expected, true)) {
        // déjà arrêté
        return;
    }

    m_chauffage.stop();
    m_froid.stop();
    m_relayEteHiver.off();
}

void PacController::pausePac() {
    m_pauseFlag.store(true);
    pauseAll();
}

void PacController::pauseAll() {
    m_chauffage.stop();
    m_froid.stop();
}

void PacController::resumePac() {
    m_pauseFlag.store(false);
}

double PacController::getTemperature(const std::string& name) const {
    return m_sondes.getTemperature(name);
}

ModePAC PacController::determineMode(double tempExt) const {
    double consEteHiver = m_consignes.get("ConsigneEteHiver");
    double consCanicule = m_consignes.get("ConsigneCanicule");
    double hystEteHiver = 0.2;

    if (tempExt < consEteHiver - hystEteHiver) return MODE_CHAUFFAGE;
    else if (tempExt < consCanicule - hystEteHiver) return MODE_FROID;
    else if (tempExt > consCanicule + hystEteHiver) return MODE_CANICULE;
    else return MODE_ARRET;
}

std::string PacController::modeToString(ModePAC m) const {
    switch (m) {
        case MODE_CHAUFFAGE: return "MODE_CHAUFFAGE";
        case MODE_FROID:     return "MODE_FROID";
        default:             return "MODE_ARRET";
    }
}

void PacController::loop() {
    using clk = std::chrono::steady_clock;
    auto lastLog = clk::now();

    const auto loopSleep = 1s;

    while (!m_stopFlag.load()) {
        m_consignes.update();
        m_temporisations.update();

        // Lecture des températures
        double tExt  = m_sondes.getTemperature("Ext");
        double tUExt = m_sondes.getTemperature("UExt");
        double tEExt = m_sondes.getTemperature("EExt");
        double tUInt = m_sondes.getTemperature("UInt");
        double tEInt = m_sondes.getTemperature("EInt");

        // Vérification de toutes les sondes
        if (std::isnan(tExt) || std::isnan(tUExt) || std::isnan(tEExt) ||
            std::isnan(tUInt) || std::isnan(tEInt)) 
        {
            if (!m_securite) {
                m_securite = true;
                stopAll(); // arrêt immédiat
            }
            std::this_thread::sleep_for(loopSleep);
            continue; // passe à l'itération suivante
        } 
        else if (m_securite) {
            m_securite = false;
            m_stopped.store(false);
        }

        bool thermostatActif = m_thermostat.isActive();

        // Gestion du relais Été/Hiver
        if (tExt < m_consignes.get("ConsigneEteHiver"))
            m_relayEteHiver.on();
        else
            m_relayEteHiver.off();

        // Détermination du mode PAC
        ModePAC nouveauMode = determineMode(tExt);
        ModePAC modeEffectif = m_pauseFlag.load() ? MODE_ARRET : nouveauMode;

        if (nouveauMode != m_modeActuel) {
            m_chauffage.stop();
            m_froid.stop();
            m_modeActuel = nouveauMode;
            m_enAttenteVolets = true;
            m_debutTempoVolets = clk::now();
        }

        if (m_enAttenteVolets) {
            int tempo = (int)m_temporisations.get("tempoVolets");
            int ecoule = (int)std::chrono::duration_cast<std::chrono::seconds>(clk::now() - m_debutTempoVolets).count();
            if (ecoule >= tempo) m_enAttenteVolets = false;
        }
        
        double delta = m_consignes.get("ConsigneDelta");
        double consigneDelta = tExt - delta;

        if (!m_pauseFlag.load() && thermostatActif && !m_enAttenteVolets && !m_securite) {
            switch (modeEffectif) {
                case MODE_CHAUFFAGE:
                    m_chauffage.update(tUExt, tEExt, tUInt, tEInt);
                    break;
                case MODE_FROID:
                    m_froid.update(tUExt, tUInt, tEInt);
                    break;
                case MODE_CANICULE:
                    if (tUInt > consigneDelta) m_froid.update(tUExt, tUInt, tEInt);
                    else m_froid.stop();
                    break;
                default:
                    m_chauffage.stop();
                    m_froid.stop();
                    break;
            }
        } else if (!m_enAttenteVolets) {
            pauseAll();
        }

        // Logging périodique
        auto now = clk::now();
        if (std::chrono::duration_cast<std::chrono::seconds>(now - lastLog).count() >= 1) {
            std::ostringstream pac_ss;
            pac_ss << "[PACController] Mode=" << modeToString(m_modeActuel);

            std::ostringstream ntc_ss;
            ntc_ss << std::fixed << std::setprecision(2);
            ntc_ss << "[PACController] "
                   << "Ext=" << tExt << " "
                   << "UExt=" << tUExt << " "
                   << "EExt=" << tEExt << "\n"
                   << "UInt=" << tUInt << " "
                   << "EInt=" << tEInt;

            lastLog = now;
        }

        std::this_thread::sleep_for(loopSleep);
    }

    stopAll();
}

ModePAC PacController::getModeActuel() const
{
    return m_modeActuel;
}

bool PacController::getVentilateurExt() const
{
        // Si la PAC est à l'arrêt ou en pause => ventilateur off
    if (m_stopFlag.load() || m_pauseFlag.load())
        return false;

    // Si le thermostat n’est pas actif => ventilateur off
    if (!m_thermostat.isActive())
        return false;

    // Si on est en attente de volets => ventilateur off
    if (m_enAttenteVolets)
        return false;

    // Sécurité active => ventilateur off
    if (m_securite)
        return false;

    // Sinon on délègue à l’état du mode en cours
    switch (m_modeActuel) {
        case MODE_CHAUFFAGE:
            return m_chauffage.isVentExtEnMarche();
        case MODE_FROID:
            return m_froid.isVentExtEnMarche();
        case MODE_CANICULE:
            return m_froid.isVentExtEnMarche();
        default:
            return false;
    }
}

int PacController::getVitesseVentilateurExt() const
{
    if (!getVentilateurExt())
        return 0;

    switch (m_modeActuel) {
        case MODE_CHAUFFAGE:
            return m_chauffage.isVitesseVentExtEnMarche() ? 2 : 1;
        case MODE_FROID:
            return m_froid.isVitesseVentExtEnMarche() ? 2 : 1;
        case MODE_CANICULE:
            return m_froid.isVitesseVentExtEnMarche() ? 2 : 1;
        default:
            return 0;
    }
}

bool PacController::getVentilateurInt() const
{
    if (m_stopFlag.load() || m_pauseFlag.load())
        return false;
    if (!m_thermostat.isActive())
        return false;
    if (m_enAttenteVolets || m_securite)
        return false;

    switch (m_modeActuel) {
        case MODE_CHAUFFAGE:
            return m_chauffage.isVentIntEnMarche();
        case MODE_FROID:
            return m_froid.isVentIntEnMarche();
        case MODE_CANICULE:
            return m_froid.isVentIntEnMarche();
        default:
            return false;
    }
}

int PacController::getVitesseVentilateurInt() const
{
    if (!getVentilateurInt())
        return 0;

    switch (m_modeActuel) {
        case MODE_CHAUFFAGE:
            return m_chauffage.isVitesseVentIntEnMarche() ? 4 : 1;
        case MODE_FROID:
            return m_froid.isVitesseVentIntEnMarche() ? 4 : 1;
        case MODE_CANICULE:
            return m_froid.isVitesseVentIntEnMarche() ? 4 : 1;
        default:
            return 0;
    }
}

bool PacController::getCompresseur() const
{
    if (m_stopFlag.load() || m_pauseFlag.load())
        return false;
    if (!m_thermostat.isActive())
        return false;
    if (m_enAttenteVolets || m_securite)
        return false;

    switch (m_modeActuel) {
        case MODE_CHAUFFAGE:
            return m_chauffage.isCompresseurEnMarche();
        case MODE_FROID:
            return m_froid.isCompresseurEnMarche();
        case MODE_CANICULE:
            return m_froid.isCompresseurEnMarche();
        default:
            return false;
    }
}

bool PacController::getCanicule() const
{
    switch (m_modeActuel) {
        case MODE_CANICULE:
            return true;
        default:
            return false;
    }
}

bool PacController::getDegivrage() const
{
    switch (m_modeActuel) {
        case MODE_CHAUFFAGE:
            return m_chauffage.isDegivrageEnMarche();
        case MODE_FROID:
            return m_froid.isDegivrageEnMarche();
        default:
            return false;
    }
}

bool PacController::getEgouttage() const
{
    switch (m_modeActuel) {
        case MODE_CHAUFFAGE:
            return m_chauffage.isEgouttageEnMarche();
        case MODE_FROID:
            return m_froid.isEgouttageEnMarche();
        default:
            return false;
    }
}
