set(srcs  mqtt.c) 

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DMCU_ESP32")

if(DEFINED ENV{DISTRO})
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DDISTRIBUTION")
endif()

if(DEFINED ENV{CELLAR})
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DCELLAR")
endif()

idf_component_register(
    SRCS ${srcs} 
    INCLUDE_DIRS . "./include" 
    PRIV_INCLUDE_DIRS "./include/net/mqtt/app" 
    REQUIRES  app_config net_mqtt
    PRIV_REQUIRES app_config cli userio_app userio
 )