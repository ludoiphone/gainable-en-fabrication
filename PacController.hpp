#pragma once
#include "utils/Logger.hpp"
#include "hardware/DS18B20.hpp"
#include "logic/Consignes.hpp"
#include "logic/Temporisations.hpp"
#include "hardware/Chauffage.hpp"
#include "hardware/Froid.hpp"
#include "hardware/Relay.hpp"
#include "hardware/SwitchSensor.hpp"
#include <string>
#include <thread>
#include <atomic>
#include <chrono>

enum ModePAC {
    MODE_ARRET,
    MODE_CHAUFFAGE,
    MODE_FROID
};

class PacController {
public:
    PacController(Logger& logger, DS18B20& sondes, Consignes& consignes, Temporisations& temporisations);
    ~PacController();

    void stopPac();
    void pausePac();
    void resumePac();

    double getTemperature(const std::string& name) const;
    DS18B20* getSondes() { return &m_sondes; }
    
    ModePAC getModeActuel() const;
    
    bool getVentilateurExt() const;
    bool getVentilateurInt() const;
    int getVitesseVentilateurExt() const;
    int getVitesseVentilateurInt() const;
    
    bool getCompresseur() const;
    
    bool getDegivrage() const;
    
    bool getEgouttage() const;

private:
    Logger& m_logger;
    DS18B20& m_sondes;
    Consignes& m_consignes;
    Temporisations& m_temporisations;
    
    void loop();
    void stopAll();
    void pauseAll();
    ModePAC determineMode(double tempExt) const;
    std::string modeToString(ModePAC m) const;

    Chauffage m_chauffage;
    Froid m_froid;
    SwitchSensor m_thermostat;
    Relay m_relayEteHiver;

    std::atomic<bool> m_stopFlag;
    std::atomic<bool> m_pauseFlag;

    ModePAC m_modeActuel;
    bool m_voletBloque;
    bool m_securite;
    bool m_enAttenteVolets;
    std::chrono::steady_clock::time_point m_debutTempoVolets;

    std::thread m_thread;
};

