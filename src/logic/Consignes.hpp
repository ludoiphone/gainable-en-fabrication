#pragma once
#include <string>
#include <unordered_map>
#include <filesystem>

class Consignes {
public:
    explicit Consignes(const std::string& fichier = "/home/ludo/Desktop/Gainable/config/consignes.ini");

    void update(); // recharge le fichier si modifié
    double get(const std::string& cle) const;
    void set(const std::string& cle, double valeur);
    void sauvegarder();

private:
    std::string m_nomFichier;
    std::unordered_map<std::string, double> m_consignes;
    std::filesystem::file_time_type m_lastWriteTime;

    void charger();
    void chargerDefaut();
};

