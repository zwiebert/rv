project(rv_host)

add_compile_definitions(MCU_HOST)

add_compile_options(  $<$<COMPILE_LANGUAGE:CXX>:-Wno-narrowing>
                      -Wno-missing-field-initializers
                      $<$<COMPILE_LANGUAGE:CXX>:-std=gnu++2a>
                      -Og -ggdb  -fstack-protector-all
                       # -fanalyzer
)

set(comp_include_dirs test/host/unity/src test/host/test_runner src/main Libraries)

list(TRANSFORM comp_include_dirs PREPEND  "${CMAKE_SOURCE_DIR}/")

include_directories(${comp_include_dirs})
add_compile_options(${comp_compile_opts})


#add_subdirectory(Libraries/mcp23017)
#add_subdirectory(Libraries/tm1638)

add_subdirectory(src/main)
#target_link_libraries(rv_main PUBLIC mcp23017 tm1638)

add_executable(rv src/main.cpp)
target_link_libraries(rv PRIVATE rv_main)

