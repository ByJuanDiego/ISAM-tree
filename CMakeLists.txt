cmake_minimum_required(VERSION 3.25)
project(ISAM)

set(CMAKE_CXX_STANDARD 17)

add_executable(sparse_index_file main.cpp ISAM.hpp record.hpp)
