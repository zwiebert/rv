set(srcs config.cc config_kvs.cc config_comps.cc)

idf_component_register(SRCS ${srcs} 
    INCLUDE_DIRS . "./include" 
    REQUIRES app_config config_kvs net net_http_server net_mqtt app_uout
    PRIV_REQUIRES  key_value_store utils_misc net_mqtt_app  
)
component_compile_options(${comp_compile_opts})
component_compile_features(${comp_compile_feats})
