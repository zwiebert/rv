# The following lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.5)

set(PROJECT_NAME "tronferno-mcu")

set(EXTRA_COMPONENT_DIRS ./components-mcu/components)

set(COMPONENTS
    app app_config cli_app config net_http_server http_server_content net_mqtt_app stm32 userio_app 
	bootloader esptool_py partition_table soc log spi_flash micro-ecc main efuse driver esp_rom  
)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)

project(netmcu esptool_py                
)
