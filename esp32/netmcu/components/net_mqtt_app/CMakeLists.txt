set(srcs  mqtt.cc) 

idf_component_register(
    SRCS ${srcs} 
    INCLUDE_DIRS . "./include" 
    REQUIRES  app_config net_mqtt
    PRIV_REQUIRES app_config cli userio_app userio
 )
 component_compile_options(${component_compile_opts})