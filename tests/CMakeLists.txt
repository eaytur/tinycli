cmake_minimum_required(VERSION 3.22)
project(tinycli_tests C)

set(CMAKE_C_STANDARD 11)

add_executable(test_cli
    test_cli.c
    ../src/cli.c
)

target_include_directories(test_cli PRIVATE
    ../inc
)

enable_testing()
add_test(NAME test_cli COMMAND test_cli)
