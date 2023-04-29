cmake_minimum_required(VERSION 2.8)

project("ECS" c)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -g -O2")

add_executable(alarm alarm.c)
add_executable(counter counter.c)

