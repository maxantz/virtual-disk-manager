cmake_minimum_required( VERSION 3.16 )

find_package(GTest REQUIRED)
find_package(Crc32c REQUIRED)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

enable_testing()

include_directories("../../include")

add_executable(
  vhdx_test
  ../../include/vhdx.h
  ../../src/vhdx.cpp
  main.cpp
)

target_link_libraries(
  vhdx_test
  GTest::gtest_main
  Crc32c::crc32c
)
