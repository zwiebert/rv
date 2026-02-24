#pragma once
#include "adapter.hh"
#include <cstring>
#include <cstdio>
#include <ctime>
#include <jsmn/jsmn_iterate.hh>
#include "jsoneat/from_to_json_jsmn_cbuf.hh"
#include <utils_time/ut_constants.hh>

struct MagValve {
  using self_type = MagValve;

  char name[CONFIG_APP_FA_NAMES_MAX_LEN] = "";

  struct {
    bool active = false;  ///< to mark as active (or not disabled temporarily)
    bool exists = false;   ///< to mark as non existent flat value in array
    bool is_due = false;


    JSONEAT_SER_FROM_TO(JSONEAT_KvPairs(exists, active, is_due));
  } flags;

  struct {
    unsigned duration_s = 0;
    int8_t adapter = 0; ///< adapt duration (0 should always be a neutral adapter)
    unsigned flow_lph = 0;
    int priority = 0;
    unsigned interval_s = SECS_PER_DAY;

    JSONEAT_SER_FROM_TO(JSONEAT_KvPairs(duration_s, adapter, flow_lph, priority, interval_s));
  } attr;

  struct {
    time_t last_time_wet = 0;
    time_t next_time_scheduled = 0;

    JSONEAT_SER_FROM_TO(JSONEAT_KvPairs(last_time_wet, next_time_scheduled));
  } state;


  JSONEAT_SER_FROM_TO(JSONEAT_KvPairs(name, flags, attr, state));
};
