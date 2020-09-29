set(srcs status_output_switch.cc so_out.cc so_print.cc so_write_msg.cc so_write_config.cc) 

idf_component_register(
    SRCS ${srcs} 
    INCLUDE_DIRS . "./include" 
    REQUIRES uout 
    PRIV_REQUIRES  app_misc app_config app_cli app_settings utils_debug utils_misc net net_mqtt_app stm32 txtio uout
    )
    
component_compile_options(${comp_compile_opts})
component_compile_features(${comp_compile_feats})
component_compile_features(${comp_compile_feats})
component_compile_features(${comp_compile_feats})