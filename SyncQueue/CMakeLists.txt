cmake_minimum_required(VERSION 3.3)
project(SyncQueue)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES main.cpp SyncQueue.cpp SyncQueue.h)
add_executable(SyncQueue ${SOURCE_FILES})