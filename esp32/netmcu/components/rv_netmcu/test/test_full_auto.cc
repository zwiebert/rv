#include "unity.h"
#ifdef TEST_HOST
#include <test_runner.h>
#endif



#include "../full_auto/automatic_timer.cc"


#include <iostream>
using namespace std;


static AutoTimer at;
char buf[1024];



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

  count = 0;
  for(auto it = at2.valves_begin(); it != at2.valves_end(); ++it, ++count) {
    if (!it->flags.exists)
      continue;
     if (it->to_json(buf, sizeof buf))
       cout << "json:" << buf << "\n";
  }

  count = 0;
  for(auto it = at2.valveGroups_begin(); it != at2.valveGroups_end(); ++it, ++count) {
    if (!it->flags.exists)
      continue;
     if (it->to_json(buf, sizeof buf))
       cout << "json:" << buf << "\n";
  }

  WeatherAdapter ad;
  count = 0;
  for(auto it = at2.adapters_begin(); it != at2.adapters_end(); ++it, ++count) {
    if (!it->flags.exists)
      continue;
     if (it->to_json(buf, sizeof buf))
       cout << "json:" << buf << "\n";
     ad.from_json(buf);
     bzero(buf, sizeof buf);
     if (ad.to_json(buf, sizeof buf))
       cout << "ad json:" << buf << "\n";
  }
}


TEST_CASE("full_auto", "[app]")
{
     test_scratch();
}
