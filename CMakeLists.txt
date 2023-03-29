cmake_minimum_required(VERSION 3.22)
project(cpp_test)

set(CMAKE_CXX_STANDARD 14)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_library(tests
        ${CMAKE_CURRENT_SOURCE_DIR}/tests/test_template.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/tests/test_unorderd_map.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/tests/test_process.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/tests/test_thread.cpp
        )

add_executable(cpp_test main.cpp)
target_link_libraries(cpp_test tests)
