# The following lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.5)

set(PROJECT_NAME "tronferno-mcu")

set(EXTRA_COMPONENT_DIRS ./components-mcu/components)

set(COMPONENTS
    app  app_config  app_net  cli_app  config  http_server_content  net_mqtt_app  userio_app
    
    cli  config_kvs  debug  key_value_store  misc  misc_time  net  net_http_server  net_mqtt  stm32  storage  txtio  userio
    
	  bootloader esptool_py partition_table soc log spi_flash micro-ecc main efuse driver esp_rom  
)

set(cxx_standard gnu++2a)



include($ENV{IDF_PATH}/tools/cmake/project.cmake)


add_compile_definitions(MCU_ESP32)
if(DEFINED ENV{DISTRO})
add_compile_definitions(DISTRIBUTION)
endif()
if(DEFINED ENV{FLAVOR_LAN})
add_compile_definitions(FLAVOR_LAN)
endif()
if(DEFINED ENV{CELLAR})
add_compile_definitions(CELLAR)
endif()


project(netmcu esptool_py                
)
