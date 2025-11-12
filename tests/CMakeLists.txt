include_directories(${CMAKE_PROJECT_SOURCE_DIR}/src)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../include)

add_executable(test_graph test_graph.cpp)
target_link_libraries(test_graph GraphEngine)
add_test(NAME GraphTest COMMAND test_graph)