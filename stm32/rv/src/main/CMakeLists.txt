set(srcs 
operator_new_malloc.cc
misc/base64.c
misc/cstring_utils.cc
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
cli/cli_mcu.cc
cli/cli_json.c
cli/parm_timer.cc
cli/cli_proto_buf.cc
cli/parm_kvs.cc
cli/cli.c
debug/dbg.c
protobuf/mcu_com.pb.c
protobuf/zone_data.cc
jsmn/jsmn.c
time/stm32/systick_1ms.c
time/stm32/real_time_clock.c
time/host/real_time_clock.cc
test/test.cc
test/test_rvTimer.cc
water_pump/water_pump_logic.cc
water_pump/water_pump.cc
)

add_library(rv_main STATIC ${srcs})
#target_include_directories(rv_main ${CMAKE_SOURCE_DIR}/

#target_add_sources(
