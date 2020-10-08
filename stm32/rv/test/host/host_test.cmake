
option(CMAKE_EXPORT_COMPILE_COMMANDS "generate compilation database for clang-tidy" ON)


project(rv-test)
include(CTest)

add_compile_definitions(UNITY_OUTPUT_COLOR UNITY_SUPPORT_TEST_CASES HOST_TESTING TEST_HOST TEST MCU_HOST)

set(comp_compile_feats cxx_std_20)
set(comp_compile_opts "$<$<COMPILE_LANGUAGE:CXX>:-Wno-narrowing>" -Wno-missing-field-initializers)
set(comp_compile_opts ${comp_compile_opts} "$<$<COMPILE_LANGUAGE:CXX>:-std=gnu++2a>" -Og -ggdb  -fstack-protector-all # -fanalyzer
)

set(comp_include_dirs test/host/unity/src test/host/test_runner src/main)

include_directories(${comp_include_dirs})
add_compile_options(${comp_compile_opts})

add_executable(test_timers test/host/src/test_timers.cc test/host/test_runner/test_runner.c test/host/unity/src/unity.c
                                          src/main/rv/rv_timers.cc src/main/rv/rv_timer.cc src/main/rv/rv_timer_pause.cc
                                          src/main/rv/rv_zones.cc src/main/rv/report.cc 
                                          src/main/time/host/real_time_clock.cc src/main/operator_new_malloc.cc)                                       
add_test(NAME test_timers COMMAND test_timers  WORKING_DIRECTORY ${PROJECT_BINARY_DIR})


add_executable(test_cli_json test/host/src/test_cli_json.c test/host/test_runner/test_runner.c test/host/unity/src/unity.c
                                          src/main/cli/cli_json.c src/main/jsmn/jsmn.c)                                       
add_test(NAME test_cli_json COMMAND test_cli_json  WORKING_DIRECTORY ${PROJECT_BINARY_DIR})
