cmake_minimum_required(VERSION 3.12)
project(chip8)
set(CMAKE_CXX_STANDARD 17)

add_library(cpu src/cpu.cpp)

add_executable(chip8 src/main.cpp)
target_link_libraries(chip8 SDL2 cpu)
