
cmake_minimum_required(VERSION 2.8)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -O0 -Wall -Wextra -W")

file(GLOB files "${CMAKE_CURRENT_SOURCE_DIR}/04_*.c")
foreach(file ${files})
    get_filename_component(name ${file} NAME_WE)
    add_executable(${name} ${file})
endforeach()

# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -O2")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -O3")

#
# 依赖
#

find_library(protobuf_LIBRARY protobuf HINTS "/usr/local/protobuf/lib")

find_path(protobuf_INCLUDE google/protobuf/message.h HINTS "/usr/local/protobuf/include")

if(protobuf_LIBRARY AND protobuf_INCLUDE)
    set(PROTOBUF_FOUND TRUE)
    
    message(STATUS "${Green}Found protobuf library at: ${protobuf_LIBRARY}${Reset}")
    message(STATUS "${Green}Found protobuf include at: ${protobuf_INCLUDE}${Reset}")
else()
    message(FATAL_ERROR "${Red}Failed to locate protobuf dependency.${Reset}")
endif()

include_directories(${protobuf_INCLUDE})
get_filename_component(protobuf_LIBRARY_PATH ${protobuf_LIBRARY} PATH)
get_filename_component(protobuf_LIBRARY_NAME ${protobuf_LIBRARY} NAME_WE)
string(REGEX REPLACE "^lib" "" protobuf_LIBRARY_NAME ${protobuf_LIBRARY_NAME})

link_directories(${protobuf_LIBRARY_PATH})

include_directories(.)

#
# 简单使用
#
file(GLOB files "${CMAKE_CURRENT_SOURCE_DIR}/05_01*.cpp")
foreach(file ${files})
    get_filename_component(name ${file} NAME_WE)
    add_executable(${name} ${file})
    target_link_libraries(${name} rt)
endforeach()

#
# 数组测试
#
file(GLOB files "${CMAKE_CURRENT_SOURCE_DIR}/05_02*.cpp")
foreach(file ${files})
    get_filename_component(name ${file} NAME_WE)
    add_executable(${name} ${file})
    target_link_libraries(${name} rt)
endforeach()

#
# 测试
#
# protoc --cpp_out=. struct.proto
#
file(GLOB files "${CMAKE_CURRENT_SOURCE_DIR}/05_03*.cpp")
foreach(file ${files})
    get_filename_component(name ${file} NAME_WE)
    add_executable(${name} ${file} struct.pb.cc)
    target_link_libraries(${name} rt pthread)
endforeach()
