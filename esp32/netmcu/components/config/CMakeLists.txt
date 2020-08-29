set(srcs config.cc config_kvs.cc config_comps.cc)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DMCU_ESP32")

if(DEFINED ENV{DISTRO})
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DDISTRIBUTION")
endif()
if(DEFINED ENV{FLAVOR_LAN})
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DFLAVOR_LAN")
endif()
if(DEFINED ENV{CELLAR})
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DCELLAR")
endif()

idf_component_register(SRCS ${srcs} 
    INCLUDE_DIRS . "./include" 
    PRIV_INCLUDE_DIRS "./include/config" ../../main
    REQUIRES app_config config_kvs net net_http_server net_mqtt
    PRIV_REQUIRES app_config key_value_store misc net_mqtt_app    
)