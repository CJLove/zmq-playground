cmake_minimum_required(VERSION 3.17)

project (udp-driver)

include_directories(
    ${CMAKE_SOURCE_DIR}/common
    ${prometheus-cpp_INCLUDE_DIR}
)

add_executable(udp-driver UdpDriver.cpp)
target_link_libraries(udp-driver common Threads::Threads ${ZeroMQ_LIBRARY} ${SODIUM_LIB})

# Install targets
install(TARGETS udp-driver DESTINATION udp-driver/bin/${BIN_PATH})
