set(srcs 
valves/valve_relays.cc
peri/uart.cc
peri/relay16.cc
peri/i2c.cc
peri/dlb8.cc
loop/loop.cc
watch_dog/watch_dog.cc
setup/app_setup.cc
setup/app.cc
rv/report.cc
rv/rv_zones.cc
rv/rv.cc
rv/rv_timer.cc
rv/rv_timers.cc
rv/rv_timer_pause.cc
rv/rain_sensor.cc
rv/rv_timer_data.cc
cli/cli_getcmdline.c
cli/parm_config.cc
cli/cli_parsecmdline.c
cli/parm_cmd.cc
cli/parm_mcu.cc
cli/cli_json.c
cli/parm_timer.cc
cli/parm_protobuf.cc
cli/parm_kvs.cc
cli/cli.c
debug/dbg.c
protobuf/mcu_com.pb.c
protobuf/zone_data.cc
time/stm32/systick_1ms.c
time/stm32/real_time_clock.c
time/host/real_time_clock.cc
test/test.cc
test/test_rvTimer.cc
water_pump/water_pump_logic.cc
water_pump/water_pump.cc
)

add_library(rv_main STATIC ${srcs})
target_link_libraries(rv_main PUBLIC mcp23017 tm1638 jsmn utils_misc ${CMAKE_SOURCE_DIR}/external/libopencm3/lib/libopencm3_stm32f1.a rv_system)
add_dependencies(rv_main ocm3)
target_compile_features(rv_main PUBLIC cxx_std_20 c_std_11)

