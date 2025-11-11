#pragma once
#include "utils/Logger.hpp"
#include "logic/Consignes.hpp"
#include "logic/Temporisations.hpp"
#include "hardware/Relay.hpp"
#include <chrono>
#include <ctime>
#include <atomic>

class Chauffage {
public:
    Chauffage(Logger& logger, Consignes* consignes, Temporisations* tempos);
    
    void update(double tempUExt, double tempEExt, double tempUInt, double tempEInt);
    void stop();
    
    bool isVentExtEnMarche() const;
    bool isVitesseVentExtEnMarche() const;
    bool isVentIntEnMarche() const;
    bool isVitesseVentIntEnMarche() const;
    
    bool isCompresseurEnMarche() const;
    
    bool isDegivrageEnMarche() const;
    
    bool isEgouttageEnMarche() const;
    
private:
    Logger& m_logger;
    Relay m_ventExt;
    Relay m_vitVentExt;
    Relay m_compresseur;
    Relay m_ventInt;
    Relay m_vitVentInt;
    Relay m_vanne4V;

    Consignes* m_consignes;
    Temporisations* m_tempos;

    std::atomic<bool> m_ventExtEnMarche;
    std::atomic<bool> m_vitesseVentExtEnMarche;
    std::atomic<bool> m_ventIntEnMarche;
    std::atomic<bool> m_vitesseVentIntEnMarche;
    
    std::atomic<bool> m_compresseurEnMarche;
    
    std::atomic<bool> m_degivrageEnMarche;
    
    std::atomic<bool> m_egouttageEnMarche;

    std::chrono::steady_clock::time_point m_departTempoComp;
    std::chrono::steady_clock::time_point m_departCompresseur;
    std::chrono::steady_clock::time_point m_debutDegivrage;
    std::chrono::steady_clock::time_point m_debutEgouttage;
    
    void gestionVentilateurExterieur(double tempUExt, bool blocageChauffage);
    void gestionCompresseur(double tempUExt, double tempEExt, bool blocageChauffage);
    void gestionVentilateurInterieur(double tempUInt, double tempEInt, bool blocageChauffage);
    void gestionV4V();
};
