#pragma once
#include "hardware/RelayManager.hpp"
#include "logic/Consignes.hpp"
#include "logic/Temporisations.hpp"
#include <chrono>

enum class HeatingState {
    IDLE,
    PREHEAT,     // ventilateur ext on + tempo compresseur
    HEATING,     
    DEFROST,
    DRAINING
};

class Chauffage
{
public:
    Chauffage(RelayManager& relays, Consignes* consignes, Temporisations* tempos);
    void update(double tempUExt, double tempEExt, double tempUInt, double tempEInt);
    void stop();

    // Getters
    bool isDefrostActif() const;
    bool isDrainingActif() const;

private:

    // === Core logic ===
    void handleState(double tempUExt, double tempEExt, double tempUInt, double tempEInt);

    void controlExternalFan(double tempEExt);
    
    void actionIdle();
    void actionPreheat(double tempEExt);
    void actionHeating(double tempUExt, double tempEExt, double tempUInt, double tempEInt);
    void actionDefrost(double tempEExt);
    void actionDraining();

    void enterState(HeatingState newState);

    RelayManager& m_relay;

    Consignes* m_consignes;
    Temporisations* m_tempos;
    
    bool m_blocageChauffage = false;

    HeatingState m_state;

    std::chrono::steady_clock::time_point m_departTempoComp;
    std::chrono::steady_clock::time_point m_departCompresseur;
    std::chrono::steady_clock::time_point m_debutDefrost;
    std::chrono::steady_clock::time_point m_debutDraining;
    
    bool m_memoryFanInt = false;
};

