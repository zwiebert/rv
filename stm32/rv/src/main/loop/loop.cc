#include <cli/cli.h>
#include <loop/loop.hh>
#include <utils_misc/int_macros.h>
#include <peri/i2c.h>
#include <peri/uart.h>
#include <rv/rain_sensor.hh>
#include <rv/report.h>
#include <rv/rv.hh>
#include <rv/rv_timers.hh>
#include <water_pump/water_pump_logic.h>
#include <uout/uout_builder_json.hh>
#include <time/systick_1ms.h>
#include <watch_dog/watch_dog.hh>
#include <debug/log.h>

#include <cstdio>

extern RainSensor rs;

volatile int loop_flags_once;
#ifndef loop_flags_periodic
volatile int loop_flags_periodic;
#endif

typedef void (*lfa_funT)(void);

static void i2c2_check_loop() {
  if (!i2c2_check()) {
    report_event("i2c:reset");
    puts("I2C had crashed. Reset");
  }
}

bool got_zoneData;
static void fetchData_loop() {
  if (!got_zoneData) {
    // esp32_puts("{\"to\":\"cli\",\"pbuf\":{\"zd\":\"?\"}}\n");
    esp32_puts("{\"to\":\"cli\",\"kvs\":{\"lph\":\"?\"}}\n");
    //esp32_puts("{\"to\":\"cli\",\"kvs\":{\"zn\":\"?\"}}\n");
  }
}

void postRemainingTimes() {
  char buf[512];
  UoutBuilderJson sj(buf, sizeof buf);
  static int have_something_to_report;

  if (sj.open_root_object()) {
    if (sj.add_object("data")) {
      if (sj.add_object("rem")) {
        for (const RvTimer &vt : *rvt.getTimerList()) {
          have_something_to_report = 5;
          int secs = vt.get_remaining();
          if (secs) {
            char key[16];
            std::snprintf(key, sizeof key, "%d.%d", vt.getValveNumber(), vt.getTimerNumber());
            sj.put_kv(key, secs);
          }
        }
        sj.close_object();
      }
      if (sj.add_object("dur")) {
        for (const RvTimer &vt : *rvt.getTimerList()) {
          int secs = vt.get_duration();
          if (secs) {
            char key[16];
            std::snprintf(key, sizeof key, "%d.%d", vt.getValveNumber(), vt.getTimerNumber());
            sj.put_kv(key, secs);
          }
        }
        sj.close_object();
      }
      sj.close_object();
    }
    sj.close_root_object();
  }

  if (have_something_to_report && have_something_to_report--)
    puts(sj.get_json());
}

static void interval_1s_loop() {
#ifdef CONFIG_RV_USE_WATCHDOG
  watchDog_loop();
#endif
  i2c2_check_loop();
  fetchData_loop();
  postRemainingTimes();
}

static void interval_512ms_loop() {
  rs.loop();
  rvt.loop();
}

static void interval_64ms_loop() {
  wpl_loop();
}

static void event_uart1_rx_buffer_full() {
db_loge("rv", "stdin,uart1 rx buffer full");
}
static void event_uart2_rx_buffer_full() {
db_loge("rv", "uart2 rx buffer full");
}

static const lfa_funT lfa_table[lf_Len] = { interval_1s_loop, interval_512ms_loop, interval_64ms_loop, cli_loop, wpl_loop, fetchData_loop,
event_uart1_rx_buffer_full, event_uart2_rx_buffer_full, };

void lf_loop() {
for (int i = 0; i < 4500 && !loop_flags_once; ++i) {
__asm__("nop");
}
// XXX: interrupt should be disabled here
int loop_flags = loop_flags_periodic | loop_flags_once;
loop_flags_once = 0;
// XXX: Interrupt should be re-enabled here

for (int i = 0; i < lf_Len; ++i) {
enum loop_flagbits fb = (enum loop_flagbits) i;
if (!GET_BIT(loop_flags, fb))
  continue;

(lfa_table[fb])();
}
}
