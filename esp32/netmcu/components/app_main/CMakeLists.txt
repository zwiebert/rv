set(srcs esp32/main.cc esp32/main_setup.cc esp32/main_loop.cc esp32/unity_setup.cc esp32/main_loop_periodic.cc) 

idf_component_register(
    SRCS ${srcs}
    INCLUDE_DIRS "./include"
    REQUIRES app_settings utils_debug utils_misc firmware_update
    PRIV_REQUIRES main_loop utils_time net app_net app_misc app_settings app_cli app_settings utils_debug key_value_store utils_misc stm32 storage txtio net_mqtt_app stm32_com net_http_server http_server_content
                  nvs_flash driver esp_event esp_netif
)

component_compile_options(${comp_compile_opts})
component_compile_features(${comp_compile_feats})
