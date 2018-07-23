cmake_minimum_required(VERSION 3.10)
project(Chat C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS "-Wall")

add_executable(Chat_server server.c)