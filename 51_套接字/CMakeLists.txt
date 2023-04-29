
cmake_minimum_required(VERSION 2.6)

project("multiplexing IO test" C)

#
# 全局配置
#
string(REGEX MATCH "GNU" CMAKE_COMPILER_IS_GNU "${CMAKE_C_COMPILER_ID}")
string(REGEX MATCH "Clang" CMAKE_COMPILER_IS_CLANG "${CMAKE_C_COMPILER_ID}")

if (CMAKE_COMPILER_IS_GNU)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -W -Wdeclaration-after-statement -Wwrite-strings \
                       -Wno-declaration-after-statement")
endif()

if (CMAKE_COMPILER_IS_CLANG)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -W -Wdeclaration-after-statement -Wwrite-strings  \
                       -Wpointer-arith -Wimplicit-fallthrough -Wshadow  \
                       -Wno-declaration-after-statement")
endif()

#
# 选项
#
option(BUILD_SOCKET     "socket"    ON)
option(BUILD_SELECT     "select"    ON)
option(BUILD_POLL       "poll"      OFF)
option(BUILD_EPOLL      "epoll"     OFF)

#
# 公共文件
#
include_directories(common)

#
# 项
#
if (BUILD_SOCKET)
    add_subdirectory(01_socket)
endif()

if (BUILD_SELECT)
    add_subdirectory(02_select)
endif()

if (BUILD_POLL)
    add_subdirectory(03_poll)
endif()

if (BUILD_EPOLL)
    add_subdirectory(04_epoll)
endif()
