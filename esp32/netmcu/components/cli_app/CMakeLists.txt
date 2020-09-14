set(srcs cli.cc  cli_out.cc
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
    REQUIRES cli userio_app
    PRIV_REQUIRES app_config app_misc cli config config_kvs debug misc net net_http_server net_mqtt_app net_mqtt
                  stm32 txtio userio nanopb protobuf
                  mbedtls
)

component_compile_options(${component_compile_opts})
component_compile_features(${component_compile_features})
component_compile_features(${component_compile_features})
component_compile_features(${component_compile_features})