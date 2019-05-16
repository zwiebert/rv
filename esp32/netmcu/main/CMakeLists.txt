# Edit following two lines to set component requirements (see docs)
set(COMPONENT_REQUIRES )
set(COMPONENT_PRIV_REQUIRES )

set(COMPONENT_SRC "main.c app.c ethernet.c uart.c")
set(COMPONENT_SRCDIRS "./userio")
set(COMPONENT_ADD_INCLUDEDIRS "")

register_component()

add_custom_target(
    myCustomTarget
    COMMAND make -C ${CMAKE_CURRENT_SOURCE_DIR}/userio/http_data
)
add_dependencies(all myCustomTarget)