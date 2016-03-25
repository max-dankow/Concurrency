cmake_minimum_required(VERSION 3.4)
project(LockFreeStack)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -lgtest -pthread")

set(SOURCE_FILES LockFreeStack.h ../Optional.h)
add_executable(Tests Tests/Tests.cpp ${SOURCE_FILES})

target_link_libraries(Tests gtest)