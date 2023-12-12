cmake_minimum_required(VERSION 3.22.1)
project(AuthenticationService C)

set(CMAKE_C_STANDARD 11)

find_package(mongoc-1.0 1.7 REQUIRED)

add_executable(AuthenticationService main.c
        threadsdata.h
        cli.h
        cli.c
        utils.c
        utils.h
        database.h
        database.c
        mongodata.h)

target_link_libraries(AuthenticationService PRIVATE mongo::mongoc_shared)


