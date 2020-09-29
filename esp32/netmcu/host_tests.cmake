project(netmcu-test)

set(COMPONENT_DIRECTORIES components components-mcu/components)
#list(TRANSFORM COMPONENT_DIRECTORIES PREPEND ${CMAKE_CURRENT_LIST_DIR})
set(comp_compile_feats cxx_std_20)
set(comp_compile_opts "$<$<COMPILE_LANGUAGE:CXX>:-Wno-narrowing>" -Wno-missing-field-initializers)
set(comp_compile_opts ${comp_compile_opts} "$<$<COMPILE_LANGUAGE:CXX>:-std=gnu++2a>" -Og -ggdb)

include(components-mcu/components/test_host/host_test.cmake)


add_subdirectory(components-mcu/components/test_host)


message("all_libs: ${COMPONENT_LIBS}")
message("all_tests: ${TEST_EXECUTABLES}")