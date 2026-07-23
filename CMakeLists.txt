cmake_minimum_required(VERSION 4.2)
project(web_server_in_c)

set(CMAKE_CXX_STANDARD 14)


add_executable(web_server webserver.c
        utilities.h
        utilities.c
        server_config.h
        server_config.c
        router.h
        router.c
        application_routes.h
        application_routes.c
        queue.h
        queue.c)

add_executable(test test.c
        utilities.h
        utilities.c
        server_config.h
        server_config.c
        router.h
        router.c
        application_routes.h
        application_routes.c)
