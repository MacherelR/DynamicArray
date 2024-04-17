cmake_minimum_required(VERSION 3.16)
project(DynamicBuffer)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(HEADER_FILES
        DynamicBuffer.h
        LastKnownValuesBuffer.h
)

set(SOURCE_FILES
        DynamicBuffer.cpp
        LastKnownValuesBuffer.cpp
)

add_library(DynamicBuffer_lib SHARED ${SOURCE_FILES} ${HEADER_FILES})