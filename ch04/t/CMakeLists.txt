cmake_minimum_required(VERSION 2.8)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -g -O2 -std=c++11")

add_executable(03_bind_cpu 03_bind_cpu.cpp)
target_link_libraries(03_bind_cpu pthread)

