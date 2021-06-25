cmake_minimum_required(VERSION 3.16)
project(spo C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_FLAGS "-pthread")

find_package(PkgConfig REQUIRED)

pkg_check_modules(JSONC REQUIRED IMPORTED_TARGET json-c)
pkg_check_modules (BSON REQUIRED IMPORTED_TARGET libbson-1.0)

add_executable(client client/client.c parser/parser.h util/util.c mt.h)
add_executable(server server/server.c parser/parser.c api/api.c util/util.c mt.h)
target_link_libraries(client PRIVATE PkgConfig::JSONC)
target_link_libraries(server PRIVATE PkgConfig::JSONC)
target_link_libraries (server PRIVATE PkgConfig::BSON)
target_link_libraries (client PRIVATE PkgConfig::BSON)