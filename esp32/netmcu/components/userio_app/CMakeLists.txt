set(srcs status_output.cc status_output_switch.cc so_out.cc so_print.cc) 

idf_component_register(
    SRCS ${srcs} 
    INCLUDE_DIRS . "./include" 
    REQUIRES userio 
    PRIV_REQUIRES  app_misc app_config cli_app config debug misc net net_mqtt_app stm32 txtio userio
    )
    
component_compile_options(${component_compile_opts})