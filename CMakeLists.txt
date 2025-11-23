cmake_minimum_required(VERSION 3.15)

project(GraphWalk)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(src)

enable_testing()

add_subdirectory(tests)