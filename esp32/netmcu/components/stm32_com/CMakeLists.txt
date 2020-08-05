set(srcs esp32/com_task.c
)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DMCU_ESP32")

if(DEFINED ENV{DISTRO})
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DDISTRIBUTION")
endif()

idf_component_register(
    SRCS ${srcs} 
    INCLUDE_DIRS . "./include" 
    #REQUIRES 
    PRIV_REQUIRES app_config app cli config config_kvs debug misc net net_http_server net_mqtt_app net_mqtt stm32 txtio userio 
)