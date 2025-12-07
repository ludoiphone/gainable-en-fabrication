#pragma once
#include <string>
#include <array>
#include <vector>
#include <thread>
#include <mutex>
#include <atomic>
#include <cmath>

struct ChannelConfig {
    std::string name;
    double offset = 0.0;
    double valeur = NAN;
};

class NTCReader {

public:
    NTCReader(const std::string& iniFile, int i2c_address = 0x4B);
    ~NTCReader();

    void start(); 
    void stop();
    
    std::vector<double> getTemperatures();
    double getTemperature(const std::string &name);
    std::string getChannelName(int phys);

private:
    int i2c_fd;
    int address;
    
    
    
    std::array<ChannelConfig,5> channels;
    std::array<double,5> temperatures;

    std::mutex mtx;
    std::atomic<bool> running;
    std::string configFile;
    std::thread worker;
    
    // Timer auto reload
    std::chrono::steady_clock::time_point lastConfigLoad;
    const std::chrono::seconds reloadInterval = std::chrono::seconds(2);
    
    // --- CALCUL + RAPIDE ---
    const double VCC = 3.3;
    const double INV_255 = 1.0 / 255.0;

    // PTA04 mapping
    const std::array<int,8> channel_map = {0,4,1,5,2,6,3,7};

    // Lissage léger
    double alpha = 0.40;

    // Fonctions internes
    int readADC(int phys);
    inline double adcToVoltageFast(int v);
    inline double voltageToTemperatureFast(double V);

    void run();
    void loadConfig(const std::string& filename);
};

