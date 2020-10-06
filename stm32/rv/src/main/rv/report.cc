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

static struct Report_Cfg MyConfig;

static void MyPuts(const char *s) {
  if (MyConfig.putString)
    (*MyConfig.putString)(s);
  else
    std::fputs(s, stdout);
}


void report_event(const char *msg) {
  char buf[64];
  std::snprintf(buf, sizeof buf, "{\"status\":{\"event\":\"rv:%s\"}}\n", msg);
  MyPuts(buf);
}

void report_valve_status(uint16_t valve_bits, uint16_t valve_mask) {
    char buf[80] = "";
    std::sprintf(buf, "{\"status\":{\"valve-bits\":\"0x%x\",\"valve-change-mask\":\"0x%x\"}}\n", valve_bits, valve_mask);
    MyPuts(buf);
}

void report_pump_status(bool state) {
    char buf[80] = "";
    std::sprintf(buf, "{\"status\":{\"pump\":%s}}\n", state ? "1" : "0");
    MyPuts(buf);
}

void report_pc_status(bool state) {
    char buf[80] = "";
    std::sprintf(buf, "{\"status\":{\"pc\":%s}}\n", state ? "1" : "0");
    MyPuts(buf);
}

void report_state_change(int valve, int run_state) {
  char buf[80] = "";
  std::sprintf(buf, "{\"status\":{\"valve\"=%d,\"run_state\"=%d}}\n", valve, run_state);
  MyPuts(buf);
}

void report_setup(struct Report_Cfg cfg) {
  MyConfig = cfg;
}
