set(srcs esp32/gw_ping.cc) 

idf_component_register(
    SRCS ${srcs}
    INCLUDE_DIRS "./include"
    #REQUIRES  
    PRIV_REQUIRES utils_debug utils_misc net
                  lwip esp_event esp_netif
)

component_compile_options(${comp_compile_opts})
component_compile_features(${comp_compile_feats})