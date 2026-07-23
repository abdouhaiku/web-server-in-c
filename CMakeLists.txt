cmake_minimum_required(VERSION 4.2)
project(web_server_in_c)

set(CMAKE_CXX_STANDARD 14)

add_executable(web_server
        src/webserver.c
        src/utilities.c
        src/server_config.c
        src/router.c
        src/application_routes.c
        src/queue.c)
target_include_directories(web_server PRIVATE include)