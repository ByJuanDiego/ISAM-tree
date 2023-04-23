cmake_minimum_required(VERSION 3.25)
project(ISAM)

set(CMAKE_CXX_STANDARD 17)


add_executable(generate_file src/generate_file.cpp inc/ISAM.hpp inc/record.hpp)
add_executable(read_sorted_file src/read_sorted_file.cpp inc/ISAM.hpp inc/record.hpp)
add_executable(isam_create_index src/isam_create_index.cpp inc/ISAM.hpp inc/record.hpp)
add_executable(isam_search_index src/isam_search_index.cpp inc/ISAM.hpp inc/record.hpp src/isam_search_index.cpp)
