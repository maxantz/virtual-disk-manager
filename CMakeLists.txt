cmake_minimum_required( VERSION 3.16 )

project( vdm LANGUAGES CXX )

find_package(Crc32c REQUIRED)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE SOURCE_FILES
    "${PROJECT_SOURCE_DIR}/src/main.cpp"
    "${PROJECT_SOURCE_DIR}/src/arguments_parser.cpp"
    "${PROJECT_SOURCE_DIR}/src/vhdx.cpp"
)

file(GLOB_RECURSE HEADER_FILES
    "${PROJECT_SOURCE_DIR}/include/arguments_parser.h"
    "${PROJECT_SOURCE_DIR}/include/vhdx.h"
)

include_directories("${PROJECT_SOURCE_DIR}/include")

add_executable( vdm
    ${SOURCE_FILES}
    ${HEADER_FILES}
)

target_link_libraries( vdm
    Crc32c::crc32c
)

add_subdirectory( tests )
