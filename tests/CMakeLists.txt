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

set(TARGET runtest)

include_directories(${TESTS_PATH})

protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS ${PROTOBUF_FILES})

file(GLOB_RECURSE SOURCES
    ${SOURCE_DIR}/Handler.cpp
    ${SOURCE_DIR}/System/*.cpp
    ${SOURCE_DIR}/Network/*.cpp
    ${SOURCE_DIR}/Crypto/*.cpp
    ${SOURCE_DIR}/Storage/*.cpp)

file(GLOB_RECURSE TEST_SOURCES ${TESTS_PATH}/*.cpp)

add_definitions(-D_TEST)

add_executable(${TARGET} ${PROTO_SRCS} ${SOURCES} ${TEST_SOURCES})

target_link_libraries(${TARGET} gtest spdlog::spdlog zmq ssl crypto secp256k1 leveldb rocksdb ${Protobuf_LIBRARIES} pthread dl)