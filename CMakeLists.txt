cmake_minimum_required(VERSION 3.22.1)
project(AuthenticationService C)

set(CMAKE_C_STANDARD 11)

SET(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake-modules/" ${CMAKE_MODULE_PATH})

FIND_PACKAGE(OPENSSL REQUIRED)
IF (NOT OPENSSL_FOUND)
    MESSAGE(FATAL_ERROR
            "OpenSSL is not found!")
ENDIF ()
SET(OPENSSL_INCLUDES ${OPENSSL_INCLUDE_DIRS})
SET(OPENSSL_LIBRARIES ${OPENSSL_LIBRARIES})

find_package(CJSON REQUIRED)
if (NOT CJSON_FOUND)
    message(FATAL_ERROR
            "cJSON are not found!")
endif ()
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
        server/server.h
        server/handlers_list.c
        server/header_list.c
        server/query_list.c
        server/request.c
        server/response.c
        sha/sha.h
        sha/sha.c
        hmac/hmac_sha2.h
        hmac/hmac_sha2.c
        server/base64.c
        server/base64.c
        server/base64.c
        server/base64.c
        server/jwt.h
        server/jwt.c
        server/server.c
)
target_link_libraries(AuthenticationService
        PUBLIC
        ${OPENSSL_LIBRARIES}
        ${CJSON_LIBRARY}
        ${CMAKE_THREAD_LIBS_INIT}
)
target_link_libraries(AuthenticationService PRIVATE mongo::mongoc_shared)


add_executable(AuthenticationServiceClient client.c)
target_link_libraries(AuthenticationServiceClient
        ${OPENSSL_LIBRARIES}
)
option(BUILD_TESTS "Build test executables" OFF)

if(BUILD_TESTS)
    add_executable(test_sha sha/test_sha2.c sha/sha.c)
    target_link_libraries(test_sha ${OPENSSL_LIBRARIES})

    add_executable(test_hmac hmac/test_hmac.c hmac/hmac_sha2.c sha/sha.c)
    target_link_libraries(test_hmac ${OPENSSL_LIBRARIES})
endif()




