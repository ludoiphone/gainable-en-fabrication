cmake_minimum_required(VERSION 3.5)
project(gainable-qt LANGUAGES CXX)

# C++17 requis (std::filesystem)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Trouver Qt (Widgets)
find_package(Qt5 COMPONENTS Widgets REQUIRED)

# Sous-dossier src contient la cible
add_subdirectory(src)
