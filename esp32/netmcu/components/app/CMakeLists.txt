set(srcs esp32/main.cc esp32/main_setup.cc esp32/main_loop.cc rtc.cc esp32/unity_setup.cc watch_dog.cc esp32/kvstore.cc) 

idf_component_register(
    SRCS ${srcs}
    INCLUDE_DIRS "./include" ../.. 
    PRIV_INCLUDE_DIRS . "./include/app" ../../main
    REQUIRES app_config debug misc firmware_update
    PRIV_REQUIRES app_net app_config cli_app config debug key_value_store misc stm32 storage txtio net_mqtt_app stm32_com
                  nvs_flash
    EMBED_TXTFILES ./esp32/ca_cert.pem
)

component_compile_options(${component_compile_opts})