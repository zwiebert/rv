#include "unity.h"
#ifdef TEST_HOST
#include <test_runner.h>
#endif

#include <debug/dbg.h>


#include "../full_auto/automatic_timer.cc"
#include "full_auto/weather.hh"
#include "full_auto/weather.hh"

#include "weather/weather_irrigation.hh"
#include "weather/weather_provider_owm.hh"
#include <full_auto/automatic_timer.hh>

#ifdef CONFIG_RV_NETMCU_TEST_DEBUG
#include <iostream>
using namespace std;
#define DEBUG
#define D(x) x
#else
#define D(x)
#endif
#undef logtag
#define logtag "rv.test.full_auto"


static AutoTimer at;
char buf[1024], buf2[1024];



void test_json()
{
  int count = 0;
  for(auto it = at.adapters_begin(); it != at.adapters_end(); ++it, ++count) {
    if (it->flags.exists)
      continue;
    if (0 <= count && count <= 4) {
     sprintf(it->name, "adapt_%d", count);
     it->flags.exists = true;
     it->d_temp = rando(0.0001, 0.5);
     it->d_wind = rando(0.0001, 0.5);
     it->d_humi = rando(0.0001, 0.5);
     it->d_clouds = rando(0.0001, 0.5);
    }
  }

  at.save_this("at");
  AutoTimer at2;
  at2.restore_this("at");

  WeatherAdapter ad;
  count = 0;
  for(auto it = at2.adapters_begin(); it != at2.adapters_end(); ++it, ++count) {
    if (!it->flags.exists)
      continue;
    TEST_ASSERT_TRUE_MESSAGE(it->to_json(buf, sizeof buf), "Serialize object to json");
    TEST_ASSERT_TRUE_MESSAGE(ad.from_json(buf), "Read back the json we just generated");
    bzero(buf2, sizeof buf2);
    TEST_ASSERT_TRUE_MESSAGE(ad.to_json(buf2, sizeof buf2), "Serialize object to json");
    TEST_ASSERT_EQUAL_STRING_MESSAGE(buf, buf2, "The json generated after  to_json()->from_json()->to_json() must be equal");


    D(cout << "at2 json:" << buf << "\n");
  }
}

void test_mulitple_fetch() {
  for (int i=0; i < 4; ++i) {
    TEST_ASSERT_TRUE_MESSAGE(fa_poll_weather_full_hour(), "Fetch, parse and save JSON data ... what could go wrong?");
    sleep(10);
  }
}
void test_at_from_json() {
  static AutoTimer at1, at2;
  static const auto buf_size = 10000;
  static char buf1[buf_size], buf2[buf_size];
  int obj_ct1 = 0, obj_ct2 = 0;


  at1.dev_random_fill_data();
  TEST_ASSERT_LESS_THAN_INT_MESSAGE(buf_size, at1.to_json(buf1, buf_size, obj_ct1), "Read JSON into large buffer in one go");
  TEST_ASSERT_LESS_THAN_INT_MESSAGE(0, obj_ct1, "Reach EOF because of large enough buffer");
  TEST_ASSERT_EQUAL_INT_MESSAGE( -at.TOTAL_OBJS, obj_ct1,  "Make TOTAL_OBJS in JSON by at.to_json()");
  D(cout << "json in buf1: <" << buf1 << ">\n");

  TEST_ASSERT_TRUE_MESSAGE(at2.from_json(buf1), "Read back JSON we produced above");
  TEST_ASSERT_LESS_THAN_INT_MESSAGE(buf_size, at2.to_json(buf2, buf_size, obj_ct2), "Read JSON into large buffer in one go");
  TEST_ASSERT_LESS_THAN_INT_MESSAGE(0, obj_ct2, "Reach EOF because of large enough buffer");
  TEST_ASSERT_EQUAL_INT_MESSAGE( -at.TOTAL_OBJS, obj_ct2,  "Make TOTAL_OBJS in JSON by at.to_json()");
  D(cout << "json in buf2: <" << buf2 << ">\n");

  TEST_ASSERT_EQUAL_STRING_MESSAGE(buf1, buf2, "Write at1 to buf1, Read buf1 into at2. Write at2 to buf2");

}

void test_at_loop() {
  static Weather_Provider_Owm weather_provider;
  static Weather_Irrigation weather_irrigation(&weather_provider);
  weather_irrigation.dev_fill_past_wd_randomly();
  static AutoTimer at(&weather_irrigation);
  at.dev_random_fill_data();


  at.todo_loop();

}

TEST_CASE("full_auto", "[app]")
{
  test_at_loop();
  test_at_from_json();
  //test_mulitple_fetch();
  test_json();
}
