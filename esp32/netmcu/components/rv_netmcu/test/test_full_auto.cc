#include "unity.h"
#ifdef TEST_HOST
#include <test_runner.h>
#endif

#include <debug/dbg.h>


#include "../full_auto/automatic_timer.cc"
#include "full_auto/weather.hh"

#define DEBUG

#ifdef DEBUG
#include <iostream>
using namespace std;
#define D(x) x
#else
#define D(x)
#endif

static AutoTimer at;
char buf[1024], buf2[1024];



void test_scratch()
{
  int count = 0;
  for(auto it = at.valves_begin(); it != at.valves_end(); ++it, ++count) {
    if (it->flags.exists)
      continue;
    if (0 <= count && count <= 4) {
     sprintf(it->name, "rv%d", count);
     it->flags.exists = true;
    }
  }

  count = 0;
  for(auto it = at.valveGroups_begin(); it != at.valveGroups_end(); ++it, ++count) {
    if (it->flags.exists)
      continue;
    if (0 <= count && count <= 4) {
     sprintf(it->name, "goup_%d", count);
     it->flags.exists = true;
    }
  }

  count = 0;
  for(auto it = at.adapters_begin(); it != at.adapters_end(); ++it, ++count) {
    if (it->flags.exists)
      continue;
    if (0 <= count && count <= 4) {
     sprintf(it->name, "adapt_%d", count);
     it->flags.exists = true;
    }
  }





  at.save_this("at");

  AutoTimer at2;

  at2.restore_this("at");

  SingleValve sv;
  count = 0;
  for(auto it = at2.valves_begin(); it != at2.valves_end(); ++it, ++count) {
    if (!it->flags.exists)
      continue;
     if (it->to_json(buf, sizeof buf))
       D(cout << "at2 json:" << buf << "\n");
     sv.from_json(buf);
     bzero(buf, sizeof buf);
     if (sv.to_json(buf, sizeof buf))
       D(cout << "sv  json:" << buf << "\n\n");
  }

  ValveGroup vg;
  count = 0;
  for(auto it = at2.valveGroups_begin(); it != at2.valveGroups_end(); ++it, ++count) {
    if (!it->flags.exists)
      continue;
     if (it->to_json(buf, sizeof buf))
       D(cout << "at2 json:" << buf << "\n");
     vg.from_json(buf);
     bzero(buf, sizeof buf);
     if (vg.to_json(buf, sizeof buf))
       D(cout << "vg  json:" << buf << "\n\n");
  }

  WeatherAdapter ad;
  count = 0;
  for(auto it = at2.adapters_begin(); it != at2.adapters_end(); ++it, ++count) {
    if (!it->flags.exists)
      continue;
     if (it->to_json(buf, sizeof buf))
       D(cout << "at2 json:" << buf << "\n");
     ad.from_json(buf);
     bzero(buf, sizeof buf);
     if (ad.to_json(buf, sizeof buf))
       D(cout << "ad  json:" << buf << "\n\n");
  }
}



