/**
 * \file cli/parm_timer.cc
 * \brief handle cli command
 */
#include "user_config.h"
#include <time/real_time_clock.h>
#include <string.h>
#include "cli_imp.h"
#include <stdio.h>
#include "peri/uart.h"
#include "water_pump/water_pump.h"
#include "rv/rv_timers.hh"
#include "rv/rain_sensor.hh"

#define warning_unknown_option(x)
extern "C" void timer_set(int8_t channel);

#define KEY_STATUS_PREFIX "status"
#define KEY_STATUS_PREFIX_LEN ((sizeof KEY_STATUS_PREFIX) - 1)
#define CMD_ASK_STATUS " status=?;"
#define CMD_ASK_STATUS_LEN (sizeof CMD_ASK_STATUS - 1)

#define JSON_PREFIX "{\"data\":{"
#define JSON_PREFIX_LEN ((sizeof JSON_PREFIX) - 1)
#define JSON_SUFFIX "}}\n"
#define JSON_SUFFIX_LEN ((sizeof JSON_SUFFIX) - 1)

#define KEY_VERSION "version"

#define BUF_SIZE 2048

const char help_parmTimer[] = "zone=[0-13]      zone number\n"
    "duration=[0-60]  how long to irrigate\n";

int process_parmTimer(clpar p[], int len) {
  RvTimer::SetArgs args = RvTimer::SetArgs(reinterpret_cast<cstr_pair*>(&p[1]), len - 1);

  RvTimer *timer;
  if (args.valve_number >= 0 && (timer = rvt.set(args)) && timer->scheduleRun()) {
    rvt.loop(); // XXX
  } else {
    // XXX: error
  }

  return 0;
}
