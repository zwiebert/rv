# The following lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.16)

option(BUILD_HOST_TESTS "build host test" OFF)

if (BUILD_HOST_TESTS)
include(host_tests.cmake)

doxy_create_input()
else()

set(PROJECT_NAME "tronferno-mcu")

set(EXTRA_COMPONENT_DIRS ./components-mcu/components)

set(COMPONENTS app_main
	  bootloader esptool_py partition_table soc log spi_flash micro-ecc main efuse driver esp_rom  
)

#set environment variables to be used in components
set(comp_compile_feats cxx_std_20)
set(comp_compile_opts "$<$<COMPILE_LANGUAGE:CXX>:-Wno-narrowing>" -Wno-missing-field-initializers)
#set(comp_compile_opts ${comp_compile_opts} "$<$<COMPILE_LANGUAGE:CXX>:-std=gnu++2a>")

add_link_options(-fsanitize=address)
add_compile_options(-fsanitize=address)

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

function(component_compile_features)
    target_compile_features(${COMPONENT_LIB} PRIVATE ${ARGV})
endfunction()

project(netmcu esptool_py                
)


#set_target_properties(${CMAKE_PROJECT_NAME}.elf PROPERTIES LINK_INTERFACE_MULTIPLICITY 10)

set(comps app_main app_settings  utils_misc utils_debug txtio uout cli jsmn 
 net_http_server net_mqtt_app uout_app app_misc app_net net app_cli protobuf nanopb stm32 config_kvs 
 key_value_store net_mqtt stm32_com http_server_content firmware_update storage utils_time) 

foreach(comp ${comps})
  message("comp: ${comp}")
 #set_target_properties(__idf_${comp} PROPERTIES LINK_INTERFACE_MULTIPLICITY 10)
endforeach()

endif()