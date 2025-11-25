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
    double offset;
    double valeur = NAN; // utilisé uniquement si la sonde est simulée
};

class NTCReader {
private:
    int i2c_fd;
    int address;
    
    std::array<ChannelConfig,5> channels;
    std::array<double,5> temperatures;
    
    const double VCC = 3.3;
    const double R_FIXED = 10000.0;
    const double BETA = 3950.0;
    const double T0 = 298.15; // 25°C en Kelvin
    const double R0 = 10000.0;
    const std::array<int,8> channel_map = {0,4,1,5,2,6,3,7};

    std::mutex mtx;
    std::atomic<bool> running;
    std::thread worker;
    double alpha = 0.2; // facteur de lissage EMA

    int readADC(int phys);
    double adcToVoltage(int val);
    double voltageToTemperature(double V);
    void run();
    void loadConfig(const std::string& filename);

public:
    NTCReader(const std::string& iniFile, int i2c_address = 0x4B);
    ~NTCReader();

    void start(); 
    void stop();
    
    std::vector<double> getTemperatures();
    double getTemperature(const std::string &name);
    std::string getChannelName(int phys); // retourne le nom de la sonde
};


