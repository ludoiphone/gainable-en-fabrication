#include "Froid.hpp"
#include <chrono>
#include <thread>

Froid::Froid(Logger& logger, Consignes* consignes, Temporisations* tempos)
    : m_logger(logger),
      m_compresseur(5), m_ventExt(13), m_vitVentExt(16),
      m_ventInt(19), m_vitVentInt(20), m_vanne4V(6),
      m_consignes(consignes), m_tempos(tempos),
      m_ventExtEnMarche(false), m_ventIntEnMarche(false),
      m_compresseurEnMarche(false),  m_degivrageEnMarche(false), 
      m_egouttageEnMarche(false), m_vanne4VEnMarche(false)
{
    //stop();
}

void Froid::stop()
{
    m_compresseur.off();
    m_ventExt.off();
    m_vitVentExt.off();
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

    m_vanne4VEnMarche = false;

    m_logger.info("[FROID] Arrêt complet");
}

void Froid::update(double tempUExt, double tempUInt, double tempEInt)
{
    gestionVentilateurExterieur(tempUExt);
    gestionVentilateurInterieur(tempUInt);
    gestionVanne4V();
    gestionCompresseur(tempEInt);
}

void Froid::gestionVentilateurExterieur(double tempUExt)
{
    // --- Dégivrage : arrêt forcé ---
    if (m_degivrageEnMarche.load()) {
        if (m_ventExtEnMarche.load()) {
            m_ventExt.off();
            m_vitVentExt.off();
            m_ventExtEnMarche.store(false);
            m_vitesseVentExtEnMarche.store(false);
            m_logger.info("[VENT EXT] OFF (dégivrage)");
        }
        return;
    }

    // --- Démarrage normal ---
    if (!m_ventExtEnMarche.load()) {
        m_ventExt.on();
        m_ventExtEnMarche.store(true);
        m_departTempoV4V = std::chrono::steady_clock::now(); // démarrage chrono V4V
        m_logger.info("[VENT EXT] ON");
    }

    // --- Gestion des vitesses ---
    double consignePetiteVitesse = m_consignes->get("ConsignePetiteVitesseExterieurFroid"); // ex: 16°C
    double consigneGrandeVitesse = m_consignes->get("ConsigneGrandeVitesseExterieurFroid"); // ex: 20°C

    if (tempUExt >= consigneGrandeVitesse) {
        // température élevée → grande vitesse
        if (!m_vitVentExt.isOn()) {
            m_vitVentExt.on();
            m_vitesseVentExtEnMarche.store(true);
            m_logger.info("[VENT EXT] Grande vitesse");
        }
    } else if (tempUExt <= consignePetiteVitesse) {
        // température basse → petite vitesse
        if (m_vitVentExt.isOn()) {
            m_vitVentExt.off();
            m_vitesseVentExtEnMarche.store(false);
            m_logger.info("[VENT EXT] Petite vitesse");
        }
    }
}

bool Froid::isVentExtEnMarche() const
{
    return m_ventExtEnMarche.load();
}

bool Froid::isVitesseVentExtEnMarche() const
{
    return m_vitesseVentExtEnMarche.load();
}

void Froid::gestionVentilateurInterieur(double tempUInt)
{
    double consigneGV = m_consignes->get("ConsigneGrandeVitesseInterieurFroid");
    // --- Dégivrage : arrêt forcé ---
    if (m_degivrageEnMarche.load()) {
        if (m_ventIntEnMarche.load()) {
            m_ventInt.off();
            m_vitVentInt.off();
            m_ventIntEnMarche.store(false);
            m_vitesseVentIntEnMarche.store(false);
            m_logger.info("[VENT INT] OFF (dégivrage)");
        }
        return;
    }

    // --- Démarrage normal ---
    if (!m_ventIntEnMarche.load()) {
        m_ventInt.on();
        m_ventIntEnMarche.store(true);
        m_logger.info("[VENT INT] ON");
    }

    // --- Gestion des vitesses ---
    if (tempUInt >= consigneGV) {
        if (!m_vitVentInt.isOn()) {
            m_vitVentInt.on();
            m_vitesseVentIntEnMarche.store(true);
            m_logger.info("[VENT INT] Grande vitesse");
        }
    } else {
        if (m_vitVentInt.isOn()) {
            m_vitVentInt.off();
            m_vitesseVentIntEnMarche.store(false);
            m_logger.info("[VENT INT] Petite vitesse");
        }
    }
}

