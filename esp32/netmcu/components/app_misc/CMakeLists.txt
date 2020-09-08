set(srcs rtc.cc watch_dog.cc esp32/kvstore.cc) 

idf_component_register(
    SRCS ${srcs}
    INCLUDE_DIRS "./include"
    REQUIRES app_config debug misc firmware_update
    PRIV_REQUIRES app_net app_config cli_app config debug key_value_store misc stm32 storage txtio net_mqtt_app stm32_com net_http_server http_server_content
                  nvs_flash
    EMBED_TXTFILES ./ca_cert.pem
)

component_compile_options(${component_compile_opts})