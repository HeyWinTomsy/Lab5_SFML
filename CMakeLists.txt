cmake_minimum_required(VERSION 3.25)

project(Lab5_SFML LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(SFML 3 COMPONENTS Graphics Window System CONFIG REQUIRED)

add_executable(Lab5_SFML main.cpp)

target_link_libraries(Lab5_SFML PRIVATE
        SFML::Graphics
        SFML::Window
        SFML::System
)
