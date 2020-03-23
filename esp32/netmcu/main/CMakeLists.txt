# Edit following two lines to set component requirements (see docs)
#set(COMPONENT_REQUIRES )
#set(COMPONENT_PRIV_REQUIRES )

#set(COMPONENT_SRC "main.c app.c ethernet.c uart.c")
set(COMPONENT_SRCDIRS ".")


set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DMCU_ESP32")
if(DEFINED ENV{DISTRO})
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DDISTRIBUTION")
endif()
if(DEFINED ENV{FLAVOR_LAN})
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DFLAVOR_LAN")
endif()

register_component(
  #  INCLUDE_DIRS .  
  #  PRIV_INCLUDE_DIRS "." 
    REQUIRES  app_config
    PRIV_REQUIRES net
)
