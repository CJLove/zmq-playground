cmake_minimum_required(VERSION 3.17)

project(common)

set (common_SRC
    Publisher.cpp
    ZmqStack.cpp
)

include_directories(
    .
    ${prometheus-cpp_INCLUDE_DIR}
)

add_library( common STATIC ${common_SRC})

target_link_libraries( common zmq Threads::Threads )
