set(srcs esp32/com_task.cc
)

idf_component_register(
    SRCS ${srcs} 
    INCLUDE_DIRS . "./include" 
    #REQUIRES 
    PRIV_REQUIRES app_config app_misc cli app_settings config_kvs utils_debug utils_misc net net_http_server net_mqtt_app net_mqtt stm32 txtio uout 
)

component_compile_options(${comp_compile_opts})
component_compile_features(${comp_compile_feats})
component_compile_features(${comp_compile_feats})
component_compile_features(${comp_compile_feats})