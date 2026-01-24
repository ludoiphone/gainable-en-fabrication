#pragma once
#include "hardware/RelayManager.hpp"
#include "logic/Consignes.hpp"
#include "logic/Temporisations.hpp"
#include <chrono>

enum class CoolingState {
    IDLE = 0,
    COOLING,
    DEGIVRAGE,
    EGOUTTAGE
};

class Froid {
public:
    Froid(RelayManager& relays, Consignes* consignes, Temporisations* tempos);

    void update(double tempUExt, double tempUInt, double tempEInt);
    void stop();

    // Getters
    bool isDefrostActif() const;
    bool isDrainingActif() const;

private:
    // === Actions selon l’état ===
    void handleState(double tempUExt, double tempUInt, double tempEInt);
    void actionIdle();
    void actionCooling(double tempUExt, double tempUInt, double tempEInt);
    void actionDegivrage(double tempEInt);
    void actionEgouttage();

    void enterState(CoolingState newState);

    RelayManager& m_relay;

    Consignes* m_consignes;
    Temporisations* m_tempos;

    CoolingState m_state;

    // timestamps
    std::chrono::steady_clock::time_point m_departTempoV4V;
    std::chrono::steady_clock::time_point m_departTempoComp;
    std::chrono::steady_clock::time_point m_departTempoDegFr;
    std::chrono::steady_clock::time_point m_debutDegivrage;
    std::chrono::steady_clock::time_point m_debutEgouttage;
    
    bool m_memoryFanExt = true;
    bool m_memoryFanInt = true;
};

