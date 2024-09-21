cmake_minimum_required(VERSION 3.29)
project(CNSockets C)

set(CMAKE_C_STANDARD 23)

add_executable(Server server.c)
add_executable(Client client.c)
