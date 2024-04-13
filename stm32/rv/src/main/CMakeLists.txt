set(srcs 
valves/valve_relays.cc
peri/uart.cc
peri/uart1.cc
peri/uart2.cc
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
rv/rv_timer_args.cc
app_cli/parm_config.cc
app_cli/parm_cmd.cc
app_cli/parm_mcu.cc
app_cli/parm_timer.cc
app_cli/parm_protobuf.cc
app_cli/parm_kvs.cc
app_cli/process_json_hook.cc
app_cli/cli.cc
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
target_link_libraries(rv_main PUBLIC mcp23017 tm1638 jsmn utils_misc cli ${CMAKE_SOURCE_DIR}/external/libopencm3/lib/libopencm3_stm32f1.a rv_system)
add_dependencies(rv_main ocm3)
target_compile_features(rv_main PUBLIC cxx_std_20 c_std_11)

