set(srcs  mqtt.cc) 

idf_component_register(
    SRCS ${srcs} 
    INCLUDE_DIRS . "./include" 
    REQUIRES  app_config 
    PRIV_REQUIRES  cli app_uout net_mqtt
 )
 
component_compile_options(${comp_compile_opts})
component_compile_features(${comp_compile_feats})
