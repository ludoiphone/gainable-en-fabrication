cmake_minimum_required(VERSION 3.5)

# liste des sources
set(SOURCES
    main.cpp
    utils/Logger.cpp
    hardware/NTCReader.cpp
    hardware/SwitchSensor.cpp
    hardware/Relay.cpp
    hardware/Chauffage.cpp
    hardware/Froid.cpp
    logic/Consignes.cpp
    logic/Temporisations.cpp
    logic/newsfetcher.cpp
    core/PacController.cpp
    ui/MainWindow.cpp
    ui/DateTime.cpp
    ui/NewsBanner.cpp
    ui/TemperatureWidget.cpp
    ui/PacControl.cpp
    ui/Gifs.cpp
    ui/Photos.cpp
    ui/appwindow.cpp
    ui/affichecons.cpp
    ui/modifcons.cpp
)

# liste des headers (facultative mais pratique)
set(HEADERS
    utils/Logger.hpp
    hardware/NTCReader.hpp
    hardware/SwitchSensor.hpp
    hardware/Relay.hpp
    hardware/Chauffage.hpp
    hardware/Froid.hpp
    logic/Consignes.hpp
    logic/Temporisations.hpp
    logic/newsfetcher.hpp
    core/PacController.hpp
    ui/MainWindow.hpp
    ui/DateTime.hpp
    ui/NewsBanner.hpp
    ui/TemperatureWidget.hpp
    ui/PacControl.hpp
    ui/Gifs.hpp
    ui/Photos.hpp
    ui/appwindow.hpp
    ui/affichecons.hpp
    ui/modifcons.hpp
)

# active automoc pour Qt (génère automatiquement les moc si Q_OBJECT est présent)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC OFF)
set(CMAKE_AUTOUIC OFF)

find_package(Qt5 COMPONENTS Widgets WebEngineWidgets Network REQUIRED)

add_executable(gainable-qt
    ${SOURCES}
    ${HEADERS}
)

# include dir (pour #include "../..." dans ton code on peut ajuster)
target_include_directories(gainable-qt PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# lier les bibliothèques nécessaires :
# - Qt Widgets
# - pthread (threading)
# - wiringPi (si tu utilises wiringPi pour les GPIO)
target_link_libraries(gainable-qt PRIVATE
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
    target_link_libraries(gainable-qt PRIVATE stdc++fs)
endif()

# Options de compilation utiles
target_compile_options(gainable-qt PRIVATE -Wall -Wextra -Wpedantic)
