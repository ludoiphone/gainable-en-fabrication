cmake_minimum_required(VERSION 3.10)

# liste des sources
set(SOURCES
    main.cpp
    hardware/NTCReader.cpp
    hardware/SwitchSensor.cpp
    hardware/Relay.cpp
    hardware/Chauffage.cpp
    hardware/Froid.cpp
    logic/Consignes.cpp
    logic/Temporisations.cpp
    core/PacController.cpp
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
)

# liste des headers (facultative mais pratique)
set(HEADERS
    hardware/NTCReader.hpp
    hardware/SwitchSensor.hpp
    hardware/Relay.hpp
    hardware/Chauffage.hpp
    hardware/Froid.hpp
    logic/Consignes.hpp
    logic/Temporisations.hpp
    core/PacController.hpp
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
)

# active automoc pour Qt (génère automatiquement les moc si Q_OBJECT est présent)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC OFF)
set(CMAKE_AUTOUIC OFF)

find_package(Qt5 5.12 COMPONENTS Widgets WebEngineWidgets Network REQUIRED)

add_executable(pac-qt
    ${SOURCES}
    ${HEADERS}
)

# include dir (pour #include)
target_include_directories(pac-qt PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}
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