void test_json()
{
  int count = 0;
  for(auto it = at.valves_begin(); it != at.valves_end(); ++it, ++count) {
    if (it->flags.exists)
      continue;
    if (0 <= count && count <= 4) {
     sprintf(it->name, "rv%d", count);
     it->flags.has_adapter = rando(0,2);
     it->flags.active = rando(0,2);
     it->flags.exists = true;
     it->dry_days = rando(0, 14);
     it->duration_s = rando(60, 60*20);
     it->flow_lph = rando(20, 400);
     it->adapter = rando(-1, 15);
    }
  }

  count = 0;
  for(auto it = at.valveGroups_begin(); it != at.valveGroups_end(); ++it, ++count) {
    if (it->flags.exists)
      continue;
    if (0 <= count && count <= 4) {
     sprintf(it->name, "goup_%d", count);
     it->flags.has_adapter = rando(0,2);
     it->flags.active = rando(0,2);
     it->flags.exists = true;
     it->valve_bits = rando(0, 9999999);
     it->interval_days = rando(1, 7);
     it->adapter = rando(-1, 15);
    }
  }

  count = 0;
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

  SingleValve sv;
  count = 0;
  for (auto it = at2.valves_begin(); it != at2.valves_end(); ++it, ++count) {
    if (!it->flags.exists)
      continue;
    TEST_ASSERT_TRUE_MESSAGE(it->to_json(buf, sizeof buf), "Serialize object to json");
    TEST_ASSERT_TRUE_MESSAGE(sv.from_json(buf), "Read back the json we just generated");
    bzero(buf2, sizeof buf2);
    TEST_ASSERT_TRUE_MESSAGE(sv.to_json(buf2, sizeof buf2), "Serialize object to json");
    TEST_ASSERT_EQUAL_STRING_MESSAGE(buf, buf2, "The json generated after  to_json()->from_json()->to_json() must be equal");
    D(cout << "at2 json:" << buf << "\n");
  }

  ValveGroup vg;
  count = 0;
  for(auto it = at2.valveGroups_begin(); it != at2.valveGroups_end(); ++it, ++count) {
    if (!it->flags.exists)
      continue;
    TEST_ASSERT_TRUE_MESSAGE(it->to_json(buf, sizeof buf), "Serialize object to json");
    TEST_ASSERT_TRUE_MESSAGE(vg.from_json(buf), "Read back the json we just generated");
    bzero(buf2, sizeof buf2);
    TEST_ASSERT_TRUE_MESSAGE(vg.to_json(buf2, sizeof buf2), "Serialize object to json");
    TEST_ASSERT_EQUAL_STRING_MESSAGE(buf, buf2, "The json generated after  to_json()->from_json()->to_json() must be equal");

    D(cout << "at2 json:" << buf << "\n");
  }

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

void test_at_to_json() {
  static AutoTimer at;
  static char buf[256];
  const auto buf_size = sizeof buf;
  static int obj_ct;
  static int br;
  for (int i=0; i < 10 && 0 <= obj_ct; ++i) {
    br = at.to_json(buf, buf_size, obj_ct);
    buf[br] = '\0';
    D(cout << "br: " << br << "json: " << buf << "\n");
  }
}

const int TOTAL_OBJS = CONFIG_APP_NUMBER_OF_VALVES + CONFIG_APP_FA_MAX_VALVE_GROUPS + CONFIG_APP_FA_MAX_WEATHER_ADAPTERS;

void test_at_from_json() {
  static AutoTimer at1, at2;
  static const auto buf_size = 10000;
  static char buf1[buf_size], buf2[buf_size];
  int obj_ct1 = 0, obj_ct2 = 0;


  at1.dev_random_fill_data();
  TEST_ASSERT_LESS_THAN_INT_MESSAGE(buf_size, at1.to_json(buf1, buf_size, obj_ct1), "Read JSON into large buffer in one go");
  TEST_ASSERT_LESS_THAN_INT_MESSAGE(0, obj_ct1, "Reach EOF because of large enough buffer");
  TEST_ASSERT_EQUAL_INT_MESSAGE( -TOTAL_OBJS, obj_ct1,  "Make TOTAL_OBJS in JSON by at.to_json()");
  D(cout << "json in buf1: <" << buf1 << ">\n");

  TEST_ASSERT_TRUE_MESSAGE(at2.from_json(buf1), "Read back JSON we produced above");
  TEST_ASSERT_LESS_THAN_INT_MESSAGE(buf_size, at2.to_json(buf2, buf_size, obj_ct2), "Read JSON into large buffer in one go");
  TEST_ASSERT_LESS_THAN_INT_MESSAGE(0, obj_ct2, "Reach EOF because of large enough buffer");
  TEST_ASSERT_EQUAL_INT_MESSAGE( -TOTAL_OBJS, obj_ct2,  "Make TOTAL_OBJS in JSON by at.to_json()");
  D(cout << "json in buf2: <" << buf2 << ">\n");

  TEST_ASSERT_EQUAL_STRING_MESSAGE(buf1, buf2, "Write at1 to buf1, Read buf1 into at2. Write at2 to buf2");

}

TEST_CASE("full_auto", "[app]")
{
  test_at_from_json();
#if 0
  test_at_to_json();
  test_mulitple_fetch();
    // test_scratch();
     test_json();
#endif
}
