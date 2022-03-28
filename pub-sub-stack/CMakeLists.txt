cmake_minimum_required(VERSION 3.17)

project (pub-sub-stack)

add_executable(pub-sub-stack ZmqStack.cpp Publisher.cpp ZmqStackMain.cpp)

target_link_libraries( pub-sub-stack zmq Threads::Threads )

add_executable(inproc-stack ZmqStack.cpp Publisher.cpp InprocStackMain.cpp)

target_link_libraries( inproc-stack zmq Threads::Threads )

install(TARGETS pub-sub-stack DESTINATION bin)
install(TARGETS inproc-stack DESTINATION bin)
