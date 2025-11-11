#pragma once
#include <string>
#include <vector>
#include <thread>
#include <mutex>
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <cmath>
#include "utils/Logger.hpp"

class DS18B20 {
public:
    struct Sonde { 
        std::string id; 
        std::string nom; 
    };

    DS18B20(Logger& loggerRef, int interval = 7);
    ~DS18B20();

    void start();
    void stop();

    // Lecture
    float getTemperature(const std::string& nom);
    std::vector<Sonde> getAllSondes() const;

    // Gestion de l’état PAC
    void setEtatPAC(bool enMarche) { pacEnMarche = enMarche; }
    bool getEtatPAC() const { return pacEnMarche; }

    bool hasCriticalErrors() const;
    bool isReady() const;

private:
    void detecterSondes();
    void rescanSondes();
    bool lireSonde(const Sonde& s, float& temp);
    void boucleLecture();

    Logger& logger;
    int intervalSec;
    
    std::atomic<bool> running;
    std::atomic<bool> pacEnMarche; // Nouvel état PAC (vrai si thermostat en marche)
    std::atomic<bool> ready;
    
    std::thread threadLecture;
    std::condition_variable cv;
    std::mutex cvMutex;
    mutable std::mutex dataMutex;

    std::vector<Sonde> sondes;
    std::vector<bool> capteurHS;
    std::vector<int> erreursConsecutives;
    std::vector<float> dernieresValeurs;
};
