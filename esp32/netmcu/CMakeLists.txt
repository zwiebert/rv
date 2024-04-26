cmake_minimum_required(VERSION 3.16)
option(CMAKE_EXPORT_COMPILE_COMMANDS "generate compilation database for clang-tidy" ON)

option(BUILD_HOST_TESTS "build configs/host-test" OFF)
option(BUILD_ESP32_RELEASE "build configs/esp32-release" OFF)
option(BUILD_ESP32_DEBUG "build configs/esp32-debug" OFF)

#option(CMAKE_esptool_py_COMPILER "/home/bertw/.espressif/tools/xtensa-esp-elf/esp-13.2.0_20230928/xtensa-esp-elf/bin/xtensa-esp32-elf-gcc") 


if(BUILD_HOST_TESTS)
project("rv-netmcu-tests")

file(MAKE_DIRECTORY ${PROJECT_BINARY_DIR}/config)
file(COPY ${CMAKE_CURRENT_LIST_DIR}/configs/host-test/config/sdkconfig.h DESTINATION ${PROJECT_BINARY_DIR}/config)
file(COPY ${CMAKE_CURRENT_LIST_DIR}/configs/host-test/config/sdkconfig.cmake DESTINATION ${PROJECT_BINARY_DIR}/config)
file(COPY ${CMAKE_CURRENT_LIST_DIR}/configs/host-test/config/sdkconfig.json DESTINATION ${PROJECT_BINARY_DIR}/config)
include("configs/host-test/CMakeLists.txt")
endif()

if(BUILD_ESP32_DEBUG)
project("rv-netmcu")
include("configs/esp32-cmake/CMakeLists.txt")
endif()

if(BUILD_ESP32_RELEASE)
project("rv-netmcu")
include("configs/esp32-release/CMakeLists.txt")
endif()
