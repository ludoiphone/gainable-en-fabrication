#pragma once
#include <string>
#include <fstream>
#include <mutex>
#include <iostream>

class Logger {
public:
    explicit Logger(const std::string& fname);
    ~Logger();

    void info(const std::string& message);
    void warning(const std::string& message);
    void error(const std::string& message);

private:
    void write(const std::string& level, const std::string& message, bool urgent);
    std::string getTime();

    std::string filename;
    std::ofstream logFile;
    std::mutex mtx;
    static constexpr size_t MAX_LINES = 2000;
};
