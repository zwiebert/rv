/*
 * report.c
 *
 *  Created on: 16.06.2019
 *      Author: bertw
 */


#include "user_config.h"
#include "report.h"
#include "peri/uart.h"
#include <stdio.h>
#include "water_pump.h"


void report_event(const char *msg) {
  char buf[64];
  snprintf(buf, sizeof buf, "{\"status\":{\"event\":\"rv:%s\"}}\n", msg);
  esp32_puts(buf);
}

void report_valve_status(uint16_t valve_bits, uint16_t valve_mask) {
    char buf[80] = "";
    sprintf(buf, "{\"status\":{\"valve-bits\":\"0x%x\",\"valve-change-mask\":\"0x%x\"}}\n", valve_bits, valve_mask);
    esp32_puts(buf);
}

void report_pump_status(bool state) {
    char buf[80] = "";
    sprintf(buf, "{\"status\":{\"pump\":%s}}\n", state ? "1" : "0");
    esp32_puts(buf);
}

void report_pc_status(bool state) {
    char buf[80] = "";
    sprintf(buf, "{\"status\":{\"pc\":%s}}\n", state ? "1" : "0");
    esp32_puts(buf);
}

void report_state_change(int valve, int run_state) {
  char buf[80] = "";
  sprintf(buf, "{\"status\":{\"valve\"=%d,\"run_state\"=%d}}\n", valve, run_state);
  esp32_puts(buf);
}
