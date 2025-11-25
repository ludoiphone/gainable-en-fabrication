#include "Chauffage.hpp"
#include <iostream>
#include <chrono>
#include <thread>

// ================== CONSTRUCTEUR ==================
Chauffage::Chauffage(Logger& logger, Consignes* consignes, Temporisations* tempos)
    : m_logger(logger),
      m_ventExt(13), m_vitVentExt(16), m_compresseur(5),
      m_ventInt(19), m_vitVentInt(20), m_vanne4V(6),
      m_consignes(consignes), m_tempos(tempos),
      m_ventExtEnMarche(false), m_ventIntEnMarche(false), 
      m_compresseurEnMarche(false), m_degivrageEnMarche(false),
      m_egouttageEnMarche(false)
{
    // Initialisation des time_points pour éviter des différences invalides
    auto now = std::chrono::steady_clock::now();
    m_departTempoComp = now;
    m_departCompresseur = now;
    m_debutDegivrage = now;
    m_debutEgouttage = now;
}

// ================== UPDATE PRINCIPAL ==================
void Chauffage::update(double tempUExt, double tempEExt, double tempUInt, double tempEInt) {
    double hystBlocChauf = 0.2;
    bool blocageChauffage = (tempUExt >= m_consignes->get("ConsigneBlocageChauffage") + hystBlocChauf);
    
    gestionVentilateurExterieur(tempUExt, blocageChauffage);
    gestionCompresseur(tempUExt, tempEExt, blocageChauffage);
    gestionVentilateurInterieur(tempUInt, tempEInt, blocageChauffage);
    gestionV4V();
}

// ================== ARRÊT COMPLET ==================
void Chauffage::stop() {
    m_ventExt.off();
    m_vitVentExt.off();
    m_compresseur.off();
    m_ventInt.off();
    m_vitVentInt.off();
    m_vanne4V.off();

    m_ventExtEnMarche.store(false);
    m_vitesseVentExtEnMarche.store(false);
    m_ventIntEnMarche.store(false);
    m_vitesseVentIntEnMarche.store(false);
    
    m_compresseurEnMarche.store(false);
    
    m_degivrageEnMarche.store(false);
    
    m_egouttageEnMarche.store(false);
}

// ================== VENTILATEUR EXTÉRIEUR ==================
void Chauffage::gestionVentilateurExterieur(double tempUExt, bool blocageChauffage) {
    
    // Dégivrage actif → arrêt du ventilo
    if (m_degivrageEnMarche.load()) {
        if (m_ventExtEnMarche.load() || m_vitVentExt.isOn()) {
            m_ventExt.off();
            m_vitVentExt.off();
            m_ventExtEnMarche.store(false);
            m_vitesseVentExtEnMarche.store(false);

            m_logger.info("[VENT EXT] OFF");
        }
        return;
    }
    
    // Égouttage → grande vitesse forcée
    if (m_egouttageEnMarche.load()) {
        m_ventExt.on();
        m_vitVentExt.on();
        m_ventExtEnMarche.store(true);
        m_vitesseVentExtEnMarche.store(true);
        m_logger.info("[VENT EXT] ON (égouttage)");
        return;
    }

    // Blocage chauffage → ventilateur arrêté
    if (blocageChauffage) {
        m_logger.info("[CHAUFFAGE] Blocage Chauffage");
        if (m_ventExtEnMarche.load() || m_vitVentExt.isOn()) {
            m_ventExt.off();
            m_vitVentExt.off();
            m_ventExtEnMarche.store(false);
            m_vitesseVentExtEnMarche.store(false);
            m_logger.info("[VENT EXT] OFF");
        }
        return;
    }
    
    double consigneGrandeVitesse = m_consignes->get("ConsigneGrandeVitesseExterieurChauffage");

    // Fonctionnement normal
    if (!m_ventExtEnMarche.load()) {
        m_ventExt.on();
        m_ventExtEnMarche.store(true);
        m_departTempoComp = std::chrono::steady_clock::now(); // chrono compresseur
        m_logger.info("[VENT EXT] ON");
    }

    // Vitesse selon température extérieure
    if (tempUExt < consigneGrandeVitesse) {
        if (!m_vitVentExt.isOn() || !m_vitesseVentExtEnMarche.load()) {
            m_vitVentExt.on();
            m_vitesseVentExtEnMarche.store(true);
            m_logger.info("[VENT EXT] Grande Vitesse");
        }
    } else {
        if (m_vitVentExt.isOn() || m_vitesseVentExtEnMarche.load()) {
            m_vitVentExt.off();
            m_vitesseVentExtEnMarche.store(false);
            m_logger.info("[VENT EXT] Petite Vitesse");
        }
    }
}

