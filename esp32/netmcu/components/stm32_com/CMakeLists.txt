set(srcs esp32/com_task.cc
)

idf_component_register(
    SRCS ${srcs} 
    INCLUDE_DIRS . "./include" 
    #REQUIRES 
    PRIV_REQUIRES app_config app_misc cli config config_kvs debug misc net net_http_server net_mqtt_app net_mqtt stm32 txtio userio 
)

component_compile_options(${component_compile_opts})