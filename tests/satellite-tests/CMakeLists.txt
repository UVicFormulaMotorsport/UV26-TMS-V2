cmake_minimum_required(VERSION 3.22)

project(mux_mapping_tests C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

enable_testing()

add_executable(test_mux_mapping
    test_mux_mapping.c
    ../../firmware/satellite/app/mux/mux_mapping.c
)

target_include_directories(test_mux_mapping PRIVATE
    ../../firmware/satellite/app/mux
)

add_test(NAME mux_mapping COMMAND test_mux_mapping)