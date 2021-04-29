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
set(TARGET cli)

protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS ${PROTOBUF_FILES})

file(GLOB_RECURSE SOURCES ${SOURCE_DIR}/System/*.cpp ${SOURCE_DIR}/Network/*.cpp ${SOURCE_DIR}/Crypto/*.cpp  ${CLI_PATH}/src/*.cpp)

include_directories(${CLI_PATH}/include ${CMAKE_CURRENT_BINARY_DIR})

add_executable(${TARGET} ${SOURCES} ${PROTO_SRCS})

target_link_libraries(${TARGET} spdlog::spdlog ${Protobuf_LIBRARIES} zmq ssl crypto secp256k1 pthread dl)