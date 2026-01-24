cmake_minimum_required(VERSION 3.10)

# liste des sources
set(SOURCES
    main.cpp
    hardware/NTCReader.cpp
    hardware/Input.cpp
    hardware/SwitchManager.cpp
    hardware/Output.cpp
    hardware/RelayManager.cpp
    hardware/VentilateurInterieur.cpp
    hardware/VentilateurExterieur.cpp
    hardware/Chauffage.cpp
    hardware/Froid.cpp
    logic/Consignes.cpp
    logic/Temporisations.cpp
    core/PacController.cpp
    core/PacStatusUI.cpp
    #core/PacStatusService.cpp
    core/FiltreManager.cpp
    core/NTCManager.cpp
    Qt/MainWindow.cpp
    Qt/DateTime.cpp
    Qt/NewsBanner.cpp
    Qt/TemperatureWidget.cpp
    Qt/WeatherManager.cpp
    Qt/WeatherWidget.cpp
    Qt/PacControl.cpp
    Qt/Gifs.cpp
    Qt/Photos.cpp
    Qt/AppWindow.cpp
    Qt/AfficheCons.cpp
    Qt/ModifCons.cpp
    Qt/ServiceTest.cpp
    Qt/RemoteCommandServer.cpp
)

# liste des headers (facultative mais pratique)
set(HEADERS
    utils/Log.hpp
    hardware/NTCReader.hpp
    hardware/Input.hpp
    hardware/SwitchManager.hpp
    hardware/Output.hpp
    hardware/RelayManager.hpp
    hardware/VentilateurInterieur.hpp
    hardware/VentilateurExterieur.hpp
    hardware/Chauffage.hpp
    hardware/Froid.hpp
    logic/Consignes.hpp
    logic/Temporisations.hpp
    core/PacController.hpp
    core/PacStatusUI.hpp
    #core/PacStatusService.hpp
    core/FiltreManager.hpp
    core/NTCManager.hpp
    Qt/MainWindow.hpp
    Qt/NewsBanner.hpp
    Qt/TemperatureWidget.hpp
    Qt/WeatherManager.hpp
    Qt/WeatherWidget.hpp
    Qt/DateTime.hpp
    Qt/Gifs.hpp
    Qt/PacControl.hpp
    Qt/Photos.hpp
    Qt/AppWindow.hpp
    Qt/AfficheCons.hpp
    Qt/ModifCons.hpp
    Qt/ServiceTest.hpp
    Qt/RemoteCommandServer.hpp
)

# active automoc pour Qt (génère automatiquement les moc si Q_OBJECT est présent)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC OFF)
set(CMAKE_AUTOUIC OFF)

find_package(Qt5 5.12 COMPONENTS Widgets WebEngineWidgets Network REQUIRED)

find_path(LOG4CXX_INCLUDE_DIR log4cxx/logger.h
    PATHS /usr/local/include /usr/include
)

find_library(LOG4CXX_LIBRARY log4cxx
    PATHS /usr/local/lib /usr/lib
)

if(NOT LOG4CXX_INCLUDE_DIR OR NOT LOG4CXX_LIBRARY)
    message(FATAL_ERROR "Log4cxx introuvable ! Vérifiez l'installation.")
endif()

message(STATUS "Log4cxx include: ${LOG4CXX_INCLUDE_DIR}")
message(STATUS "Log4cxx lib: ${LOG4CXX_LIBRARY}")

add_executable(pac-qt
    ${SOURCES}
    ${HEADERS}
)

# include dir (pour #include)
target_include_directories(pac-qt PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${LOG4CXX_INCLUDE_DIR}
)

# lier les bibliothèques nécessaires :
# - Qt Widgets
# - pthread (threading)
# - wiringPi (les GPIO)
target_link_libraries(pac-qt PRIVATE
    Qt5::Widgets
    Qt5::Network
    Qt5::WebEngineWidgets
    pthread
    wiringPi
    ${LOG4CXX_LIBRARY}
)

# Problème éventuel avec std::filesystem sur vieux toolchains :
# sur certains anciennes versions de GCC il faut linker explicitement stdc++fs.
include(CheckCXXCompilerFlag)
check_cxx_compiler_flag("-lstdc++fs" HAS_STDCXXFS_LIB)
if(HAS_STDCXXFS_LIB)
    target_link_libraries(pac-qt PRIVATE stdc++fs)
endif()

# Options de compilation utiles
target_compile_options(pac-qt PRIVATE -Wall -Wextra -Wpedantic)
