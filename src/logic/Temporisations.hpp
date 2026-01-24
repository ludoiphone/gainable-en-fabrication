#pragma once
#include <string>
#include <unordered_map>
#include <filesystem>

class Temporisations {
public:
    explicit Temporisations(const std::string& fichier = "/home/ludo/Desktop/PAC2/config/temporisations.ini");

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

