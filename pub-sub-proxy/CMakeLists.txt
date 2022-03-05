cmake_minimum_required(VERSION 3.17)

project (pub-sub-proxy)

add_executable(zmq_proxy zmq_proxy.cpp common.cpp)

target_link_libraries( zmq_proxy zmq Threads::Threads)

add_executable(zmq_producer zmq_producer.cpp common.cpp)

target_link_libraries( zmq_producer zmq Threads::Threads)

add_executable(zmq_subscriber zmq_subscriber.cpp common.cpp)

target_link_libraries( zmq_subscriber zmq Threads::Threads)