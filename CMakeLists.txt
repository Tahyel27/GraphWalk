cmake_minimum_required(VERSION 3.15)

option(GUI "Use GUI" ON)

project(GraphWalk)

set(CMAKE_CXX_STANDARD 20)

find_package(OpenMP REQUIRED)

add_subdirectory(src)

file(COPY_FILE "${CMAKE_CURRENT_LIST_DIR}/cfg_files/config.txt" "${CMAKE_CURRENT_LIST_DIR}/build/src/config.txt")
file(COPY_FILE "${CMAKE_CURRENT_LIST_DIR}/cfg_files/testgrph.txt" "${CMAKE_CURRENT_LIST_DIR}/build/src/testgrph.txt")

enable_testing()

add_subdirectory(tests)