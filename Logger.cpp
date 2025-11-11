#include "Logger.hpp"
#include <sstream>
#include <vector>
#include <fstream>
#include <iomanip>
#include <ctime>

// Codes ANSI pour la couleur en console
#define COLOR_RESET   "\033[0m"
#define COLOR_INFO    "\033[1;32m"  // Vert
#define COLOR_WARNING "\033[1;33m"  // Jaune
#define COLOR_ERROR   "\033[1;31m"  // Rouge

Logger::Logger(const std::string& fname) : filename(fname)
{
    logFile.open(filename, std::ios::app);
    if (!logFile.is_open())
        std::cerr << COLOR_ERROR << "Impossible d'ouvrir le fichier log !" << COLOR_RESET << std::endl;
}

Logger::~Logger()
{
    if (logFile.is_open()) logFile.close();
}

void Logger::write(const std::string& level, const std::string& message, bool urgent)
{
    std::lock_guard<std::mutex> lock(mtx);

    // Horodatage + format ligne
    std::ostringstream line;
    line << "[" << std::setw(7) << std::left << level << "] "
         << getTime() << " : " << message << "\n";

    std::string lineStr = line.str();

    // Écriture dans le fichier (texte brut)
    if (urgent) {
        std::ofstream file(filename, std::ios::app);
        if (file.is_open())
            file << lineStr;
    } else if (logFile.is_open()) {
        logFile << lineStr;
        logFile.flush();
    }

    // Écriture console avec couleur
    const char* color = COLOR_RESET;
    if (level == "INFO") color = COLOR_INFO;
    else if (level == "WARNING") color = COLOR_WARNING;
    else if (level == "ERROR") color = COLOR_ERROR;

    std::cout << color << lineStr << COLOR_RESET;
    std::cout.flush();

    // Rotation du fichier log
    std::ifstream in(filename);
    std::vector<std::string> lignes;
    std::string ligne;
    while (std::getline(in, ligne))
        lignes.push_back(ligne);
    in.close();

    if (lignes.size() > MAX_LINES) {
        std::ofstream out(filename, std::ios::trunc);
        for (size_t i = lignes.size() - MAX_LINES; i < lignes.size(); ++i)
            out << lignes[i] << "\n";
    }
}

void Logger::info(const std::string& message)
{
    write("INFO", message, false);
}

void Logger::warning(const std::string& message)
{
    write("WARNING", message, false);
}

void Logger::error(const std::string& message)
{
    write("ERROR", message, true); // urgent = true pour les pannes
}

std::string Logger::getTime()
{
    std::time_t now = std::time(nullptr);
    char buf[64];
    std::strftime(buf, sizeof(buf), "%a %d %b %Y %H:%M:%S", std::localtime(&now));
    return std::string(buf);
}

