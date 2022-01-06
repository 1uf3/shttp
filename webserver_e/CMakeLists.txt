cmake_minimum_required(VERSION 3.10)
project(webserver_d)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS "-pthread")

add_executable(server src/server.c)

