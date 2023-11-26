
cmake_minimum_required(VERSION 3.0.2)

project("c test" C)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -W -g -O2")

file(GLOB files "${CMAKE_CURRENT_SOURCE_DIR}/*.c")
foreach(file ${files})
    get_filename_component(name ${file} NAME_WE)
    add_executable(${name} ${file})
endforeach()
