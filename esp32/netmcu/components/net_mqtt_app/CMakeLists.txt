set(srcs  mqtt.cc) 

idf_component_register(
    SRCS ${srcs} 
    INCLUDE_DIRS . "./include" 
    REQUIRES  app_config net_mqtt
    PRIV_REQUIRES  cli uout_app uout
 )
 
component_compile_options(${comp_compile_opts})
component_compile_features(${comp_compile_feats})
