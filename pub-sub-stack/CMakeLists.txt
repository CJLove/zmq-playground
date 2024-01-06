cmake_minimum_required(VERSION 3.17)

project (stacks)

include_directories(
    ${CMAKE_SOURCE_DIR}/common
    ${prometheus-cpp_INCLUDE_DIR}
)

find_package(yaml-cpp 0.6.2 REQUIRED)

find_package(httplib REQUIRED)

add_executable(zmq-stack ZmqStackMain.cpp)
target_link_libraries( zmq-stack common Threads::Threads yaml-cpp::yaml-cpp httplib::httplib prometheus-cpp::pull ${ZeroMQ_LIBRARY} ${SODIUM_LIB})

add_executable(conv-stack ConvStack.cpp ConvStackMain.cpp)
target_link_libraries( conv-stack common Threads::Threads yaml-cpp::yaml-cpp httplib::httplib prometheus-cpp::pull ${ZeroMQ_LIBRARY} ${SODIUM_LIB})

add_executable(zmq-proxy Proxy.cpp ProxyMain.cpp )
target_link_libraries( zmq-proxy PRIVATE common Threads::Threads yaml-cpp::yaml-cpp httplib::httplib prometheus-cpp::pull ${ZeroMQ_LIBRARY} ${SODIUM_LIB})

add_executable(net-stack NetStack.cpp NetStackMain.cpp)
target_link_libraries( net-stack common Threads::Threads yaml-cpp::yaml-cpp httplib::httplib prometheus-cpp::pull ${ZeroMQ_LIBRARY} ${SODIUM_LIB})

add_executable(inproc-stack Proxy.cpp ConvStack.cpp NetStack.cpp InprocStackMain.cpp)
target_link_libraries( inproc-stack common Threads::Threads prometheus-cpp::pull ${ZeroMQ_LIBRARY} ${SODIUM_LIB})

add_executable(udp-driver UdpDriver.cpp)
target_link_libraries(udp-driver common Threads::Threads ${ZeroMQ_LIBRARY} ${SODIUM_LIB})





# Install targets
install(TARGETS zmq-stack DESTINATION bin/${BIN_PATH})
install(TARGETS zmq-proxy DESTINATION bin/${BIN_PATH})
install(TARGETS conv-stack DESTINATION bin/${BIN_PATH})
install(TARGETS net-stack DESTINATION bin/${BIN_PATH})
install(TARGETS inproc-stack DESTINATION bin/${BIN_PATH})
install(TARGETS udp-driver DESTINATION bin/${BIN_PATH})
