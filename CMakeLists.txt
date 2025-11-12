cmake_minimum_required(VERSION 3.16)
project(DriftSolver)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(SFML 3 COMPONENTS System Window Graphics Audio REQUIRED)

add_executable(DriftSolver
    src/main.cpp
    src/game/Racer.cpp
    src/game/Road.cpp
    src/engine/DriftEngine.cpp
    src/engine/DriftEngine.h
    src/game/Racer.h
    src/game/Road.h
)

target_include_directories(DriftSolver PRIVATE src)
target_link_libraries(DriftSolver PRIVATE SFML::System SFML::Window SFML::Graphics SFML::Audio)