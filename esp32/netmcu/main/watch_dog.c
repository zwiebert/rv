/*
 * watch_dog.c
 *
 *  Created on: 04.06.2019
 *      Author: bertw
 */
#include <time.h>
#include "user_config.h"
#include <string.h>
#include <uart.h>


#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

int missing_call_count;
time_t last_call_time;
#define MAX_CALL_INTERVAL 12
#define MAX_MISSING_CALLS 3
#define RESET_PIN_MS 500

#ifndef STM32_RESET_PIN
#define STM32_RESET_PIN  GPIO_NUM_33
#endif

static void rebootSTM32() {
    ets_printf("reboot stm32\n");
    GPIO_OUTPUT_SET(STM32_RESET_PIN, 0);
    vTaskDelay(RESET_PIN_MS / portTICK_PERIOD_MS);
    GPIO_OUTPUT_SET(STM32_RESET_PIN, 1);
}

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
        if (MAX_MISSING_CALLS < ++missing_call_count) {
           rebootSTM32();
           missing_call_count = 0;
        }
    }

}

void watchDog_setup() {
    GPIO_OUTPUT_SET(STM32_RESET_PIN, 1);
    gpio_set_direction(STM32_RESET_PIN, GPIO_MODE_OUTPUT_OD);
    GPIO_OUTPUT_SET(STM32_RESET_PIN, 1);

}
