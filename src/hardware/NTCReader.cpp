#include "NTCReader.hpp"
#include "utils/Log.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>
#include <iomanip>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>

NTCReader::NTCReader(const std::string& iniFile, int i2c_address)
    : i2c_fd(-1), address(i2c_address), running(false), configFile(iniFile)
{
    i2c_fd = open("/dev/i2c-1", O_RDWR);
    if (i2c_fd >= 0 && ioctl(i2c_fd, I2C_SLAVE, address) < 0) {
        perror("I2C addr");
    }

    for (int i = 0; i < 5; ++i) {
        channels[i].name = "CH" + std::to_string(i);
        channels[i].offset = 0.0;
        temperatures[i] = 20.0;
    }

    loadConfig(configFile);
    lastConfigLoad = std::chrono::steady_clock::now();
}

NTCReader::~NTCReader() {
    stop();
    if (i2c_fd >= 0) close(i2c_fd);
}

int NTCReader::readADC(int phys) {
    if (i2c_fd < 0) return -1;

    int ch = channel_map[phys];
    char cmd = 0x84 | (ch << 4);

    if (write(i2c_fd, &cmd, 1) != 1) return -1;

    unsigned char buf = 0;
    if (read(i2c_fd, &buf, 1) != 1) return -1;

    return static_cast<int>(buf);
}

inline double NTCReader::adcToVoltageFast(int v) {
    return v / 255.0 * VCC;
}

inline double NTCReader::voltageToTemperatureFast(double V) {
    if (V <= 0.01 || V >= VCC - 0.01) return NAN;
    double R = 10000.0 * V / (VCC - V);
    double invT = (1.0 / 298.15) + std::log(R / 10000.0) / 3950.0;
    return (1.0 / invT) - 273.15;
}

void NTCReader::run() {
    while (running.load()) {
        auto now = std::chrono::steady_clock::now();
        if (now - lastConfigLoad >= reloadInterval) {
            std::lock_guard<std::mutex> lock(mtx);
            loadConfig(configFile);
            lastConfigLoad = now;
        }

        for (int i = 0; i < 5; ++i) {
            int adc = readADC(i);
            if (adc < 0) continue;

            double V = adcToVoltageFast(adc);
            double T = voltageToTemperatureFast(V);

            std::lock_guard<std::mutex> lock(mtx);
            
            if (!std::isnan(T)) {
                temperatures[i] += alpha * ((T + channels[i].offset) - temperatures[i]);
            }
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }
}

void NTCReader::start() {
    if (!running.load()) {
        running.store(true);
        worker = std::thread(&NTCReader::run, this);
    }
}

void NTCReader::stop() {
    running.store(false);
    if (worker.joinable()) worker.join();
}

std::vector<double> NTCReader::getTemperatures() {
    std::lock_guard<std::mutex> lock(mtx);
    return std::vector<double>(temperatures.begin(), temperatures.end());
}

double NTCReader::getTemperature(const std::string &name) {
    std::lock_guard<std::mutex> lock(mtx);
    for (int i = 0; i < 5; ++i) {
        if (channels[i].name == name) return temperatures[i];
    }
    return temperatures[0];
}

std::string NTCReader::getChannelName(int phys) {
    if (phys >= 0 && phys < 5) return channels[phys].name;
    return "CH?";
}

int NTCReader::getChannelIndex(const std::string& name)
{
    for (int i = 0; i < 5; ++i) {
        if (channels[i].name == name)
            return i;
    }
    return -1; // nom inconnu
}

int NTCReader::getADC(int phys) {
    return readADC(phys);
}

double NTCReader::getVoltage(int phys) {
    int adc = readADC(phys);
    if(adc < 0) return -1.0;
    return adcToVoltageFast(adc);
}

double NTCReader::getResistance(int phys) {
    double V = getVoltage(phys);
    if(V <= 0.0 || V >= VCC) return -1.0;
    return 10000.0 * V / (VCC - V);
}

void NTCReader::loadConfig(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) return;

    std::string line, section;
    while (std::getline(file, line)) {
        line.erase(0, line.find_first_not_of(" \t\r\n"));
        line.erase(line.find_last_not_of(" \t\r\n") + 1);
        if (line.empty() || line[0] == '#' || line[0] == ';') continue;

        if (line.front() == '[' && line.back() == ']') {
            section = line.substr(1, line.size() - 2);
            continue;
        }

        auto pos = line.find('=');
        if (pos == std::string::npos) continue;

        std::string key = line.substr(0, pos);
        std::string value = line.substr(pos + 1);
        key.erase(0, key.find_first_not_of(" \t\r\n"));
        key.erase(key.find_last_not_of(" \t\r\n") + 1);
        value.erase(0, value.find_first_not_of(" \t\r\n"));
        value.erase(value.find_last_not_of(" \t\r\n") + 1);

        for (int i = 0; i < 5; ++i) {
            if (section == "CH" + std::to_string(i)) {
                if (key == "name") channels[i].name = value;
                else if (key == "offset") {
                    try { channels[i].offset = std::stod(value); }
                    catch (...) {}
                }
            }
        }
    }
}
