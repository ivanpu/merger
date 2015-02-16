cmake_minimum_required(VERSION 3.1 FATAL_ERROR)

project(Merger CXX)

find_package(Boost REQUIRED COMPONENTS program_options date_time filesystem system)

add_subdirectory(src)
