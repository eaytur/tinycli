cmake_minimum_required(VERSION 3.22)
project(tinycli C)

set(CMAKE_C_STANDARD 11)

add_library(tinycli STATIC
    src/cli.c
)

target_include_directories(tinycli PUBLIC
    inc
)
