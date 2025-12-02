#pragma once
#include "hardware/Relay.hpp"
#include "logic/Consignes.hpp"
#include "logic/Temporisations.hpp"
#include <chrono>

enum class CoolingState {
    IDLE = 0,
    COOLING,
    DEGIVRAGE,
    EGOUTTAGE
};

enum class VentSpeedCool { VENT_OFF = 0, VENT_LOW = 1, VENT_HIGH = 2 };/////

class Froid {
public:
    Froid(Consignes* consignes, Temporisations* tempos);

    void update(double tempUExt, double tempUInt, double tempEInt);
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
    // === Actions selon l’état ===
    void handleState(double tempUExt, double tempUInt, double tempEInt);
    void actionIdle();
    void actionCooling(double tempUExt, double tempUInt, double tempEInt);
    void actionDegivrage();
    void actionEgouttage();

    void enterState(CoolingState newState);

    void setVentExt(bool on, bool grandeVitesse);
    void setVentInt(bool on, bool grandeVitesse);

    Relay m_compresseur;
    Relay m_ventExt;
    Relay m_vitVentExt;
    Relay m_ventInt;
    Relay m_vitVentInt;
    Relay m_vanne4V;

    Consignes* m_consignes;
    Temporisations* m_tempos;

    CoolingState m_state;

    // timestamps
    std::chrono::steady_clock::time_point m_departTempoComp;
    std::chrono::steady_clock::time_point m_debutDegivrage;
    std::chrono::steady_clock::time_point m_debutEgouttage;
    
    // mémoire vitesse ventilateur extérieur
    VentSpeedCool m_lastVentExtSpeed;
    VentSpeedCool m_lastVentIntSpeed;
};
