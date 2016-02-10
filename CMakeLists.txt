cmake_minimum_required(VERSION 3.0)
project(simpson)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")
SET(CMAKE_C_COMPILER mpicc)

set(SOURCE_FILES maestro.c esclavo.c)
add_executable(maestro maestro.c)
add_executable(esclavo esclavo.c)