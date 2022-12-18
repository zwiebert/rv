set(srcs rtc.cc watch_dog.cc esp32/kvstore.cc opt_map.cc) 

idf_component_register(
    SRCS ${srcs}
    INCLUDE_DIRS "./include"
    REQUIRES app_config utils_debug utils_misc firmware_update utils_time
    PRIV_REQUIRES app_net  app_settings utils_debug key_value_store utils_misc stm32 storage txtio net_mqtt_app  net_http_server http_server_content
                  nvs_flash driver
    EMBED_TXTFILES ./ca_cert.pem
)

component_compile_options(${comp_compile_opts})
component_compile_features(${comp_compile_feats})
