#pragma once
#include "hardware/NTCReader.hpp"
#include <string>
#include <unordered_map>

class NTCManager {
public:
    NTCManager(NTCReader& sondes);

    // Met à jour toutes les sondes
    void update();

    // Vérifie si une sonde est OK
    bool isValid(const std::string& name) const;

    // Retourne la température (NAN si défaut)
    double getTemperature(const std::string& name) const;

    // État global de défaut
    bool hasFault() const;

private:
    NTCReader& m_sondes;
    std::unordered_map<std::string, bool> m_validSondes;
    bool m_fault;
};
