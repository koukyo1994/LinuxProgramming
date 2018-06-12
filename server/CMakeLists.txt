cmake_minimum_required(VERSION 3.10)
project(server C)

set(CMAKE_C_STANDARD 11)
find_package(Threads REQUIRED)
add_executable(server server.c)
target_link_libraries(server ${CMAKE_THREAD_LIBS_INIT})