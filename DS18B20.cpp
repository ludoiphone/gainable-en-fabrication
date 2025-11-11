#include "DS18B20.hpp"
#include <filesystem>
#include <fstream>
#include <iostream>

namespace fs = std::filesystem;
using namespace std::chrono;

DS18B20::DS18B20(Logger& loggerRef, int interval)
    : logger(loggerRef),
      intervalSec(interval),
      running(false),
      pacEnMarche(false),
      ready(false) {}

DS18B20::~DS18B20() { stop(); }

void DS18B20::start() {
    if (running.exchange(true)) return;

    detecterSondes();
    ready.store(false);

    threadLecture = std::thread(&DS18B20::boucleLecture, this);
    logger.info("DS18B20 thread démarré (intervalle " + std::to_string(intervalSec) + "s).");
}

void DS18B20::stop() {
    if (!running.exchange(false)) return;
    cv.notify_all();
    if (threadLecture.joinable()) threadLecture.join();
    logger.info("DS18B20 thread arrêté proprement.");
}

void DS18B20::detecterSondes() {
    sondes.clear();
    std::vector<std::string> noms = {"Ext", "UExt", "EExt", "UInt", "EInt"};

    for (const auto& e : fs::directory_iterator("/sys/bus/w1/devices")) {
        std::string id = e.path().filename().string();
        if (id.rfind("28-", 0) == 0)
            sondes.push_back({id, noms.size() > sondes.size() ? noms[sondes.size()] : id});
    }

    for (size_t i = sondes.size(); i < noms.size(); ++i)
        sondes.push_back({"", noms[i]});

    dernieresValeurs.assign(sondes.size(), NAN);
    erreursConsecutives.assign(sondes.size(), 0);
    capteurHS.assign(sondes.size(), false);
}

bool DS18B20::lireSonde(const Sonde& s, float& temp) {
    if (s.id.empty()) return false;

    std::string path = "/sys/bus/w1/devices/" + s.id + "/w1_slave";
    if (!fs::exists(path)) return false;

    try {
        std::ifstream f(path);
        if (!f.is_open()) return false;

        std::string l1, l2;
        std::getline(f, l1);
        std::getline(f, l2);
        f.close();

        if (l1.find("YES") == std::string::npos || l2.find("t=") == std::string::npos)
            return false;

        temp = std::stof(l2.substr(l2.find("t=") + 2)) / 1000.0f;
        if (temp < -55.0f || temp > 125.0f) return false;
        temp = std::round(temp * 100.0f) / 100.0f;

        return true;
    } catch (const std::exception& e) {
        logger.error("Erreur lecture sonde " + s.nom + " : " + e.what());
        return false;
    } catch (...) {
        logger.error("Erreur inconnue lecture sonde " + s.nom);
        return false;
    }
}

void DS18B20::rescanSondes() {
    for (auto& s : sondes) {
        if (s.id.empty()) {
            for (const auto& e : fs::directory_iterator("/sys/bus/w1/devices")) {
                std::string id = e.path().filename().string();
                if (id.rfind("28-", 0) == 0) {
                    s.id = id;
                    logger.info("Nouvelle sonde détectée : " + s.nom + " (" + id + ")");
                    break;
                }
            }
        }
    }
}

void DS18B20::boucleLecture() {
    using namespace std::chrono_literals;
    ready.store(true);
    auto nextWake = steady_clock::now();
    static int cycles = 0;

    while (running.load()) {
        nextWake += seconds(intervalSec);

        std::vector<float> nouvellesValeurs(sondes.size(), NAN);
        std::vector<int> nouvellesErreurs(sondes.size(), 0);
        std::vector<bool> nouveauxHS(sondes.size(), false);

        for (size_t i = 0; i < sondes.size(); ++i) {
            try {
                float temp;
                bool ok = lireSonde(sondes[i], temp);

                if (ok) {
                    nouvellesValeurs[i] = temp;
                    nouvellesErreurs[i] = 0;
                    nouveauxHS[i] = false;
                } else {
                    nouvellesErreurs[i] = erreursConsecutives[i] + 1;
                    nouveauxHS[i] = (nouvellesErreurs[i] >= 3);
                    if (nouveauxHS[i] && !capteurHS[i])
                        logger.error("Sonde " + sondes[i].nom + " HS (3 échecs consécutifs)");
                }
            } catch (...) {
                logger.error("Exception inattendue lors de la lecture de " + sondes[i].nom);
            }
        }

        {
            std::lock_guard<std::mutex> lock(dataMutex);
            dernieresValeurs = std::move(nouvellesValeurs);
            erreursConsecutives = std::move(nouvellesErreurs);
            capteurHS = std::move(nouveauxHS);
        }

        if (++cycles % (60 / intervalSec) == 0)
            rescanSondes();

        std::unique_lock<std::mutex> lk(cvMutex);
        cv.wait_until(lk, nextWake, [this]() { return !running.load(); });
    }
}

float DS18B20::getTemperature(const std::string& nom) {
    std::lock_guard<std::mutex> lock(dataMutex);
    for (size_t i = 0; i < sondes.size(); ++i)
        if (sondes[i].nom == nom)
            return dernieresValeurs[i];
    return NAN;
}

std::vector<DS18B20::Sonde> DS18B20::getAllSondes() const {
    std::lock_guard<std::mutex> lock(dataMutex);
    return sondes;
}

bool DS18B20::hasCriticalErrors() const {
    std::lock_guard<std::mutex> lock(dataMutex);
    for (bool hs : capteurHS)
        if (hs) return true;
    return false;
}

bool DS18B20::isReady() const { return ready.load(); }

