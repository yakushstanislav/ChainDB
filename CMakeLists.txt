# Copyright (c) 2021 Stanislav Yakush (st.yakush@yandex.ru)
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

cmake_minimum_required(VERSION 3.16)

project(ChainDB VERSION 1.0)

set(TARGET ${PROJECT_NAME})

option(BUILD_DEBUG "Build for debug" OFF)
option(BUILD_CLI "Build CLI" OFF)
option(BUILD_TESTS "Build tests" OFF)

find_package(Protobuf REQUIRED)
find_package(spdlog REQUIRED)

include(cmake/Config.cmake)

set(CMAKE_CXX_COMPILER "/usr/bin/g++")
set(CMAKE_CXX_FLAGS "-std=c++2a -O2 -pedantic -Wall -Wextra")

if (BUILD_DEBUG)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ggdb")
    add_definitions(-D_DEBUG_MODE)
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s")
endif()

set(CMAKE_DIR ${PROJECT_SOURCE_DIR}/cmake)
set(SOURCE_DIR ${PROJECT_SOURCE_DIR}/src)
set(INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include)
set(PROTOBUF_DIR ${PROJECT_SOURCE_DIR}/proto)
set(TESTS_PATH ${PROJECT_SOURCE_DIR}/tests)
set(CLI_PATH ${PROJECT_SOURCE_DIR}/cli)

file(GLOB_RECURSE SOURCES ${SOURCE_DIR}/*.cpp)
file(GLOB_RECURSE PROTOBUF_FILES ${PROTOBUF_DIR}/*.proto)

protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS ${PROTOBUF_FILES})

include_directories(${SOURCE_DIR} ${INCLUDE_DIR} ${CMAKE_CURRENT_BINARY_DIR})

add_executable(${TARGET} ${SOURCES} ${PROTO_SRCS})
target_link_libraries(${TARGET} spdlog::spdlog zmq ssl crypto secp256k1 leveldb rocksdb ${Protobuf_LIBRARIES} pthread dl)

if (BUILD_CLI)
    add_subdirectory(${CLI_PATH})
endif()

if (BUILD_TESTS)
    add_subdirectory(${TESTS_PATH})
endif()