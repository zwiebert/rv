set(srcs esp32/ota.cc esp32/main.cc esp32/main_setup.cc esp32/main_loop.cc rtc.cc esp32/unity_setup.cc watch_dog.cc esp32/kvstore.cc) 

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


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CMAKE_C_FLAGS} -std=gnu++2a" )


idf_component_register(
    SRCS ${srcs}
    INCLUDE_DIRS "./include" ../.. 
    PRIV_INCLUDE_DIRS . "./include/app" ../../main
    REQUIRES app_config debug misc 
    PRIV_REQUIRES app_net app_config cli_app config debug key_value_store misc stm32 storage txtio net_mqtt_app stm32_com
                  nvs_flash esp_https_ota app_update
    EMBED_TXTFILES ./esp32/ca_cert.pem
)