# Project specifications
cmake_minimum_required(VERSION 2.8.11)
project (Server)

# Compiler flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

# Header files to be included
include_directories(include)

# Specify the source files
file(GLOB SOURCES "src/*.cpp")

#Specify Threads package
find_package(Threads)

# Executable to be created
add_executable(Server ${SOURCES})

# Add threads package
target_link_libraries(Server ${CMAKE_THREAD_LIBS_INIT})
