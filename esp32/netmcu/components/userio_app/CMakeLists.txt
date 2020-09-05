set(srcs status_output.cc status_output_switch.cc so_out.cc so_print.cc) 

idf_component_register(
    SRCS ${srcs} 
    INCLUDE_DIRS . "./include" 
    PRIV_INCLUDE_DIRS "./include/userio_app" ../../main 
    REQUIRES userio 
    PRIV_REQUIRES  app app_config cli_app config debug misc net net_mqtt_app stm32 txtio userio
    )
    
component_compile_options(${component_compile_opts})