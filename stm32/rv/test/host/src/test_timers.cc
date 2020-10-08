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
  std::cout << "switch_valve: " << vn << ":" << isOpen << '\n';
}

static void timer_mod(int vn, int tn, bool remove) {
  std::cout << "modified timer: vn=" << vn << " tn=" << tn << " remove=" << remove << '\n';
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

     if (i == 5)
       rain = true;

     if (IS_IN_RANGE(2, i, 5)) {
       TEST_ASSERT_TRUE(GET_BIT(valves, 0));
       TEST_ASSERT_FALSE(GET_BIT(valves, 1));
     } else if (IS_IN_RANGE(8, i, 10)) {
       TEST_ASSERT_FALSE(GET_BIT(valves, 0));
       TEST_ASSERT_FALSE(GET_BIT(valves, 1));
       TEST_ASSERT_EQUAL(0, rvt.get_used_count());
     }

     rvt.loop();
   }

   rain = false;
}


static void test_rain() {
  rvt.register_callback(timer_mod);
  rain = true;

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

     if (IS_IN_RANGE(1, i, 30)) {
       TEST_ASSERT_FALSE(GET_BIT(valves, 0));
       TEST_ASSERT_FALSE(GET_BIT(valves, 1));
       TEST_ASSERT_EQUAL(0, rvt.get_used_count());
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
     std::cout << '\r' << i << "s :" << std::flush;

     if (IS_IN_RANGE(2, i, 10)) {
       TEST_ASSERT_TRUE(GET_BIT(valves, 0));
       TEST_ASSERT_FALSE(GET_BIT(valves, 1));
     } else if (IS_IN_RANGE(12, i, 20)) {
       TEST_ASSERT_FALSE(GET_BIT(valves, 0));
       TEST_ASSERT_TRUE(GET_BIT(valves, 1));
     }

     if (IS_IN_RANGE(0, i, 10)) {
       TEST_ASSERT_EQUAL(2, rvt.get_used_count());
     } else if (IS_IN_RANGE(14, i, 20)) {
       TEST_ASSERT_EQUAL(1, rvt.get_used_count());
     } else if (IS_IN_RANGE(24, i, 25)) {
       TEST_ASSERT_EQUAL(0, rvt.get_used_count());
     }


     rvt.loop();
   }
}


TEST_CASE("timer", "[rv_timers]") {
  test_rain2();
  test_rain();
  test_timer();
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
