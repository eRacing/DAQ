cmake_minimum_required(VERSION 3.9)
project(daq)

include_directories(${PROJECT_SOURCE_DIR}/include)

set(DAQ_SOURCES ${PROJECT_SOURCE_DIR}/source)

option(BUILD_TESTS "Build unit tests" ON)
if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif(BUILD_TESTS)
