cmake_minimum_required(VERSION 3.17)

project (pub-sub-stack)

add_executable(pub-sub-stack ZmqStack.cpp Publisher.cpp main.cpp)

target_link_libraries( pub-sub-stack zmq Threads::Threads )

install(TARGETS pub-sub-stack DESTINATION bin)
