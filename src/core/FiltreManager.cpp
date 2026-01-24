#include "FiltreManager.hpp"
#include "utils/Log.hpp"

FiltreManager::FiltreManager(Temporisations& tempos)
    : m_tempos(tempos)
{
}

void FiltreManager::update(double dt, bool ventiloIntActif, bool pauseGlobale, bool filtreOpen)
{
    double runTime = m_tempos.get("dureeExecutionFiltre");
    double seuilInfo = m_tempos.get("tempoNettoyageFiltre");
    double seuilStop = m_tempos.get("tempoArretNettoyageFiltre");

    // Comptage
    if (ventiloIntActif && !filtreOpen && !pauseGlobale) {
        runTime += dt;
        m_tempos.set("dureeExecutionFiltre", runTime);
    }

    // Filtre ouvert
    if (filtreOpen && !m_prevFilterOpen) {
        if (!m_prevFilterOpen) {
            LOG_INFO("[FILTREMANAGER] Nettoyage filtre en cours");
        }
    }

    // Fin nettoyage
    if (!filtreOpen && m_prevFilterOpen) {
        LOG_INFO("[FILTREMANAGER] Nettoyage filtre terminé");
        reset();
        runTime = 0;
    }

    // Info nettoyage
    if (!filtreOpen && runTime >= seuilInfo && runTime < seuilStop && !m_filtreInfoLogged) {
        LOG_INFO("[FILTREMANAGER] Nettoyage filtre à faire");
        m_filtreInfoLogged = true;
    }

    // Arrêt absolu
    if (!pauseGlobale && !filtreOpen && runTime >= seuilStop && !m_filtreStopLogged) {
        LOG_WARN("[FILTREMANAGER] Nettoyage filtre arrêt absolu");
        m_stopFiltre = true;
        m_filtreStopLogged = true;
    }

    m_prevFilterOpen = filtreOpen;
}

void FiltreManager::reset()
{
    m_tempos.set("dureeExecutionFiltre", 0);
    m_filtreInfoLogged = false;
    m_filtreStopLogged = false;
    m_stopFiltre = false;
}

bool FiltreManager::nettoyageEnCours() const
{
    return m_prevFilterOpen && !m_stopFiltre;
}

bool FiltreManager::alerteNettoyage() const
{
    return m_filtreInfoLogged && !m_stopFiltre && !m_prevFilterOpen;
}

bool FiltreManager::arretAbsolu() const
{
    return m_stopFiltre && !m_prevFilterOpen;
}
