set(srcs cli.cc  cli_out.cc
    parm_cmd.cc
    parm_config.cc parm_config_switch.cc
    parm_help.cc
    parm_kvs.cc
    parm_mcu.cc
    parm_status.cc
    parm_proto_buf.cc
)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DMCU_ESP32")

if(DEFINED ENV{DISTRO})
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DDISTRIBUTION")
endif()

idf_component_register(
    SRCS ${srcs} 
    INCLUDE_DIRS . "./include" 
    PRIV_INCLUDE_DIRS "./include/cli_app" ../../main/
    REQUIRES cli userio_app
    PRIV_REQUIRES app_config app cli config config_kvs debug misc net net_http_server net_mqtt_app net_mqtt
                  stm32 txtio userio nanopb protobuf
                  mbedtls
)