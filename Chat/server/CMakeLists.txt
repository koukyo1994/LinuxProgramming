cmake_minimum_required(VERSION 3.10)
project(Chat C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS "-Wall")
find_package(Threads)

add_executable(Chat_server server.c)
add_executable(Chat_server2 server2.c)
target_link_libraries(Chat_server ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(Chat_server2 ${CMAKE_THREAD_LIBS_INIT})