bool Froid::isVentIntEnMarche() const
{
    return m_ventIntEnMarche.load();
}

bool Froid::isVitesseVentIntEnMarche() const
{
    return m_vitesseVentIntEnMarche.load();
}

void Froid::gestionVanne4V()
{
    if (m_vanne4VEnMarche) return;

    int tempoV4V = static_cast<int>(m_tempos->get("tempoV4V")); // délai avant basculement
    auto maintenant = std::chrono::steady_clock::now();
    int ecoule = std::chrono::duration_cast<std::chrono::seconds>(maintenant - m_departTempoV4V).count();

    if (ecoule >= tempoV4V) {
        m_vanne4V.on();
        m_vanne4VEnMarche = true;
        m_departTempoComp = std::chrono::steady_clock::now(); // démarrage chrono compresseur
        m_logger.info("[V4V] ON");
    }
}

void Froid::gestionCompresseur(double tempEInt)
{
    auto maintenant = std::chrono::steady_clock::now();

    int tempoComp = static_cast<int>(m_tempos->get("tempoComp"));
    int tempoFinDegivrage = static_cast<int>(m_tempos->get("tempoFinDegFr"));
    int tempoEgouttage = static_cast<int>(m_tempos->get("tempoEgouttage"));
    double consigneDegivrage = m_consignes->get("ConsigneDegivrageFroid");

    // === Étape 1 : Dégivrage en cours ===
    if (m_degivrageEnMarche.load()) {
        int dureeDeg = std::chrono::duration_cast<std::chrono::seconds>(maintenant - m_debutDegivrage).count();
        if (dureeDeg >= tempoFinDegivrage) {
            m_logger.info("[DÉGIVRAGE] Fin — début égouttage");
            m_degivrageEnMarche.store(false);
            m_egouttageEnMarche.store(true);
            m_debutEgouttage = maintenant;
        }
        return;
    }

    // === Étape 2 : Égouttage ===
    if (m_egouttageEnMarche.load()) {
        int dureeEgout = std::chrono::duration_cast<std::chrono::seconds>(maintenant - m_debutEgouttage).count();
        if (dureeEgout >= tempoEgouttage) {
            m_logger.info("[ÉGOUTTAGE] Fin — reprise cycle froid");
            m_egouttageEnMarche.store(false);
            stop(); // reset complet du cycle
        }
        return;
    }

    // === Étape 3 : Déclenchement dégivrage ===
    if (!m_degivrageEnMarche.load() && !m_egouttageEnMarche.load() && tempEInt <= consigneDegivrage) {
        m_logger.info("[DÉGIVRAGE] Démarrage — TEInt=" + std::to_string(tempEInt));
        m_compresseur.off();
        m_ventExt.off();
        m_vitVentExt.off();
        m_ventInt.off();
        m_vitVentInt.off();
        m_vanne4V.off();
        m_compresseurEnMarche.store(false);
        m_degivrageEnMarche.store(true);
        m_debutDegivrage = maintenant;
        return;
    }

    // === Étape 4 : Démarrage compresseur normal ===
    if (!m_vanne4VEnMarche || m_compresseurEnMarche.load()) return;

    int ecoule = std::chrono::duration_cast<std::chrono::seconds>(maintenant - m_departTempoComp).count();
    if (ecoule >= tempoComp) {
        m_compresseur.on();
        m_compresseurEnMarche.store(true);
        m_logger.info("[COMPRESSEUR] ON");
    }
}

bool Froid::isCompresseurEnMarche() const
{
    return m_compresseurEnMarche.load();
}

bool Froid::isDegivrageEnMarche() const
{
    return m_degivrageEnMarche.load();
}

bool Froid::isEgouttageEnMarche() const
{
    return m_egouttageEnMarche.load();
}

