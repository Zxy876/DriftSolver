cmake_minimum_required(VERSION 3.16)
project(DriftSolver)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(SFML 3 COMPONENTS System Window Graphics Audio REQUIRED)

add_executable(DriftSolver
    src/main.cpp
    src/engine/DriftEngine.cpp
    src/game/Road.cpp
    src/game/Racer.cpp
)

target_include_directories(DriftSolver PRIVATE
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/src/engine
    ${CMAKE_SOURCE_DIR}/src/game
)

target_link_libraries(DriftSolver PRIVATE
    SFML::System SFML::Window SFML::Graphics SFML::Audio
)