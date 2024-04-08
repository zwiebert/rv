/*
 * report.c
 *
 *  Created on: 16.06.2019
 *      Author: bertw
 */


#include <stdbool.h>
#include <cstdio>
#include <stdint.h>
#include "report.h"
#include <debug/log.h>
#ifdef CONFIG_CLI_DEBUG
#define DEBUG
#define D(x) x
#define L(x) x
#else
#define D(x)
#define L(x) x
#endif
#define logtag "rv"


void report_event(const char *msg) {
  D(db_logi(logtag, "%s(%s)", __func__, msg));
  char buf[64];
  std::snprintf(buf, sizeof buf, "{\"status\":{\"event\":\"rv:%s\"}}", msg);
  std::puts(buf);
}

void report_valve_status(uint16_t valve_bits, uint16_t valve_mask) {
    D(db_logi(logtag, "%s(bits=0x%x, mask=0x%x)", __func__, valve_bits, valve_mask));
    char buf[80] = "";
    std::sprintf(buf, "{\"status\":{\"valve-bits\":\"0x%x\",\"valve-change-mask\":\"0x%x\"}}", valve_bits, valve_mask);
    std::puts(buf);
}

void report_pump_status(bool state) {
    D(db_logi(logtag, "%s(%d)", __func__, state));
    char buf[80] = "";
    std::sprintf(buf, "{\"status\":{\"pump\":%s}}", state ? "1" : "0");
    std::puts(buf);
}

void report_pc_status(bool state) {
    D(db_logi(logtag, "%s(%d)", __func__, state));
    char buf[80] = "";
    std::sprintf(buf, "{\"status\":{\"pc\":%s}}", state ? "1" : "0");
    std::puts(buf);
}

void report_state_change(int valve, int run_state) {
  D(db_logi(logtag, "%s(vn=%d, state=%d)", __func__, valve, run_state));
  char buf[80] = "";
  std::sprintf(buf, "{\"status\":{\"valve\":%d,\"run_state\":%d}}", valve, run_state);
  std::puts(buf);
}
