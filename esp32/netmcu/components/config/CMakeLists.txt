set(srcs config.cc config_kvs.cc config_comps.cc)

idf_component_register(SRCS ${srcs} 
    INCLUDE_DIRS . "./include" 
    PRIV_INCLUDE_DIRS "./include/config" ../../main
    REQUIRES app_config config_kvs net net_http_server net_mqtt
    PRIV_REQUIRES app_config key_value_store misc net_mqtt_app    
)
component_compile_options(-std=${cxx_standard})