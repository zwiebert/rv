cmake_minimum_required(VERSION 3.16)
option(BUILD_HOST_TESTS "build host test" OFF)
option(CMAKE_EXPORT_COMPILE_COMMANDS "generate compilation database for clang-tidy" ON)
option(ASAN "enable compiler sanitizer" OFF)
option(GPROF "enable gprof" OFF)

file(GLOB external_components ${CMAKE_SOURCE_DIR}/../../external/*/components ${CMAKE_SOURCE_DIR}/../../external/*/external/*/components)
set(EXTRA_COMPONENT_DIRS components)
list(TRANSFORM EXTRA_COMPONENT_DIRS PREPEND   ${CMAKE_SOURCE_DIR}/../../)
list(APPEND EXTRA_COMPONENT_DIRS ${external_components})

### for menuconfig we use  esp-idf (XXX: switch to using kconfig tools directly)###
# esp-idf menuconfig
# insert a normal esp-idf CMakeLists.txt here for creating sdkconfig
if (NOT BUILD_HOST_TESTS)
set(PROJECT_NAME "rv-netmcu")


set(COMPONENTS rv_netmcu
    bootloader esptool_py partition_table soc log spi_flash micro-ecc main efuse driver esp_rom  
)

#set environment variables to be used in components
set(comp_compile_feats cxx_std_20)
set(comp_compile_opts -Wno-missing-field-initializers)


include($ENV{IDF_PATH}/tools/cmake/project.cmake)

add_compile_definitions(MCU_ESP32 MCU_TYPE="esp32")
add_compile_options(-include esp_attr.h)


function(component_compile_features)
    target_compile_features(${COMPONENT_LIB} PRIVATE ${ARGV})
endfunction()

project(netmcu)
endif()


#### testing uses cmake w/o esp-idf ####
if (BUILD_HOST_TESTS)
project(netmcu-test)

set(comp_compile_feats cxx_std_20)
set(comp_compile_opts -O0 -ggdb  # -fanalyzer
-Werror=return-type
)

if(${GPROF})
  add_link_options(-pg)
  add_compile_options(-pg)
endif()

if(ASAN)
  add_link_options(-fsanitize=address)
  add_compile_options(-fsanitize=address -fstack-protector-all)
endif()

include(${CMAKE_SOURCE_DIR}/../../external/host-test-idf/components/test_host/host_test.cmake)

add_subdirectory(${CMAKE_SOURCE_DIR}/../../components/rv_netmcu ${CMAKE_BINARY_DIR}/${BIN_COMP_ROOT}/rv_netmcu)


endif() # BUILD_HOST_TESTS
