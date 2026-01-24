#pragma once
#include <string>
#include <array>
#include <vector>
#include <thread>
#include <mutex>
#include <atomic>

struct ChannelConfig {
    std::string name;
    double offset = 0.0;
};

class NTCReader {
public:
    NTCReader(const std::string& iniFile, int i2c_address = 0x4B);
    ~NTCReader();

    void start();
    void stop();

    std::vector<double> getTemperatures();        // température lissée
    double getTemperature(const std::string &name);
    std::string getChannelName(int phys);

    int getChannelIndex(const std::string& name);
    int getADC(int phys);
    double getVoltage(int phys);
    double getResistance(int phys);

private:
    void run();
    void loadConfig(const std::string& filename);
    int readADC(int phys);
    inline double adcToVoltageFast(int v);
    inline double voltageToTemperatureFast(double V);

    int i2c_fd;
    int address;

    std::array<ChannelConfig,5> channels;
    std::array<double,5> temperatures;

    std::mutex mtx;
    std::atomic<bool> running;
    std::string configFile;
    std::thread worker;

    std::chrono::steady_clock::time_point lastConfigLoad;
    const std::chrono::seconds reloadInterval = std::chrono::seconds(2);

    const double VCC = 3.3;
    const std::array<int,8> channel_map = {0,4,1,5,2,6,3,7};
    double alpha = 0.05;// 0.40
    
    std::array<std::chrono::steady_clock::time_point, 5> lastLog{};
    std::chrono::seconds logInterval{20};
};
