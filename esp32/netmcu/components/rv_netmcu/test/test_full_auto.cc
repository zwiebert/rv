#include "unity.h"
#ifdef TEST_HOST
#include <test_runner.h>
#endif



#include "full_auto/scratch/structures.hh"
static auto &bp = brunnen_pumpe;


#include <iostream>
using namespace std;

WetterDaten wd = {.rain_sensor = false, .wetter_faktor = 15.0};






void test_scratch()
{
  for(auto it = bp.ek_begin(); it != bp.ek_end(); ++it) {
    cout << it->name << "\n";
  }

  for(auto gr = bp.gr_begin(); gr != bp.gr_end(); ++gr) {
    auto &group = *gr;
    cout << group.name << ", h: " << group.ge.haeufigkeit <<  ", ha: " << group.ge.adaption_h(group.ge.haeufigkeit, wd) << "\n";
    for (auto kreis : group.kreise) {
      cout << "    " << kreis->name << ", dran: " <<  group.ist_dran(*kreis, wd) <<  "\n";
    }
  }
}


TEST_CASE("test rtc", "[app]")
{
     test_scratch();
}
