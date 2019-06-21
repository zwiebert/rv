/*
 * watch_dog.c
 *
 *  Created on: 04.06.2019
 *      Author: bertw
 */
#include <stm32/stm32.h>
#include <time.h>
#include "user_config.h"
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

int missing_call_count;
time_t last_call_time;
#define MAX_CALL_INTERVAL 12
#define MAX_MISSING_CALLS 3



bool watchDog_checkCommandLine(const char *cmdLine) {
    if (strcmp(cmdLine, "alive?;") == 0) {
        stm32_write("ALIVE!;", 7); // response
        missing_call_count = 0;
        last_call_time = time(0);
        return true;
    }
    return false;
}

void watchDog_loop() {
    if (last_call_time + MAX_CALL_INTERVAL <= time(0) ) {
        last_call_time = time(0);
        if (stm32_isFirmwareRunning() && MAX_MISSING_CALLS < ++missing_call_count) {
           stm32_runFirmware();
           missing_call_count = 0;
        }
    }

}

void watchDog_setup() {

}
