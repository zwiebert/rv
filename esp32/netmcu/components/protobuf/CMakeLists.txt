set(srcs
  "mcu_com.cc"
  zone_data.cc
)



set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DMCU_ESP32")

if(DEFINED ENV{DISTRO})
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DDISTRIBUTION")
endif()

idf_component_register(
    SRCS ${srcs}
    INCLUDE_DIRS "./include" "./include/protobuf"
    REQUIRES nanopb
   )
 
 component_compile_options(-${component_compile_opts})
 
 add_custom_command(
     OUTPUT ${COMPONENT_DIR}/_force.h   
     COMMAND make -B all -C ../../../../../mcu_com
     VERBATIM)
  
add_custom_target(pbuf_files ALL  DEPENDS ${COMPONENT_DIR}/_force.h )
add_dependencies(  ${COMPONENT_LIB} pbuf_files)