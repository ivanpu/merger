cmake_minimum_required(VERSION 3.1 FATAL_ERROR)

project(Merger CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

## warnings
if(MSVC)
    add_compile_options(/W4)
elseif(CMAKE_COMPILER_IS_GNUCXX)
    add_compile_options(-Wall -Wextra)
endif()

## Boost
find_package(Boost 1.44 REQUIRED COMPONENTS program_options date_time filesystem system)
add_definitions(-DBOOST_FILESYSTEM_NO_DEPRECATED)

## sources
add_subdirectory(src)
