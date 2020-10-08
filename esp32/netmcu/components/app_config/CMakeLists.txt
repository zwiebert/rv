idf_component_register(
    INCLUDE_DIRS . "./include"
    REQUIRES utils_misc
)
   

 add_custom_command(
     OUTPUT #"${COMPONENT_DIR}/include/app_settings/proj_kconfig.h"
             ${CMAKE_CURRENT_BINARY_DIR}/_force.h   
     COMMAND make all -C ${CMAKE_CURRENT_LIST_DIR}
     VERBATIM)
  
add_custom_target(app_config_kconfig_header ALL  DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/_force.h )
add_dependencies(${COMPONENT_LIB} app_config_kconfig_header)