#include "NTCReader.hpp"
#include <iostream>
#include <fstream>
#include <cmath>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <chrono>
#include <sstream>
#include <algorithm>

NTCReader::NTCReader(const std::string& iniFile, int i2c_address)
    : address(i2c_address), running(false)
{
    i2c_fd = open("/dev/i2c-1", O_RDWR);
    if (i2c_fd < 0) {
        perror("Erreur ouverture I2C");
        exit(1);
    }

    if (ioctl(i2c_fd, I2C_SLAVE, address) < 0) {
        perror("Erreur sélection adresse I2C");
        exit(1);
    }
    
    // Initialisation par défaut
    for (int i = 0; i <5; ++i) {
        channels[i].name = "CH" + std::to_string(i);
        channels[i].offset = 0;
        channels[i].valeur = NAN;
        temperatures[i] = NAN;
    }

    // Charger le fichier INI au démarrage
    loadConfig(iniFile);
}

NTCReader::~NTCReader() {
    stop();
    if (i2c_fd >= 0) close(i2c_fd);
}

int NTCReader::readADC(int phys) {
    int ch = channel_map[phys];

    char command = 0x84 | (ch << 4);
    if (write(i2c_fd, &command, 1) != 1) {
        perror("Erreur écriture I2C");
        return -1;
    }

    char buf;
    if (read(i2c_fd, &buf, 1) != 1) {
        perror("Erreur lecture I2C");
        return -1;
    }
    
    int val = static_cast<unsigned char>(buf);
    if (val == 0 || val == 255) {
        std::cerr << "[NTCReader] WARN readADC returned " << val << " (board ou mapping ?) \n";
    }
    return val;
}

double NTCReader::adcToVoltage(int val) {
    return val / 255.0 * VCC;
}

double NTCReader::voltageToTemperature(double V) {
    if (V <= 0 || V >= VCC) return NAN;

    double R_ctn = R_FIXED * V / (VCC - V);
    double T = 1.0 / (1.0 / T0 + log(R_ctn / R0) / BETA);

    return T - 273.15;
}

// --- Rechargement dynamique du fichier INI ---
void NTCReader::loadConfig(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "[NTCReader] Impossible d'ouvrir " << filename << "\n";
        return;
    }

    std::string line, currentSection;
    while (std::getline(file, line)) {
        // Trim espaces
        line.erase(0, line.find_first_not_of(" \t\r\n"));
        line.erase(line.find_last_not_of(" \t\r\n") + 1);

        if (line.empty() || line[0] == ';' || line[0] == '#') continue;

        if (line.front() == '[' && line.back() == ']') {
            currentSection = line.substr(1, line.size() - 2);
            continue;
        }

        auto eqPos = line.find('=');
        if (eqPos == std::string::npos) continue;

        std::string key = line.substr(0, eqPos);
        std::string value = line.substr(eqPos + 1);

        key.erase(0, key.find_first_not_of(" \t\r\n"));
        key.erase(key.find_last_not_of(" \t\r\n") + 1);
        value.erase(0, value.find_first_not_of(" \t\r\n"));
        value.erase(value.find_last_not_of(" \t\r\n") + 1);

        for (int i = 0; i < 5; ++i) {
            std::string chName = "CH" + std::to_string(i);
            if (currentSection == chName) {
                std::lock_guard<std::mutex> lock(mtx);
                if (key == "name") channels[i].name = value;
                else if (key == "offset") channels[i].offset = std::stod(value);
                else if (key == "valeur") channels[i].valeur = std::stod(value);
            }
        }
    }

    file.close();
}

void NTCReader::run() {
    auto lastConfigReload = std::chrono::steady_clock::now();

    while (running) {
        // CH0–CH3 sondes NTC physiques
        for (int phys = 0; phys < 4; ++phys) {
            int adc_val = readADC(phys);
            double V = adcToVoltage(adc_val);
            double T = voltageToTemperature(V);

            std::lock_guard<std::mutex> lock(mtx);
            T += channels[phys].offset;
            if (std::isnan(temperatures[phys]))
                temperatures[phys] = T;
            else
                temperatures[phys] = alpha * T + (1 - alpha) * temperatures[phys];
        }

        // CH4 = sonde simulée
        {
            std::lock_guard<std::mutex> lock(mtx);
            if (!std::isnan(channels[4].valeur))
                temperatures[4] = channels[4].valeur + channels[4].offset;
        }

        // Rechargement config toutes les 5 secondes
        auto now = std::chrono::steady_clock::now();
        if (std::chrono::duration_cast<std::chrono::seconds>(now - lastConfigReload).count() >= 5) {
            loadConfig("configNTC.ini");
            lastConfigReload = now;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
}

void NTCReader::start() {
    if (!running) {
        running = true;
        worker = std::thread(&NTCReader::run, this);
    }
}

void NTCReader::stop() {
    if (running) {
        running = false;
        if (worker.joinable())
            worker.join();
    }
}

std::vector<double> NTCReader::getTemperatures() {
    std::lock_guard<std::mutex> lock(mtx);
    return std::vector<double>(temperatures.begin(), temperatures.end());
}

double NTCReader::getTemperature(const std::string &name) {
    std::lock_guard<std::mutex> lock(mtx);
    for (int i = 0; i < 5; ++i) {
        if (channels[i].name == name)
            return temperatures[i];
    }
    return NAN;
}

std::string NTCReader::getChannelName(int phys) {
    if (phys >= 0 && phys < 5) return channels[phys].name;
    return "CH" + std::to_string(phys);
}

