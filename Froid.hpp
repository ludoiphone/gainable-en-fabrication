#pragma once
#include "utils/Logger.hpp"
#include "logic/Consignes.hpp"
#include "logic/Temporisations.hpp"
#include "hardware/Relay.hpp"
#include <chrono>
#include <atomic>

class Froid {
public:
    Froid(Logger& logger, Consignes* consignes, Temporisations* tempos);

    void update(double tempUExt, double tempUInt, double tempEInt);
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

    // Relais
    Relay m_compresseur;
    Relay m_ventExt;
    Relay m_vitVentExt;
    Relay m_ventInt;
    Relay m_vitVentInt;
    Relay m_vanne4V;
    
    // logic
    Consignes* m_consignes;
    Temporisations* m_tempos;

    // États internes
    std::atomic<bool> m_ventExtEnMarche;
    std::atomic<bool> m_vitesseVentExtEnMarche;
    std::atomic<bool> m_ventIntEnMarche;
    std::atomic<bool> m_vitesseVentIntEnMarche;
    
    std::atomic<bool> m_compresseurEnMarche;
    
    std::atomic<bool> m_degivrageEnMarche;
    
    std::atomic<bool> m_egouttageEnMarche;
    
    bool m_vanne4VEnMarche;

    std::chrono::steady_clock::time_point m_departTempoV4V;
    std::chrono::steady_clock::time_point m_departTempoComp;
    std::chrono::steady_clock::time_point m_debutDegivrage;
    std::chrono::steady_clock::time_point m_debutEgouttage;

    // Méthodes internes
    void gestionCompresseur(double tempEInt);
    void gestionVentilateurExterieur(double tempUExt);
    void gestionVentilateurInterieur(double tempUInt);
    void gestionVanne4V();
};
