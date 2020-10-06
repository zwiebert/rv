#define UNITY_SUPPORT_TEST_CASES
#include "../test_runner/test_runner.h";
#include "unity.h"

#include <stdint.h>
#include "rv/rv_timers.hh"
#include <rv/rv_zone.hh>
#include <rv/rv_zones.hh>


static void sw_valve(int vn, bool isOpen) {

}


RvTimers rvt = RvTimers(sw_valve);
RainSensor rs;
RvZones rvz;



RainSensor::RainSensor() {
}

bool RainSensor::getState(time_t delay) {
  return false;
}

void RainSensor::loop(void) {

}

time_t RainSensor::getOffTime() {
  return 1000;
}

volatile uint32_t run_time;


void setUp() {

}

void tearDown() {

}
