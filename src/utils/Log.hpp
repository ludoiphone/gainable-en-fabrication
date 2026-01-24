#pragma once
#include <log4cxx/logger.h>
#include <log4cxx/propertyconfigurator.h>

namespace Log
{
    inline log4cxx::LoggerPtr logger;

    inline void init(const std::string& configFile)
    {
        log4cxx::PropertyConfigurator::configure(configFile);
        logger = log4cxx::Logger::getLogger("PAC");
    }
}

#define LOG_INFO(msg)  LOG4CXX_INFO(Log::logger, msg)
#define LOG_WARN(msg)  LOG4CXX_WARN(Log::logger, msg)
#define LOG_ERROR(msg) LOG4CXX_ERROR(Log::logger, msg)
#define LOG_DEBUG(msg) LOG4CXX_DEBUG(Log::logger, msg)
