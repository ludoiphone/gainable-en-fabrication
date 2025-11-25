#pragma once
#include <string>
#include <unordered_map>
#include <fstream>
#include <sstream>
#include <iostream>
#include <filesystem>
#include <chrono>

class Temporisations {
public:
    explicit Temporisations(const std::string& fichier = "temporisations.ini");

    void update(); // recharge le fichier s’il a été modifié
    double get(const std::string& cle) const;
    void set(const std::string& cle, double valeur);

private:
    std::string m_nomFichier;
    std::unordered_map<std::string, double> m_temporisations;
    std::filesystem::file_time_type m_lastWriteTime;

    void charger();
    void sauvegarder();
    void chargerDefaut();
};

