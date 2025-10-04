cmake_minimum_required(VERSION 3.17)

project (inproc-stack)

include_directories(
    ${CMAKE_SOURCE_DIR}/common
    ${CMAKE_SOURCE_DIR}/zmq-proxy
    ${CMAKE_SOURCE_DIR}/conv-stack
    ${CMAKE_SOURCE_DIR}/net-stack
    ${prometheus-cpp_INCLUDE_DIR}
)

add_executable(inproc-stack ../zmq-proxy/Proxy.cpp ../conv-stack/ConvStack.cpp ../net-stack/NetStack.cpp InprocStackMain.cpp)
target_link_libraries( inproc-stack common Threads::Threads fmt::fmt spdlog::spdlog libzmq-static prometheus-cpp::pull)

# Install targets
install(TARGETS inproc-stack DESTINATION inproc-stack/bin/${BIN_PATH})
