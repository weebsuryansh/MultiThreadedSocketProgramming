cmake_minimum_required(VERSION 3.29)
project(CNSockets C)

set(CMAKE_C_STANDARD 23)

add_executable(CNSockets server.c
        client.c)
