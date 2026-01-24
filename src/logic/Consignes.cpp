#include "Consignes.hpp"
#include <fstream>
#include <iostream>
#include <sstream>

Consignes::Consignes(const std::string& fichier)
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

void Consignes::update() {
    auto currentWriteTime = std::filesystem::last_write_time(m_nomFichier);
    if (currentWriteTime != m_lastWriteTime) {
        std::cout << " Fichier consignes.ini modifié — rechargement...\n";
        charger();
        m_lastWriteTime = currentWriteTime;
    }
}

void Consignes::charger() {
    m_consignes.clear();
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
            m_consignes[key] = std::stod(val);
        } catch (...) {
            std::cerr << "Erreur de lecture pour la clé: " << key << "\n";
        }
    }
}

void Consignes::sauvegarder() {
    std::ofstream file(m_nomFichier);
    if (!file.is_open()) {
        std::cerr << "Impossible d'écrire dans " << m_nomFichier << "\n";
        return;
    }

    file << "[Consignes]\n";
    for (const auto& [key, val] : m_consignes) {
        file << key << "= " << val << "\n";
    }
}

void Consignes::chargerDefaut() {
    m_consignes["ConsigneEteHiver"] = 13.5;
    m_consignes["ConsigneCanicule"] = 30.0;
    m_consignes["ConsigneDelta"] = 6.0;
    m_consignes["ConsigneBlocageChauffage"] = 11.0;
    m_consignes["ConsigneGrandeVitesseExterieurChauffage"] = -1.0;
    m_consignes["ConsigneStopVentilateurExterieurChauffage"] = 8.0;
    m_consignes["ConsigneGrandeVitesseExterieurFroid"] = 20.0;
    m_consignes["ConsignePetiteVitesseExterieurFroid"] = 16.0;
    m_consignes["ConsigneFinDegivrageChauffage"] = 12.5;
    m_consignes["ConsignePetiteVitesseInterieurChauffage"] = 23.0;
    m_consignes["ConsignePetiteVitesseInterieurFroid"] = 23.0;
    m_consignes["ConsigneDepartVentilationInterieurChauffage"] = 35.0;
    m_consignes["ConsigneDegivrageFroid"] = -3.0;
    m_consignes["ConsigneFinDegivrageFroid"] = 10.0;
}

double Consignes::get(const std::string& cle) const {
    auto it = m_consignes.find(cle);
    if (it != m_consignes.end()) return it->second;
    return 0.0;
}

void Consignes::set(const std::string& cle, double valeur) {
    m_consignes[cle] = valeur;
    sauvegarder();
    m_lastWriteTime = std::filesystem::last_write_time(m_nomFichier);
}

