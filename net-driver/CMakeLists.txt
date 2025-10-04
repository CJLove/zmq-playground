cmake_minimum_required(VERSION 3.17)

project (net-driver)

include_directories(
    ${CMAKE_SOURCE_DIR}/common
    ${prometheus-cpp_INCLUDE_DIR}
)

add_executable(net-driver NetDriver.cpp)
target_link_libraries(net-driver common Threads::Threads fmt::fmt spdlog::spdlog prometheus-cpp::pull libzmq-static)

# Install targets
install(TARGETS net-driver DESTINATION net-driver/bin/${BIN_PATH})
