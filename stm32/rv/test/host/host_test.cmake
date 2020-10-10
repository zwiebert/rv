cmake_minimum_required(VERSION 3.16)

option(CMAKE_EXPORT_COMPILE_COMMANDS "generate compilation database for clang-tidy" ON)

project(rv-test)
include(CTest)

add_compile_definitions(HOST_TESTING TEST_HOST TEST MCU_HOST)
add_compile_options( "$<$<COMPILE_LANGUAGE:CXX>:-Wno-narrowing>" -Wno-missing-field-initializers
                     "$<$<COMPILE_LANGUAGE:CXX>:-std=gnu++2a>" -Og -ggdb  -fstack-protector-all # -fanalyzer
)
include_directories(src/main)


################# Catch2 Test Framework ##################
add_library(catch2_tr test/host/src/catch2.cc)
add_subdirectory(external/catch2)
target_link_libraries(catch2_tr PUBLIC Catch2)

function(catch_test)
  list(POP_FRONT ARGV test_name)
  
  add_executable(${test_name} ${ARGV})
  target_link_libraries(${test_name} PUBLIC catch2_tr)                                      
  add_test(NAME ${test_name} COMMAND ${test_name}  WORKING_DIRECTORY ${PROJECT_BINARY_DIR})
endfunction()



################# Unity Test Framework  ##################
add_library(unity_tr STATIC test/host/test_runner/test_runner.c)
target_include_directories(unity_tr PUBLIC test/host/test_runner)
add_subdirectory(external/unity)
target_link_libraries(unity_tr PUBLIC unity)
add_compile_definitions(UNITY_OUTPUT_COLOR UNITY_SUPPORT_TEST_CASES)

function(unity_test)
  list(POP_FRONT ARGV test_name)
  
  add_executable(${test_name} ${ARGV})
  target_link_libraries(${test_name} PUBLIC unity_tr)                                      
  add_test(NAME ${test_name} COMMAND ${test_name}  WORKING_DIRECTORY ${PROJECT_BINARY_DIR})
endfunction()

####################################################################
######################### Tests ####################################
####################################################################

unity_test(test_timers test/host/src/test_timers.cc 
                       src/main/rv/rv_timers.cc src/main/rv/rv_timer.cc src/main/rv/rv_timer_pause.cc
                       src/main/rv/rv_zones.cc src/main/rv/report.cc 
                       src/main/time/host/real_time_clock.cc src/main/operator_new_malloc.cc)                                       


unity_test(test_cli_json  test/host/src/test_cli_json.c
                          src/main/cli/cli_json.c src/main/jsmn/jsmn.c)



catch_test(test_catch2 test/host/src/test_catch2.cc)                                       

