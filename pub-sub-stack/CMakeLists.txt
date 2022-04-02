cmake_minimum_required(VERSION 3.17)

project (stacks)

add_executable(zmq-stack ZmqStack.cpp Publisher.cpp ZmqStackMain.cpp)
target_link_libraries( zmq-stack zmq Threads::Threads )

add_executable(conv-stack ZmqStack.cpp ConvStack.cpp Publisher.cpp ConvStackMain.cpp)
target_link_libraries( conv-stack zmq Threads::Threads )

add_executable(zmq-proxy Proxy.cpp ProxyMain.cpp )
target_link_libraries( zmq-proxy zmq Threads::Threads )

add_executable(inproc-stack ZmqStack.cpp Publisher.cpp Proxy.cpp InprocStackMain.cpp)
target_link_libraries( inproc-stack zmq Threads::Threads )

# Install targets
install(TARGETS zmq-stack DESTINATION bin)
install(TARGETS zmq-proxy DESTINATION bin)
install(TARGETS conv-stack DESTINATION bin)
install(TARGETS inproc-stack DESTINATION bin)
