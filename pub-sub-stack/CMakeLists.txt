cmake_minimum_required(VERSION 3.17)

project (stacks)

include_directories(
    ${CMAKE_SOURCE_DIR}/common
)


find_package(yaml-cpp 0.6.2 REQUIRED)

find_package(httplib REQUIRED)

add_executable(zmq-stack ZmqStackMain.cpp)
target_link_libraries( zmq-stack common zmq Threads::Threads yaml-cpp httplib::httplib)

add_executable(conv-stack ConvStack.cpp ConvStackMain.cpp)
target_link_libraries( conv-stack common zmq Threads::Threads yaml-cpp httplib::httplib)

add_executable(zmq-proxy Proxy.cpp ProxyMain.cpp )
target_link_libraries( zmq-proxy common zmq Threads::Threads yaml-cpp httplib::httplib)

add_executable(net-stack NetStack.cpp NetStackMain.cpp)
target_link_libraries( net-stack common zmq Threads::Threads yaml-cpp httplib::httplib)

add_executable(inproc-stack Proxy.cpp ConvStack.cpp NetStack.cpp InprocStackMain.cpp)
target_link_libraries( inproc-stack common zmq Threads::Threads )




# Install targets
install(TARGETS zmq-stack DESTINATION bin)
install(TARGETS zmq-proxy DESTINATION bin)
install(TARGETS conv-stack DESTINATION bin)
install(TARGETS net-stack DESTINATION bin)
install(TARGETS inproc-stack DESTINATION bin)
