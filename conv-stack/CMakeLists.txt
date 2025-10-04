cmake_minimum_required(VERSION 3.17)

project (conv-stack)

include_directories(
    ${CMAKE_SOURCE_DIR}/common
    ${prometheus-cpp_INCLUDE_DIR}
)

add_executable(conv-stack ConvStack.cpp ConvStackMain.cpp)
target_link_libraries( conv-stack common Threads::Threads fmt::fmt spdlog::spdlog libzmq-static yaml-cpp::yaml-cpp httplib::httplib prometheus-cpp::pull)

# Install targets
install(TARGETS conv-stack DESTINATION conv-stack/bin/${BIN_PATH})

