set(srcs cli.c  cli_out.c
    parm_cmd.c
    parm_config.c parm_config_switch.c
    parm_help.c
    parm_kvs.c
    parm_mcu.c
    parm_status.c
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
    PRIV_REQUIRES app_config app cli config config_kvs debug misc net net_http_server net_mqtt_app net_mqtt stm32 txtio userio 
)