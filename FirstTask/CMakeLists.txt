cmake_minimum_required(VERSION 3.3)
project(MultuThreading)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pthread")

set(SOURCE_FILES main.cpp)
add_executable(MultuThreading ${SOURCE_FILES})