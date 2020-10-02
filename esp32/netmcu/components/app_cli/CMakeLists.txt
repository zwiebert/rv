set(srcs cli.cc
    parm_cmd.cc
    parm_config.cc parm_config_switch.cc
    parm_help.cc
    parm_kvs.cc
    parm_mcu.cc
    parm_status.cc
    parm_proto_buf.cc
)

idf_component_register(
    SRCS ${srcs} 
    INCLUDE_DIRS . "./include" 
    REQUIRES cli uout_app
    PRIV_REQUIRES app_config stm32 app_misc cli app_settings config_kvs utils_debug utils_misc net net_http_server net_mqtt_app net_mqtt  txtio uout nanopb protobuf
                  mbedtls
)

component_compile_options(${comp_compile_opts})
component_compile_features(${comp_compile_feats})
