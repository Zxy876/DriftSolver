cmake_minimum_required(VERSION 3.16)
project(DriftSolver)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# SFML 3.x 版本
find_package(SFML 3 COMPONENTS System Window Graphics Audio REQUIRED)

file(GLOB SRC_FILES CONFIGURE_DEPENDS
    src/*.cpp
    src/*/*.cpp
)

add_executable(DriftSolver ${SRC_FILES})
target_include_directories(DriftSolver PRIVATE src)
target_link_libraries(DriftSolver PRIVATE SFML::System SFML::Window SFML::Graphics SFML::Audio)