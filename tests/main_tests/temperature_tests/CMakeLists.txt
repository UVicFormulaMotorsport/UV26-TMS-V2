cmake_minimum_required(VERSION 3.22)

project(temperature_tests C)

set(CMAKE_C_STANDARD 11)

add_executable(test_temperature
    test_temperature.c
    ../../../firmware/main/app/temperature/temperature.c
)

target_include_directories(test_temperature PRIVATE
    ../../../firmware/main/app/temperature
)