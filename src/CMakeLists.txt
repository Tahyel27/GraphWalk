add_executable(GraphWalk main.cpp)
target_include_directories(GraphWalk PUBLIC ${PROJECT_SOURCE_DIR}/include)

add_library(GraphEngine Graph.cpp)
target_include_directories(GraphEngine PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(GraphWalk PUBLIC GraphEngine)

add_library(MCSimulation MCSimulation.cpp)
target_include_directories(MCSimulation PUBLIC ${CMAKE_CURRENT_LIST_DIR}/../include)
target_link_libraries(GraphWalk PUBLIC MCSimulation)

add_library(PostProcessor PostProcessor.cpp)
target_include_directories(PostProcessor PUBLIC ${CMAKE_CURRENT_LIST_DIR}/../include)
target_link_libraries(GraphWalk PUBLIC PostProcessor)