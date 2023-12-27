cmake_minimum_required(VERSION 3.22.1)
project(AuthenticationService C)

set(CMAKE_C_STANDARD 11)

find_package(mongoc-1.0 1.7 REQUIRED)
add_executable(AuthenticationService main.c
        threadsdata.h
        cli/cli.h
        cli/cli.c
        utils/utils.c
        utils/utils.h
        database/database.h
        database/database.c
        database/mongodata.h
)

add_executable(AuthenticationServiceClient client.c)


target_link_libraries(AuthenticationService PRIVATE mongo::mongoc_shared)





