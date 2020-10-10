cmake_minimum_required(VERSION 3.16)
option(BUILD_ONLY_TESTS "build host test" OFF)
set(TARGET_ARCH "STM32F1" CACHE STRING "Set the intended target. One of: HOST, STM32F1")


###################### BUILD ONLY UNIT TESTS FOR HOST ###############################
if(${TARGET_ARCH} STREQUAL "HOST" AND ${BUILD_ONLY_TESTS})
include(test/host/host_test.cmake)


#######################  BUILD FIRMWARE FOR HOST ####################################
elseif (${TARGET_ARCH} STREQUAL "HOST")
include(host_firmware.cmake)

######################  BUILD FIRMWARE FOR STM32F1 ##################################
elseif (${TARGET_ARCH} STREQUAL "STM32F1")
include(stm32f1_firmware.cmake)
 
endif()