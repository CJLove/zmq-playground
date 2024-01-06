cmake_minimum_required(VERSION 3.17)

project (net-stack)

include_directories(
    ${CMAKE_SOURCE_DIR}/common
    ${prometheus-cpp_INCLUDE_DIR}
)

add_executable(net-stack NetStack.cpp NetStackMain.cpp)
target_link_libraries( net-stack common Threads::Threads yaml-cpp::yaml-cpp httplib::httplib prometheus-cpp::pull ${ZeroMQ_LIBRARY} ${SODIUM_LIB})

# Install targets
install(TARGETS net-stack DESTINATION net-stack/bin/${BIN_PATH})

