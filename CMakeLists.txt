cmake_minimum_required(VERSION 3.17)


message( STATUS "CMAKE_SOURCE_DIR=${CMAKE_SOURCE_DIR}" )
message( STATUS "CMAKE_PREFIX_PATH=${CMAKE_PREFIX_PATH}")

project(zmq-playground)

#---------------------------------------------------------------------------------------
# compiler config
#---------------------------------------------------------------------------------------
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" OR "${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
    add_compile_options("-Wall")
    add_compile_options("-Wextra")
    add_compile_options("-Wconversion")
    add_compile_options("-pedantic")
    add_compile_options("-Wfatal-errors")
    
endif()


# fmt library dependency
find_package(fmt CONFIG REQUIRED)

# zmq library dependency for ZeroMQ built from source with CMake support
find_package(ZeroMQ REQUIRED)


# spdlog library dependency
find_package(spdlog CONFIG REQUIRED)

# prometheus-cpp library dependency for exposing metrics
find_package(prometheus-cpp CONFIG REQUIRED)

# yaml-cpp library dependency
find_package(yaml-cpp REQUIRED)

# httplib library dependency
find_package(httplib REQUIRED)

# Threads
find_package(Threads)

message("Building for ${CMAKE_SYSTEM_PROCESSOR}")
message("Installing to ${CMAKE_INSTALL_PREFIX}")

# Build the bin path for the install target by mapping CMAKE_SYSTEM_PROCESSOR
# (x86_64 or aarch64) to the docker builx TARGETPLATFORM (linux/amd64 or
# linux/arm64)
if (${CMAKE_SYSTEM_PROCESSOR} STREQUAL "x86_64")
    set(BIN_PATH "linux/amd64")
elseif (${CMAKE_SYSTEM_PROCESSOR} STREQUAL "aarch64")
    set(BIN_PATH "linux/arm64")
endif()

add_subdirectory(common)
add_subdirectory(zmq-proxy)
add_subdirectory(zmq-stack)
add_subdirectory(net-stack)
add_subdirectory(conv-stack)
add_subdirectory(inproc-stack)
add_subdirectory(net-driver)
# add_subdirectory(pub-sub-stack)
