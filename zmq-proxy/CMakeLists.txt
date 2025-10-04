cmake_minimum_required(VERSION 3.17)

project (zmq-proxy)

include_directories(
    ${CMAKE_SOURCE_DIR}/common
    ${prometheus-cpp_INCLUDE_DIR}
)

add_executable(zmq-proxy Proxy.cpp ProxyMain.cpp )
target_link_libraries( zmq-proxy PRIVATE common Threads::Threads fmt::fmt spdlog::spdlog libzmq-static yaml-cpp::yaml-cpp httplib::httplib prometheus-cpp::pull )

# Install targets
install(TARGETS zmq-proxy DESTINATION zmq-proxy/bin/${BIN_PATH})
