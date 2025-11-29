#pragma once
#include "utils/Logger.hpp"
#include "hardware/Relay.hpp"
#include "logic/Consignes.hpp"
#include "logic/Temporisations.hpp"
#include <atomic>
#include <chrono>

enum class HeatingState {
    IDLE,
    PREHEAT,     // ventilateur ext on + tempo compresseur
    HEATING,     
    DEGIVRAGE,
    EGOUTTAGE
};

class Chauffage
{
public:
    Chauffage(Logger& logger, Consignes* consignes, Temporisations* tempos);
    void update(double tempUExt, double tempEExt, double tempUInt, double tempEInt);
    void stop();

    // Getters
    bool isVentExtEnMarche() const;
    bool isVitesseVentExtEnMarche() const;
    bool isVentIntEnMarche() const;
    bool isVitesseVentIntEnMarche() const;
    bool isCompresseurEnMarche() const;
    bool isDegivrageEnMarche() const;
    bool isEgouttageEnMarche() const;

private:

    // === Core logic ===
    void handleState(double tempUExt, double tempEExt, double tempUInt, double tempEInt);

    void actionIdle();
    void actionPreheat();
    void actionHeating(double tempUExt, double tempEExt, double tempUInt, double tempEInt);
    void actionDegivrage();
    void actionEgouttage();

    void enterState(HeatingState newState);

    // helpers
    void setVentExt(bool on, bool grandeVitesse);
    void setVentInt(bool on, bool grandeVitesse);

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

    std::atomic<bool> m_blocageChauffage{false};

    std::atomic<bool> m_ventExtEnMarche{false};
    std::atomic<bool> m_vitesseVentExtEnMarche{false};
    std::atomic<bool> m_ventIntEnMarche{false};
    std::atomic<bool> m_vitesseVentIntEnMarche{false};
    std::atomic<bool> m_compresseurEnMarche{false};
    std::atomic<bool> m_degivrageEnMarche{false};
    std::atomic<bool> m_egouttageEnMarche{false};

    HeatingState m_state;

    std::chrono::steady_clock::time_point m_departTempoComp;
    std::chrono::steady_clock::time_point m_departCompresseur;
    std::chrono::steady_clock::time_point m_debutDegivrage;
    std::chrono::steady_clock::time_point m_debutEgouttage;
};
