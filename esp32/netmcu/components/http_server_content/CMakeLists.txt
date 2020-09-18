set(srcs esp32/http_server_content.cc
)


idf_component_register(
    SRCS ${srcs}
    INCLUDE_DIRS . "./include"
    REQUIRES ""
    PRIV_REQUIRES app_cli cli config_app misc uout net_http_server webapp
                  mbedtls
   )
   
 component_compile_options(${comp_compile_opts})
component_compile_features(${comp_compile_feats})
component_compile_features(${comp_compile_feats})
component_compile_features(${comp_compile_feats})