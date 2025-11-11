#include "PacController.hpp"
#include <sstream>
#include <cmath>
#include <QDebug>

using namespace std::chrono_literals;

PacController::PacController(Logger& logger, DS18B20& sondes, Consignes& consignes, Temporisations& temporisations)
    : m_logger(logger),
      m_sondes(sondes),
      m_consignes(consignes),
      m_temporisations(temporisations),
      m_chauffage(logger, &m_consignes, &m_temporisations),
      m_froid(logger, &m_consignes, &m_temporisations),
      m_thermostat(17),
      m_relayEteHiver(26),
      m_stopFlag(false),
      m_pauseFlag(false),
      m_modeActuel(MODE_ARRET),
      m_voletBloque(false),
      m_securite(false),
      m_enAttenteVolets(false)
{
    m_thread = std::thread(&PacController::loop, this);
    m_logger.info("[PACController] démarré.");
}

PacController::~PacController() {
    stopPac();
}

void PacController::stopPac() {
    m_stopFlag.store(true);
    if (m_thread.joinable()) m_thread.join();
    stopAll();
    m_logger.info("[PACController] PAC arrêt complet");
}

void PacController::stopAll() {
    m_chauffage.stop();
    m_froid.stop();
    m_relayEteHiver.off();
    m_logger.info("[PACController] Tous les relais arrêtés");
}

void PacController::pausePac() {
    m_pauseFlag.store(true);
    pauseAll();
    m_logger.info("[PACController] PAC mise en pause");
}

void PacController::pauseAll() {
    m_chauffage.stop();
    m_froid.stop();
    m_logger.info("[PACController] Chauffage/Froid arrêtés (ETE/HIVER maintenu)");
}

void PacController::resumePac() {
    m_pauseFlag.store(false);
    m_logger.info("[PACController] PAC reprise");
}

double PacController::getTemperature(const std::string& name) const {
    return m_sondes.getTemperature(name);
}

ModePAC PacController::determineMode(double tempExt) const {
    double consEteHiver = m_consignes.get("ConsigneEteHiver");
    double consCanicule = m_consignes.get("ConsigneCanicule");

    if (tempExt < consEteHiver) return MODE_CHAUFFAGE;
    if (tempExt < consCanicule) return MODE_FROID;
    return MODE_ARRET;
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
    auto lastVoletLog = clk::now();

    // tempo volets initiale
    m_enAttenteVolets = true;
    m_debutTempoVolets = clk::now();
    int dernierTempsRestant = -1;

    while (!m_stopFlag.load()) {
        // courte attente pour réactivité
        if (!m_sondes.isReady()) {
            std::this_thread::sleep_for(1s);
            continue;
        }

        // mise à jour des consignes et temporisations
        m_consignes.update();
        m_temporisations.update();

        bool thermostatActif = m_thermostat.isActive();
        m_sondes.setEtatPAC(thermostatActif);

        // sécurité sondes
        if (m_sondes.hasCriticalErrors()) {
            if (!m_securite) {
                m_logger.error("[SECURITE] Panne de sonde critique détectée — arrêt immédiat de la PAC !");
                stopAll();
                m_securite = true;
            }
            std::this_thread::sleep_for(1s);
            continue;
        } else if (m_securite) {
            m_logger.info("[SECURITE] Toutes les sondes OK — relance automatique de la PAC");
            m_securite = false;
        }

        // lecture températures
        float tempExt  = m_sondes.getTemperature("Ext");
        float tempUExt = m_sondes.getTemperature("UExt");
        float tempEExt = m_sondes.getTemperature("EExt");
        float tempUInt = m_sondes.getTemperature("UInt");
        float tempEInt = m_sondes.getTemperature("EInt");

        // gestion relais été/hiver
        double consEteHiver = m_consignes.get("ConsigneEteHiver");
        if (!std::isnan(tempExt) && tempExt < consEteHiver)
            m_relayEteHiver.on();
        else
            m_relayEteHiver.off();

        // détermination du mode PAC
        //ModePAC nouveauMode = m_pauseFlag.load() ? MODE_ARRET : determineMode(tempExt);
        ModePAC nouveauMode = determineMode(tempExt); // mode réel pour la comparaison
        
        ModePAC modeEffectif = m_pauseFlag.load() ? MODE_ARRET : nouveauMode;

        // changement de mode -> reset tempo volets
        if (nouveauMode != m_modeActuel) {
            m_logger.info("[PACController] Changement de mode : " + modeToString(m_modeActuel) +
                          " -> " + modeToString(nouveauMode));
            m_chauffage.stop();
            m_froid.stop();
            m_debutTempoVolets = clk::now();
            m_enAttenteVolets = true;
            m_modeActuel = nouveauMode;
            dernierTempsRestant = -1; // reset affichage tempo
        }

        // --- tempo volets non bloquante ---
        if (m_enAttenteVolets) {
            int tempoVolets = static_cast<int>(m_temporisations.get("tempoVolets"));
            int ecoule = static_cast<int>(std::chrono::duration_cast<std::chrono::seconds>(clk::now() - m_debutTempoVolets).count());
            int restant = tempoVolets - ecoule;

            if (restant != dernierTempsRestant && restant >= 0 &&
                std::chrono::duration_cast<std::chrono::seconds>(clk::now() - lastVoletLog).count() >= 1) {
                m_logger.info("[PACController] Attente volets : " + std::to_string(restant) + "s restantes");
                dernierTempsRestant = restant;
                lastVoletLog = clk::now();
            }

            if (restant <= 0) {
                m_enAttenteVolets = false;
                m_logger.info("[PACController] Fin attente volets — reprise normale");
            }
        }

        // --- fonctionnement normal ---
        if (!m_pauseFlag.load() && thermostatActif && !m_enAttenteVolets) {
            switch (modeEffectif) {//m_modeActuel) {
                case MODE_CHAUFFAGE:
                    m_chauffage.update(tempUExt, tempEExt, tempUInt, tempEInt);
                    break;
                case MODE_FROID:
                    m_froid.update(tempUExt, tempUInt, tempEInt);
                    break;
                case MODE_ARRET:
                default:
                    m_chauffage.stop();
                    m_froid.stop();
                    break;
            }
        } else if (!m_enAttenteVolets) {
            // pause ou thermostat inactif : stop chauffage/froid
            pauseAll();
        }

        // log périodique toutes les 1s
        auto now = clk::now();
        if (std::chrono::duration_cast<std::chrono::seconds>(now - lastLog).count() >= 1) {
            auto fmtTemp = [](float t) -> std::string {
                if (std::isnan(t)) return "N/A";
                std::ostringstream ss;
                ss << std::fixed << std::setprecision(2) << t << "°C";
                return ss.str();
            };
            std::ostringstream ss;
            ss << "[PACController] Mode=" << modeToString(m_modeActuel) << "\n";
            ss << " Ext=" << fmtTemp(tempExt)
               << " UExt=" << fmtTemp(tempUExt)
               << " EExt=" << fmtTemp(tempEExt)
               << " UInt=" << fmtTemp(tempUInt)
               << " EInt=" << fmtTemp(tempEInt);
               
            m_logger.info(ss.str());
            lastLog = now;
        }

        // micro-pause réactive
        std::this_thread::sleep_for(1s);
    }

    stopAll();
    m_logger.info("[PACController] Fin du thread de contrôle.");
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
