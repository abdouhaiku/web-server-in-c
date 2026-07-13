cmake_minimum_required(VERSION 4.2)
project(web_server_in_c)

set(CMAKE_CXX_STANDARD 14)


add_executable(web_server webserver.c
        utilities.h
        utilities.c)
