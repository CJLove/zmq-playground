cmake_minimum_required(VERSION 3.17)

project (zmq-stack)

include_directories(
    ${CMAKE_SOURCE_DIR}/common
    ${prometheus-cpp_INCLUDE_DIR}
)


add_executable(zmq-stack ZmqStackMain.cpp)
target_link_libraries( zmq-stack common Threads::Threads yaml-cpp::yaml-cpp httplib::httplib prometheus-cpp::pull ${ZeroMQ_LIBRARY} ${SODIUM_LIB})

# Install targets
install(TARGETS zmq-stack DESTINATION zmq-stack/bin/${BIN_PATH})

