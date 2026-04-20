cmake_minimum_required( VERSION 3.16 )
find_package(GTest REQUIRED)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

enable_testing()

include_directories("../../include")

add_executable(
    arguments_parser_test
    ../../include/arguments_parser.h
    ../../src/arguments_parser.cpp
    main.cpp
)

target_link_libraries(
    arguments_parser_test
    GTest::gtest_main
)

gtest_discover_tests( arguments_parser_test )
