cmake_minimum_required(VERSION 3.5)
project(CPU)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pthread")

set(SOURCE_FILES main.cpp CachePingPong.h)
add_executable(CPU ${SOURCE_FILES})