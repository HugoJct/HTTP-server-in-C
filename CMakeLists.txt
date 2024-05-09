cmake_minimum_required(VERSION 3.0)

set(CMAKE_C_STANDARD 11)

set(This http_server)
set(HEADERS_DIR "src/includes")
set(SOURCES_DIR "src")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra")

project(${This} C)

file(GLOB ${This}_SRC CONFIGURE_DEPENDS
      "${HEADERS_DIR}/*.h"
      "${SOURCES_DIR}/*.c"
)

include_directories(src/includes)

add_executable(${This} ${${This}_SRC})