bool Chauffage::isVentExtEnMarche() const {
    return m_ventExtEnMarche.load();
}

bool Chauffage::isVitesseVentExtEnMarche() const {
    return m_vitesseVentExtEnMarche.load();
}

// ================== COMPRESSEUR ==================
void Chauffage::gestionCompresseur(double tempUExt, double tempEExt, bool blocageChauffage) {
    
    // blocage chauffage
    if (blocageChauffage) {
        if (m_compresseurEnMarche.load()) {
            m_compresseur.off();
            m_compresseurEnMarche.store(false);
            m_logger.info("[COMPRESSEUR] OFF (blocage chauffage)");
        }
        return;
    }
    
    int tempoComp = static_cast<int>(m_tempos->get("tempoComp"));
    int tempoDepartDegivrage = static_cast<int>(m_tempos->get("tempoDepartDegCh"));
    int tempoFinDegivrage = static_cast<int>(m_tempos->get("tempoFinDegCh"));
    int tempoEgouttage = static_cast<int>(m_tempos->get("tempoEgouttage"));
    
    double consigneFinDeg = m_consignes->get("ConsigneFinDegivrageChauffage");

    auto maintenant = std::chrono::steady_clock::now();
    
    // Démarrage compresseur après tempo
    if (!m_compresseurEnMarche.load() && !m_degivrageEnMarche.load() && !m_egouttageEnMarche.load()) {
        int ecoule = static_cast<int>(
            std::chrono::duration_cast<std::chrono::seconds>(maintenant - m_departTempoComp).count()
        );

        if (ecoule < tempoComp) {
            int reste = tempoComp - ecoule;
            m_logger.info("[COMPRESSEUR] Attente avant redémarrage : " + std::to_string(reste) + " s restantes");
            return;
        }

        m_compresseur.on();
        m_compresseurEnMarche.store(true);
        m_departCompresseur = maintenant;
        m_logger.info("[COMPRESSEUR] ON");
    }

    // Gestion dégivrage
    if (!m_degivrageEnMarche.load() && !m_egouttageEnMarche.load()) {
        int dureeComp = static_cast<int>(std::chrono::duration_cast<std::chrono::seconds>(maintenant - m_departCompresseur).count());
        
        bool conditionTemps = (dureeComp >= tempoDepartDegivrage);
        bool conditionTemp = (tempUExt >= 5.0 && tempEExt < -3.0) ||
                             (tempEExt < (0.4 * tempUExt - 5.0));

        if (conditionTemps && conditionTemp) {
            m_logger.info("[DÉGIVRAGE] ON");

            m_degivrageEnMarche.store(true);
            m_debutDegivrage = maintenant;
            m_compresseurEnMarche.store(false);
        }
    }

    // Fin dégivrage
    if (m_degivrageEnMarche.load()) {
        m_logger.warning("[DEGIVRAGE] En Cours");
        int dureeDeg = static_cast<int>(
            std::chrono::duration_cast<std::chrono::seconds>(maintenant - m_debutDegivrage).count());

        if (dureeDeg >= tempoFinDegivrage || tempEExt > consigneFinDeg) {
            m_degivrageEnMarche.store(false);
            m_egouttageEnMarche.store(true);
            m_compresseur.off();
            m_debutEgouttage = maintenant;
            m_logger.info("[DÉGIVRAGE] OFF");
        }
    }
    
    // Fin égouttage
    if (m_egouttageEnMarche.load()) {
        m_logger.info("[EGOUTTAGE] ON");
        int dureeEgout = static_cast<int>(
            std::chrono::duration_cast<std::chrono::seconds>(maintenant - m_debutEgouttage).count()
        );
        if (dureeEgout >= tempoEgouttage) {
            m_egouttageEnMarche.store(false);
            m_departTempoComp = maintenant; // reset chrono compresseur
            m_logger.info("[ÉGOUTTAGE] OFF");
        }
    }
}

