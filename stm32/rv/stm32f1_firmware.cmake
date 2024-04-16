cmake_minimum_required(VERSION 3.16)

option(CMAKE_BUILD_TYPE "Set to one of: Debug, Release, Cellar" Debug)

set(CMAKE_TOOLCHAIN_FILE  "arm-gcc-toolchain.cmake" CACHE STRING "build for arm")
message("toolchain: ${CMAKE_TOOLCHAIN_FILE}")
message("C compile features:  ${CMAKE_C_COMPILE_FEATURES} (${CMAKE_C_KNOWN_FEATURES})") 
message("C++ compile features:  ${CMAKE_CXX_COMPILE_FEATURES}  (${CMAKE_CXX_KNOWN_FEATURES})") 
project(rv_stm32f1 C CXX ASM)

add_compile_definitions(MCU_STM32F1 STM32F1 STM32F10X_MD)

add_compile_options($<$<COMPILE_LANGUAGE:CXX>:-fno-rtti>
		    $<$<COMPILE_LANGUAGE:CXX>:-fno-threadsafe-statics>
		    $<$<COMPILE_LANGUAGE:CXX>:-fno-use-cxa-atexit>
                    $<$<COMPILE_LANGUAGE:CXX>:-fno-exceptions>
		    $<$<COMPILE_LANGUAGE:CXX>:-std=gnu++2a>
                    $<$<COMPILE_LANGUAGE:CXX>:-Wno-narrowing>
		    $<$<COMPILE_LANGUAGE:C>:-std=gnu11>
                    $<$<COMPILE_LANGUAGE:ASM>:-xassembler-with-cpp>
                      -mcpu=cortex-m3 -mfloat-abi=soft -mthumb
                      #  -fstack-protector-all # -fanalyzer
                      -ffunction-sections -fdata-sections -fstack-usage
                      -Wall -Wextra -Wno-missing-field-initializers -Wno-psabi
)

add_compile_options(-include ${PROJECT_BINARY_DIR}/rv_config.h)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  add_compile_options(${comp_compile_opts} -Og -g3)
  add_compile_definitions(BUILD_DEBUG)
elseif(CMAKE_BUILD_TYPE STREQUAL "Cellar")
  add_compile_options(${comp_compile_opts} -Os -g3)
  add_compile_definitions(BUILD_CELLAR)
else()
  add_compile_options(${comp_compile_opts} -O3 -g3)
  add_compile_definitions(BUILD_RELEASE)
endif()



include_directories(test/host/unity/src test/host/test_runner src/main  src src/nanopb)

add_compile_options(${comp_compile_opts})
add_link_options(-mcpu=cortex-m3  -nostartfiles -T ${CMAKE_SOURCE_DIR}/stm32f103cb.ld --specs=nosys.specs -Wl,-Map=${PROJECT_NAME}.map
                 -Wl,--gc-sections -static -L${CMAKE_SOURCE_DIR}/external/libopencm3/lib  -mfloat-abi=soft -mthumb -Wl,--start-group -lc -lm -lstdc++ -lsupc++ -Wl,--end-group -lopencm3_stm32f1)

######### External OpenCM3 lib ##########
include_directories(external/libopencm3/include)
add_custom_command(OUTPUT ${CMAKE_SOURCE_DIR}/external/libopencm3/lib/libopencm3_stm32f1.a
                   COMMAND make ARGS -C ${CMAKE_SOURCE_DIR}/external/libopencm3 lib/stm32/f1)
add_custom_target(ocm3 DEPENDS ${CMAKE_SOURCE_DIR}/external/libopencm3/lib/libopencm3_stm32f1.a)


#####  components-mcu  #########
add_subdirectory(external/jsmnit/components/jsmn)
add_subdirectory(components-mcu/utils_misc)
add_subdirectory(components-mcu/utils_debug)
add_subdirectory(components-mcu/cli)
add_subdirectory(components-mcu/txtio)
add_subdirectory(components-mcu/uout)

######### Our MCP23017 lib ##############
add_subdirectory(Libraries/mcp23017)

######### Our TM1638 lib  ###############
add_subdirectory(Libraries/tm1638)


##### Startup, Interrupt Handlers, etc ##
add_library(rv_system  src/tiny_printf.c)
target_compile_features(rv_system PUBLIC c_std_11)

#### App Archive ########################
add_subdirectory(src/main)

### Linked Firmware #####################
add_executable(rv.elf src/main.cpp)
target_link_libraries(rv.elf PRIVATE rv_main)
add_custom_command(OUTPUT rv.bin DEPENDS rv.elf  COMMAND ${CMAKE_OBJCOPY} ARGS -O binary rv.elf rv.bin)


## phony buildtargets: elf, bin, flash ##
add_custom_target(elf DEPENDS rv.elf)
add_custom_target(bin DEPENDS rv.bin)
add_custom_target(flash DEPENDS rv.bin COMMAND st-flash write rv.bin 0x8000000) 
