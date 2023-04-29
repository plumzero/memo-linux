
cmake_minimum_required(VERSION 3.0.2)

set(CMAKE_CXX_STANDARD 11)

include_directories(.)

file(GLOB files "${CMAKE_CURRENT_SOURCE_DIR}/*.cc")
foreach(file ${files})
    get_filename_component(name ${file} NAME_WE)
    add_executable(${name} ${file})
endforeach()
