cmake_minimum_required(VERSION 3.3)
project(SyncQueue)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -lgtest -pthread")

set(SOURCE_FILES SyncQueue.h Optional.h Adapter.h)
add_executable(SyncQueue main.cpp ${SOURCE_FILES})
add_executable(Tests Tests/Tests.cpp ${SOURCE_FILES})
target_link_libraries(Tests gtest)