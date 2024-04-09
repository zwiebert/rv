/*
 * watch_dog.c
 *
 *  Created on: 04.06.2019
 *      Author: bertw
 */
#include <stm32/stm32.h>
#include <time.h>
#include "app_config/proj_app_cfg.h"
#include <string.h>
#include "app_net/gw_ping.h"
#include "debug/dbg.h"

#ifdef CONFIG_RV_NETMCU_DEBUG
#define DEBUG
#define D(x) x
#else
#define D(x)
#endif
#define logtag "rv.misc.watch_dog"

int missing_call_count;
time_t last_call_time;
#define MAX_CALL_INTERVAL 12
#define MAX_MISSING_CALLS 3



bool watchDog_checkCommandLine(const char *cmdLine) {
  if (strcmp(cmdLine, "alive?") == 0) {
    stm32_write("\r\nALIVE!\r\n", 7); // response
    missing_call_count = 0;
    last_call_time = time(0);
    return true;
  }
  return false;
}

void watchDog_loop() {
  time_t now = time(0);
    if ((last_call_time + MAX_CALL_INTERVAL) <= now ) {
        last_call_time = now; // missed call from STM32
        if (stm32_isFirmwareRunning() && MAX_MISSING_CALLS < ++missing_call_count) {
           stm32_runFirmware();
           missing_call_count = 0;
        }
    }
}

