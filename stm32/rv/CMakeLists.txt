cmake_minimum_required(VERSION 3.16)
option(BUILD_HOST_TESTS "build host test" ON)

if(${BUILD_HOST_TESTS})
include(test/host/host_test.cmake)


endif()