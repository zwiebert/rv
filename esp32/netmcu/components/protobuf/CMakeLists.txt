set(srcs
  "mcu_com.cc"
  zone_data.cc
)

set(pb_path ../../../../../mcu_com)


set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DMCU_ESP32")

if(DEFINED ENV{DISTRO})
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DDISTRIBUTION")
endif()

idf_component_register(
    SRCS ${srcs}
    INCLUDE_DIRS "./include" "./include/protobuf"
    REQUIRES nanopb
   )
 
 component_compile_options(${component_compile_opts})
component_compile_features(${component_compile_features})
component_compile_features(${component_compile_features})
component_compile_features(${component_compile_features})
 
 add_custom_command(
     OUTPUT ${COMPONENT_DIR}/_force.h ${COMPONENT_DIR}/mcu_com.cc
     COMMAND make -B all -C ${pb_path} && ln -sf ${pb_path}/build/mcu_com.pb.c mcu_com.cc
     VERBATIM)
  
add_custom_target(pbuf_files ALL  DEPENDS ${COMPONENT_DIR}/_force.h )
add_dependencies(  ${COMPONENT_LIB} pbuf_files)