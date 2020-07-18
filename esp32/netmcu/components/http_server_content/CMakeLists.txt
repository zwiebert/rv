set(srcs esp32/http_server_content.c)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DMCU_ESP32")

if(DEFINED ENV{DISTRO})
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DDISTRIBUTION")
endif()

idf_component_register(
    SRCS ${srcs}
    INCLUDE_DIRS . "./include"
    PRIV_INCLUDE_DIRS "./include/http_server_content"
    REQUIRES ""
    PRIV_REQUIRES cli_app cli config misc userio net_http_server
                  mbedtls
   )
 

    
 add_custom_command(
     OUTPUT ${COMPONENT_DIR}/webapp/__force.h
            "./webapp/build/wapp.html.gz.c"
            "./webapp/build/wapp.js.gz.c"
            "./webapp/build/wapp.css.gz.c"
            "./webapp/build/wapp.js.map.gz.c"
     COMMAND make all -C ${COMPONENT_DIR}/webapp
     VERBATIM)
     
 add_custom_target(
    content_files
    DEPENDS ${COMPONENT_DIR}/webapp/__force.h
 )
 
 add_dependencies(
  ${COMPONENT_LIB} content_files
)