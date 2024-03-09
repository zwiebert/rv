#include "full_auto/scratch/structures.hh"

#include <iostream>
#include <array>
#include <string>
#include <iterator>
using namespace std;


static AdaptionH h_adapters[] = {
    {.name = "Lawn", .fun = [](int h, const WetterDaten &wd) -> int {
      auto wpt = wd.wetter_faktor;
      return wpt > 17 ? 3 : (wpt > 15) ? 4 : (wpt > 10) ? 5 : 7;
    } },
    {.name = "Flower", .fun = [](int h, const WetterDaten &wd) -> int {
      auto wpt = wd.wetter_faktor;
      return wpt > 14 ? 1 : (wpt > 12.5) ? 2 : (wpt > 11) ? 3 : 5;
    } },
    {.name = "Hedge", .fun = [](int h, const WetterDaten &wd) -> int {
      auto wpt = wd.wetter_faktor;
      return wpt > 15 ? 2 : (wpt > 13) ? 3 : (wpt > 11) ? 4 : 7;
    } },
    {.name = "Pot", .fun = [](int h, const WetterDaten &wd) -> int {
      auto wpt = wd.wetter_faktor;
      return wpt > 10 ? 2 : 10;
    } },
};


static EinzelKreis all_valves[] = {
    {.name = "lawn_west", .not_run_since = 4, .eke = {.normal_dauer = 20,  .wasserdurchfluss_lph = 200, .adaption_d = nullptr}},
    {.name = "flow_hortens", .not_run_since = 4,  .eke = {.normal_dauer = 9,  .wasserdurchfluss_lph = 200, .adaption_d = nullptr}},
    {.name = "flow_south", .not_run_since = 4,  .eke = {.normal_dauer = 7,  .wasserdurchfluss_lph = 200, .adaption_d = nullptr}},
    {.name = "lawn_east", .not_run_since = 4,  .eke = {.normal_dauer = 20,  .wasserdurchfluss_lph = 200, .adaption_d = nullptr}},
    {.name = "rv4", .not_run_since = 4,  .eke = {.normal_dauer = 0,  .wasserdurchfluss_lph = 200, .adaption_d = nullptr}},
    {.name = "flow_east", .not_run_since = 4,  .eke = {.normal_dauer = 9,  .wasserdurchfluss_lph = 200, .adaption_d = nullptr}},
    {.name = "lawn_north", .not_run_since = 4,  .eke = {.normal_dauer = 20,  .wasserdurchfluss_lph = 200, .adaption_d = nullptr}},
    {.name = "hedge_east", .not_run_since = 4,  .eke = {.normal_dauer = 12,  .wasserdurchfluss_lph = 200, .adaption_d = nullptr}},
    {.name = "flow_northeast", .not_run_since = 4,  .eke = {.normal_dauer = 5,  .wasserdurchfluss_lph = 200, .adaption_d = nullptr}},
    {.name = "flow_west", .not_run_since = 4,  .eke = {.normal_dauer = 20,  .wasserdurchfluss_lph = 200, .adaption_d = nullptr}},
    {.name = "flow_north", .not_run_since = 4,  .eke = {.normal_dauer = 11, .wasserdurchfluss_lph = 200, .adaption_d = nullptr}},
    {.name = "pot_north",  .not_run_since = 4, .eke = {.normal_dauer = 1, .wasserdurchfluss_lph = 200, .adaption_d = nullptr}},
    {.name = "rv12", .not_run_since = 4,  .eke = {.normal_dauer = 0, .wasserdurchfluss_lph = 200, .adaption_d = nullptr}},
    {.name = "rv13", .not_run_since = 4,  .eke = {.normal_dauer = 0, .wasserdurchfluss_lph = 200, .adaption_d = nullptr}},
    {.name = "rv14",  .not_run_since = 4, .eke = {.normal_dauer = 0, .wasserdurchfluss_lph = 200, .adaption_d = nullptr}},
};

static auto &av = all_valves;

static Gruppe all_groups[] = {
    {.name = "Lawns", .kreise = {&av[0], &av[3], &av[6]}, .ge = { .prio = 0, .haeufigkeit = 7, .adaption_h = h_adapters[0].fun}},
    {.name = "Hedges", .kreise = {&av[7]}, .ge = { .prio = 0, .haeufigkeit = 7, .adaption_h = h_adapters[2].fun}},
    {.name = "Flowers", .kreise = {&av[1], &av[5], &av[8], &av[9], &av[10]}, .ge = { .prio = 0, .haeufigkeit = 7, .adaption_h = h_adapters[1].fun}},
    {.name = "Pots", .kreise = {&av[11]}, .ge = { .prio = 0, .haeufigkeit = 7, .adaption_h = h_adapters[3].fun}},

};
static constexpr unsigned nmb_all_valves = sizeof all_valves / sizeof all_valves[0];
static constexpr unsigned nmb_groups = sizeof all_groups / sizeof all_groups[0];

EinzelKreis *GartenPumpe::ek_begin() {
  return &all_valves[0];
}
EinzelKreis *GartenPumpe::ek_end() {
  return &all_valves[nmb_all_valves];
}
EinzelKreis *GartenPumpe::ek_get(unsigned idx) {
  return &all_valves[idx];
}

Gruppe *GartenPumpe::gr_begin() {
  return &all_groups[0];
}
Gruppe *GartenPumpe::gr_end() {
  return &all_groups[nmb_groups];
}
Gruppe *GartenPumpe::gr_get(unsigned idx) {
  return &all_groups[idx];
}

GartenPumpe::GartenPumpe(): name("Brunnenpumpe"), max_wasserdurchfluss_lph(1000),
    max_wasserentnahme_lph(3000)

{

}

GartenPumpe brunnen_pumpe;
