cmake_minimum_required(VERSION 3.10)
project(server_thread C)

set(CMAKE_C_STANDARD 11)
find_package(Threads REQUIRED)
add_executable(server_thread server.c)
target_link_libraries(server_thread ${CMAKE_THREAD_LIBS_INIT})