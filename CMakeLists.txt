cmake_minimum_required(VERSION 3.25)
project(ISAM)

set(CMAKE_CXX_STANDARD 17)

add_executable(range_search src/isam_range_search.cpp)
add_executable(equality_search src/isam_equality_search.cpp)
add_executable(remove src/isam_remove.cpp)
