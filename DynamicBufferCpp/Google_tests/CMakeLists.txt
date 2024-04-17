cmake_minimum_required(VERSION 3.16)
project(Google_tests)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_subdirectory(lib) 
add_subdirectory(../src/DynamicBuffer_lib ${CMAKE_CURRENT_BINARY_DIR}/DynamicBuffer_lib_build)


# adding the Google_Tests_run target
add_executable(Google_Tests_run DynamicBufferTest.cpp)

target_include_directories(Google_Tests_run PRIVATE ../src/DynamicBuffer_lib)

target_link_libraries(Google_Tests_run DynamicBuffer_lib)

target_link_libraries(Google_Tests_run gtest gtest_main) 