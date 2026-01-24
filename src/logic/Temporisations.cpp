#include "Temporisations.hpp"
#include <fstream>
#include <iostream>
#include <sstream>

Temporisations::Temporisations(const std::string& fichier)
    : m_nomFichier(fichier)
{
    if (std::filesystem::exists(m_nomFichier)) {
        charger();
        m_lastWriteTime = std::filesystem::last_write_time(m_nomFichier);
    } else {
        chargerDefaut();
        sauvegarder();
        m_lastWriteTime = std::filesystem::last_write_time(m_nomFichier);
    }
}

void Temporisations::update() {
    auto currentWriteTime = std::filesystem::last_write_time(m_nomFichier);
    if (currentWriteTime != m_lastWriteTime) {
        std::cout << "Fichier temporisations.ini modifié — rechargement...\n";
        charger();
        m_lastWriteTime = currentWriteTime;
    }
}

void Temporisations::charger() {
    m_temporisations.clear();
    std::ifstream file(m_nomFichier);
    if (!file.is_open()) {
        std::cerr << "Impossible d'ouvrir " << m_nomFichier << "\n";
        return;
    }

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty() || line[0] == ';' || line[0] == '[') continue;
        auto pos = line.find('=');
        if (pos == std::string::npos) continue;

        std::string key = line.substr(0, pos);
        std::string val = line.substr(pos + 1);
        try {
            m_temporisations[key] = std::stod(val);
        } catch (...) {
            std::cerr << "Erreur de lecture pour la clé: " << key << "\n";
        }
    }
}

void Temporisations::sauvegarder() {
    std::ofstream file(m_nomFichier);
    if (!file.is_open()) {
        std::cerr << "Impossible d'écrire dans " << m_nomFichier << "\n";
        return;
    }
    
    file << "[Temporisations]\n";
    for (const auto& [key, value] : m_temporisations)
        file << key << "=" << std::fixed << std::setprecision(0) << value << "\n";
}

void Temporisations::chargerDefaut() {
    m_temporisations["tempoVolets"] = 15; // 15 secondes
    m_temporisations["tempoComp"] = 180; // 3 minutes
    m_temporisations["tempoDepartDegCh"] = 2400; // 40 minutes
    m_temporisations["tempoFinDegCh"] = 600; // 10 minutes
    m_temporisations["tempoEgouttage"] = 120; // 2 minute
    m_temporisations["tempoV4V"] = 60; // 1 minute
    m_temporisations["tempoDepartDegFr"] = 360; // 6 minutes
    m_temporisations["tempoFinDegFr"] = 300; // 5 minutes
    
    m_temporisations["tempoNettoyageFiltre"] = 7776000; // 90 jours
    m_temporisations["tempoArretNettoyageFiltre"] = 7257600; // 7 jours
    m_temporisations["dureeExecutionFiltre"] = 0; // sauvegarde temps de fonctionnement ventilateur interieur pour nettoyage filtre
}

double Temporisations::get(const std::string& cle) const {
    auto it = m_temporisations.find(cle);
    if (it != m_temporisations.end()) return it->second;
    return 0.0;
}

void Temporisations::set(const std::string& cle, double valeur) {
    m_temporisations[cle] = valeur;
    sauvegarder();
    m_lastWriteTime = std::filesystem::last_write_time(m_nomFichier);
}

