cmake_minimum_required(VERSION 3.25)
project(ISAM)

set(CMAKE_CXX_STANDARD 17)

add_executable(generate_file src/generate_file.cpp inc/ISAM.hpp inc/record.hpp inc/utils.hpp)
add_executable(read_sorted_file src/read_sorted_file.cpp inc/ISAM.hpp inc/record.hpp inc/utils.hpp)
add_executable(isam_create_index src/isam_create_index.cpp inc/ISAM.hpp inc/record.hpp inc/utils.hpp)

add_executable(isam_search_record src/isam_search_record.cpp inc/ISAM.hpp inc/record.hpp inc/utils.hpp)
add_executable(isam_search_all src/isam_search_all.cpp inc/ISAM.hpp inc/record.hpp inc/utils.hpp)
add_executable(isam_range_search src/isam_range_search.cpp inc/ISAM.hpp inc/record.hpp inc/utils.hpp)

add_executable(isam_insert_record src/isam_insert_record.cpp inc/ISAM.hpp inc/record.hpp inc/utils.hpp)
add_executable(isam src/main.cpp inc/ISAM.hpp inc/record.hpp src/main.cpp inc/utils.hpp)
