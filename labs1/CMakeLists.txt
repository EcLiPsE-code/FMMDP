cmake_minimum_required(VERSION 3.17)
project(labs)

set(CMAKE_CXX_STANDARD 20)

add_executable(labs main.cpp src/headers/matrix.h src/headers/LLTCalculator.h src/LLTCalculator.cpp)