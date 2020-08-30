set(srcs  mqtt.cc) 

idf_component_register(
    SRCS ${srcs} 
    INCLUDE_DIRS . "./include" 
    PRIV_INCLUDE_DIRS "./include/net/mqtt/app" 
    REQUIRES  app_config net_mqtt
    PRIV_REQUIRES app_config cli userio_app userio
 )
 component_compile_options(-std=${cxx_standard})