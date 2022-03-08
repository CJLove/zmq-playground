cmake_minimum_required(VERSION 3.17)

project (pub-sub)

add_executable(zmq_pub zmq_pub.cpp common.cpp)

target_link_libraries( zmq_pub zmq Threads::Threads)

add_executable(zmq_sub zmq_sub.cpp common.cpp)

target_link_libraries( zmq_sub zmq Threads::Threads)

install(TARGETS zmq_pub DESTINATION bin)
install(TARGETS zmq_sub DESTINATION bin)
