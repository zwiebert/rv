set(srcs cli.cc
    parm_cmd.cc
    parm_config.cc parm_config_comp.cc parm_config_app.cc parm_config_help.cc
    parm_help.cc
    parm_kvs.cc
    parm_mcu.cc
    parm_status.cc
    
)


if(CONFIG_APP_USE_PROTOBUF)
set(protobuf nanopb protobuf)
set(protobuf_srcs parm_proto_buf.cc)
endif()

idf_component_register(
    SRCS ${srcs} ${protobuf_srcs}
    INCLUDE_DIRS . "./include" 
    REQUIRES cli app_uout
    PRIV_REQUIRES stm32 main_loop app_config  app_misc cli app_settings config_kvs utils_debug utils_misc
                  net net_http_server net_mqtt_app net_mqtt  txtio uout  ${protobuf}
                  mbedtls
)

component_compile_options(${comp_compile_opts})
component_compile_features(${comp_compile_feats})
