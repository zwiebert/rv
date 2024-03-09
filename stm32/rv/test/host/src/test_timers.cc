#include "test_runner.h"
#include "unity.h"

#include <stdint.h>
#include <unistd.h>
#include <iostream>
#include "rv/rv_timers.hh"
#include <rv/rv_zone.hh>
#include <rv/rv_zones.hh>
#include <misc/int_macros.h>

static uint16_t valves;
static bool rain;

static void sw_valve(int vn, bool isOpen) {
  PUT_BIT(valves, vn, isOpen);
  std::cout << (isOpen ? "open" : "close") <<  " valve " << vn << '\n';
}

static void timer_mod(int vn, int tn, bool remove) {
  std::cout << "modify timer: vn=" << vn << " tn=" << tn << " remove=" << remove << '\n';
}

RvTimers rvt = RvTimers(sw_valve);
RainSensor rs;
RvZones rvz;

static void test_rain2() {
  rvt.register_callback(timer_mod);
  rain = false;

  RvTimer::SetArgs args;

  args.valve_number = 0;
  args.on_duration = 10;
  args.ignorePumpPause = true;

   if (auto t = rvt.set(args))
       t->scheduleRun();

   args.valve_number = 1;
    if (auto t = rvt.set(args))
       t->scheduleRun();


   for(int i=0; i < 10; ++i) {
     sleep(1);
     std::cout << '\r' << i << "s :" << std::flush;

     const bool v0 = GET_BIT(valves, 0);
     const bool v1 = GET_BIT(valves, 1);
     const int  tmr_count = rvt.get_used_count();

     if (i == 5)  // Activate rain sensor
       rain = true;

     if (IS_IN_RANGE(2, i, 5)) {
       TEST_ASSERT_TRUE_MESSAGE(v0, "Open valve 0 by timer because rain sensor off");
       TEST_ASSERT_FALSE_MESSAGE(v1, "Open valve 0 by timer because rain sensor off");
     } else if (IS_IN_RANGE(8, i, 10)) {
       TEST_ASSERT_FALSE_MESSAGE(v0, "Close valve 0 because rain sensor off");
       TEST_ASSERT_FALSE_MESSAGE(v1, "Close valve 1 because rain sensor off");
       TEST_ASSERT_EQUAL_MESSAGE(0, tmr_count, "Remove all active timers because rain off");
     }

     rvt.loop();
   }

   rain = false;
}


static void test_rain() {
  rvt.register_callback(timer_mod);
  rain = true; // rain sensor on

  RvTimer::SetArgs args;

  args.valve_number = 0;
  args.on_duration = 10;
  args.ignorePumpPause = true;

   if (auto t = rvt.set(args))
       t->scheduleRun();

   args.valve_number = 1;
    if (auto t = rvt.set(args))
       t->scheduleRun();


   for(int i=0; i < 10; ++i) {
     sleep(1);
     std::cout << '\r' << i << "s :" << std::flush;

     const bool v0 = GET_BIT(valves, 0);
     const bool v1 = GET_BIT(valves, 1);
     const int  tmr_count = rvt.get_used_count();

     if (IS_IN_RANGE(1, i, 30)) {
       TEST_ASSERT_FALSE_MESSAGE(v0, "Prevent valve 0 open because rain sensor on");
       TEST_ASSERT_FALSE_MESSAGE(v1, "Prevent valve 1 open because rain sensor on");
       TEST_ASSERT_EQUAL_MESSAGE(0, tmr_count, "Prevent timer activating because rain sensor on");
     }

     rvt.loop();
   }

   rain = false;
}



static void test_timer() {
  rvt.register_callback(timer_mod);

  RvTimer::SetArgs args;

  args.valve_number = 0;
  args.on_duration = 10;
  args.ignorePumpPause = true;

   if (auto t = rvt.set(args))
       t->scheduleRun();

   args.valve_number = 1;
    if (auto t = rvt.set(args))
       t->scheduleRun();


   for(int i=0; i < 30; ++i) {
     sleep(1);
     std::cout << '\r' << i << "s: " << std::flush;

     const bool v0 = GET_BIT(valves, 0);
     const bool v1 = GET_BIT(valves, 1);
     const int  tmr_count = rvt.get_used_count();

     if (IS_IN_RANGE(2, i, 10)) {
       TEST_ASSERT_TRUE_MESSAGE(v0, "Open valve 0 by timer");
       TEST_ASSERT_FALSE_MESSAGE(v1, "Don't open valve 1 until valve 0 closes");
     } else if (IS_IN_RANGE(13, i, 20)) {
       TEST_ASSERT_FALSE_MESSAGE(v0, "Close valve 0 by timer");
       TEST_ASSERT_TRUE_MESSAGE(v1, "Open valve 1 after valve 0 has closed");
     }

     if (IS_IN_RANGE(0, i, 10)) {
       TEST_ASSERT_EQUAL_MESSAGE(2, tmr_count, "Keep timers active until done");
     } else if (IS_IN_RANGE(14, i, 20)) {
       TEST_ASSERT_EQUAL_MESSAGE(1, tmr_count, "End first timer because its done");
     } else if (IS_IN_RANGE(25, i, 26)) {
       TEST_ASSERT_EQUAL_MESSAGE(0, tmr_count, "End second timer because its done");
     }


     rvt.loop();
   }
}


TEST_CASE("timer", "[rv_timers]") {
  test_timer();
  test_rain2();
  test_rain();

}







RainSensor::RainSensor() {
}

bool RainSensor::getState(time_t delay) {
  return rain;
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
