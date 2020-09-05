set(srcs esp32/http_server_content.cc
)


idf_component_register(
    SRCS ${srcs}
    INCLUDE_DIRS . "./include"
    PRIV_INCLUDE_DIRS "./include/http_server_content"
    REQUIRES ""
    PRIV_REQUIRES cli_app cli config misc userio net_http_server webapp
                  mbedtls
   )
   
 component_compile_options(${component_compile_opts})