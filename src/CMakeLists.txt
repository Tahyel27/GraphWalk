add_executable(GraphWalk main.cpp)
target_include_directories(GraphWalk PUBLIC ${PROJECT_SOURCE_DIR}/include)

add_library(Utilities Utilities.cpp)
target_include_directories(Utilities PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(GraphWalk PUBLIC Utilities)

add_library(GraphEngine Graph.cpp)
target_include_directories(GraphEngine PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(GraphWalk PUBLIC GraphEngine)

add_library(MCSimulation MCSimulation.cpp)
target_include_directories(MCSimulation PUBLIC ${CMAKE_CURRENT_LIST_DIR}/../include)
target_link_libraries(GraphWalk PUBLIC MCSimulation)
target_link_libraries(MCSimulation PRIVATE OpenMP::OpenMP_CXX)
target_compile_options(MCSimulation PRIVATE ${OpenMP_CXX_FLAGS})

add_library(PostProcessor PostProcessor.cpp)
target_include_directories(PostProcessor PUBLIC ${CMAKE_CURRENT_LIST_DIR}/../include)
target_link_libraries(GraphWalk PUBLIC PostProcessor)
target_link_libraries(PostProcessor PRIVATE OpenMP::OpenMP_CXX)
target_compile_options(PostProcessor PRIVATE ${OpenMP_CXX_FLAGS})

if(GUI)
    target_compile_definitions(GraphWalk PRIVATE USE_GUI)
else()
    target_compile_definitions(GraphWalk PRIVATE USE_CLI)
endif()

if(GUI)
    add_library(GUI GUI.cpp)
    target_include_directories(GUI PUBLIC ${CMAKE_CURRENT_LIST_DIR}/../include)
    if(WIN32)
        target_link_directories(GUI PUBLIC ${CMAKE_CURRENT_LIST_DIR}/../lib/win)
        target_link_libraries(GUI PRIVATE Utilities raylib gdi32 winmm)
    elseif(UNIX AND NOT APPLE)
        target_link_directories(GUI PUBLIC ${CMAKE_CURRENT_LIST_DIR}/../lib/linux)
        target_link_libraries(GUI PRIVATE Utilities raylib GL m pthread dl rt X11)
    endif()
    target_link_libraries(GraphWalk PRIVATE GUI MCSimulation PostProcessor)

    add_library(GUI_GraphEditor GUI_GraphEditor.cpp)
    target_include_directories(GUI_GraphEditor PUBLIC ${CMAKE_CURRENT_LIST_DIR}/../include)
    if(WIN32)
        target_link_directories(GUI_GraphEditor PUBLIC ${CMAKE_CURRENT_LIST_DIR}/../lib/win)
        target_link_libraries(GUI_GraphEditor PRIVATE raylib gdi32 winmm)
    elseif(UNIX AND NOT APPLE)
        target_link_directories(GUI_GraphEditor PUBLIC ${CMAKE_CURRENT_LIST_DIR}/../lib/linux)
        target_link_libraries(GUI_GraphEditor PRIVATE raylib GL m pthread dl rt X11)
    endif()
    target_link_libraries(GraphWalk PRIVATE GUI_GraphEditor)

    if(WIN32)
        file(COPY_FILE "${CMAKE_CURRENT_LIST_DIR}/../lib/win/raylib.dll"
            "${CMAKE_CURRENT_BINARY_DIR}/raylib.dll"
        )
    endif()

endif()