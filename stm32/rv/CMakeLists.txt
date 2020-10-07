cmake_minimum_required(VERSION 3.16)
option(BUILD_HOST_TESTS "build host test" OFF)
option(BUILD_HOST_XXX "build host xxx" OFF)
option(BUILD_STM32F1_XXX "build host xxx" ON)

if(${BUILD_HOST_TESTS})
include(test/host/host_test.cmake)

elseif (BUILD_HOST_XXX)

project(rv_xxx)

add_compile_definitions(MCU_HOST)

set(comp_compile_feats cxx_std_20)
set(comp_compile_opts $<$<COMPILE_LANGUAGE:CXX>:-Wno-narrowing> -Wno-missing-field-initializers)
set(comp_compile_opts ${comp_compile_opts} $<$<COMPILE_LANGUAGE:CXX>:-std=gnu++2a> -Og -ggdb  -fstack-protector-all # -fanalyzer
)
set(comp_include_dirs test/host/unity/src test/host/test_runner src/main Libraries)

list(TRANSFORM comp_include_dirs PREPEND  "${CMAKE_SOURCE_DIR}/")

include_directories(${comp_include_dirs})
add_compile_options(${comp_compile_opts})


add_subdirectory(Libraries/mcp23017)
add_subdirectory(Libraries/tm1638)

add_subdirectory(src/main)
target_link_libraries(rv_main PUBLIC mcp23017 tm1638)

add_executable(rv src/main.cpp)
target_link_libraries(rv PRIVATE rv_main)




#####################################################################################
elseif (BUILD_STM32F1_XXX)
######################################## STM32F10x ##################################
option(CMAKE_TOOLCHAIN_FILE "build for STM32" "arm-gcc-toolchain.cmake")

project(rv_stm32f1 C CXX ASM)

add_compile_definitions(STM32F1 STM32F10X_MD USE_STDPERIPH_DRIVER BUILD_DEBUG)

set(comp_compile_feats cxx_std_20)
set(comp_compile_opts $<$<COMPILE_LANGUAGE:CXX>:-Wno-narrowing> -Wno-missing-field-initializers -Wno-psabi)
set(comp_compile_opts ${comp_compile_opts}
 					  $<$<COMPILE_LANGUAGE:CXX>:-std=gnu++2a>
					  $<$<COMPILE_LANGUAGE:CXX>:-fno-rtti>
					  $<$<COMPILE_LANGUAGE:CXX>:-fno-threadsafe-statics>
					  $<$<COMPILE_LANGUAGE:CXX>:-fno-use-cxa-atexit>
					  $<$<COMPILE_LANGUAGE:C>:-std=gnu11>
                      $<$<COMPILE_LANGUAGE:ASM>:-xassembler-with-cpp>
 -Os -mcpu=cortex-m3  -g3 #  -fstack-protector-all # -fanalyzer
 -ffunction-sections -fdata-sections -fno-exceptions 
 -Wall -Wextra -Wno-missing-field-initializers -fstack-usage  -mfloat-abi=soft -mthumb
)

set(comp_include_dirs test/host/unity/src test/host/test_runner src/main
                       Libraries/CMSIS/Device/ST/STM32F10x/Include Libraries/CMSIS/Include 
                       Libraries/STM32F10x_StdPeriph_Driver/inc
                       src src/nanopb)

list(TRANSFORM comp_include_dirs PREPEND  "${CMAKE_SOURCE_DIR}/")

include_directories(${comp_include_dirs})
add_compile_options(${comp_compile_opts})
add_link_options(-mcpu=cortex-m3  librv_system.a  -T ${CMAKE_SOURCE_DIR}/Debug_STM32F103CB_FLASH.ld --specs=nosys.specs -Wl,-Map=${PROJECT_NAME}.map
                 -Wl,--gc-sections -static -L/home/bertw/arm/libopencm3/lib  -mfloat-abi=soft -mthumb -Wl,--start-group -lc -lm -lstdc++ -lsupc++ -Wl,--end-group)


add_subdirectory(Libraries/mcp23017)
add_subdirectory(Libraries/tm1638)
add_library(rv_system src/stm32f1xx_it.c  src/startup_stm32f10x_md.s src/system_stm32f10x.c  src/tiny_printf.c)
add_subdirectory(src/main)

target_link_libraries(rv_main PUBLIC mcp23017 tm1638 /home/bertw/arm/libopencm3/lib/libopencm3_stm32f1.a rv_system)


add_executable(rv.elf src/main.cpp)
target_link_libraries(rv.elf PRIVATE rv_main)

#add_custom_command(TARGET rv.elf  POST_BUILD COMMAND ${CMAKE_OBJCOPY} ARGS -O binary rv.elf rv.bin)

add_custom_command(OUTPUT rv.bin DEPENDS rv.elf  COMMAND ${CMAKE_OBJCOPY} ARGS -O binary rv.elf rv.bin)
add_custom_target(elf DEPENDS rv.elf)
add_custom_target(bin DEPENDS rv.bin)
add_custom_target(flash DEPENDS rv.bin COMMAND st-flash write rv.bin 0x8000000) 

    
endif()