cmake_minimum_required(VERSION 3.10)
project(DynamicBufferApp VERSION 1.0)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0")
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(SOURCE_FILES main.cpp)
add_executable(DynamicBuffer_run ${SOURCE_FILES})
# Include directories
include_directories(DynamicBuffer_lib)
add_subdirectory(DynamicBuffer_lib)

target_link_libraries(DynamicBuffer_run DynamicBuffer_lib)

add_subdirectory(Google_tests)
# Add executable
#add_executable(DynamicBufferApp main.cpp DynamicBuffer.cpp DynamicCounter.cpp
#        constants.h)
