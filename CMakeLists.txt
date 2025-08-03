cmake_minimum_required(VERSION 3.5)

project(GAINABLE)

set(CMAKE_CXX_STANDARD 17)##11
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt5 COMPONENTS Widgets REQUIRED)

add_executable(test 
main.cpp   
ds18b20.cpp 
thermostats.cpp
system.cpp
gpioPin.cpp 
)

target_link_libraries(test Qt5::Widgets 
gpiod 
)
