cmake_minimum_required(VERSION 3.10)
project(webserver_a)
set(CMAKE_C_STANDARD 11)

add_executable(client src/client.c)
add_executable(server src/server.c)