bool Chauffage::isCompresseurEnMarche() const
{
    return m_compresseurEnMarche.load();
}

bool Chauffage::isDegivrageEnMarche() const
{
    return m_degivrageEnMarche.load();
}

bool Chauffage::isEgouttageEnMarche() const
{
    return m_egouttageEnMarche.load();
}

// ================== VENTILATEUR INTÉRIEUR ==================
void Chauffage::gestionVentilateurInterieur(double tempUInt, double tempEInt, bool blocageChauffage) {
    
    // arrêt automatique en dégivrage
    if (m_degivrageEnMarche.load()) {
        if (m_ventIntEnMarche.load()) {
            m_ventInt.off();
            m_vitVentInt.off();
            m_ventIntEnMarche.store(false);
            m_vitesseVentIntEnMarche.store(false);
            m_logger.info("[VENT INT] OFF");
        }
        return;
    }
    
    // blocage chauffage arrêt immediat du ventilateur
    if (blocageChauffage) {
        if (m_ventIntEnMarche.load()) {
            m_ventInt.off();
            m_vitVentInt.off();
            m_ventIntEnMarche.store(false);
            m_vitesseVentIntEnMarche.store(false);
            m_logger.info("[VENT INT] OFF (blocage chauffage)");
        }
        return;
    }

    // fonctionnement normal
    double consigneDepartVentInt = m_consignes->get("ConsigneDepartVentilateurInterieurChauffage");
    double consignePetiteVitesse = m_consignes->get("ConsignePetiteVitesseInterieurChauffage");
    double hystVentInt = 0.2;

    if (!m_ventIntEnMarche.load() && m_compresseurEnMarche.load() && tempEInt > consigneDepartVentInt) {
        m_ventInt.on();
        m_ventIntEnMarche.store(true);
        m_logger.info("[VENT INT] ON");
    }

    if (m_ventIntEnMarche) {
        if (tempUInt <= consignePetiteVitesse - hystVentInt) {
            if (!m_vitVentInt.isOn()) {
                m_vitVentInt.on();
                m_vitesseVentIntEnMarche.store(true);
                m_logger.info("[VENT INT] Grande Vitesse");
            }
        } else if (tempUInt >= consignePetiteVitesse + hystVentInt) {
            if (m_vitVentInt.isOn()) {
                m_vitVentInt.off();
                m_vitesseVentIntEnMarche.store(false);
                m_logger.info("[VENT INT] Petite Vitesse");
            }
        }
    }
}

bool Chauffage::isVentIntEnMarche() const
{
    return m_ventIntEnMarche.load();
}

bool Chauffage::isVitesseVentIntEnMarche() const
{
    return m_vitesseVentIntEnMarche.load();
}

// ================== VANNE 4 VOIES ==================
void Chauffage::gestionV4V() {
    if (m_degivrageEnMarche.load() && !m_vanne4V.isOn()) {
        m_vanne4V.on();
        m_logger.info("[V4V] ON");
    } else if (!m_degivrageEnMarche.load() && m_vanne4V.isOn()) {
        m_vanne4V.off();
        m_logger.info("[V4V] OFF");
    }
}

