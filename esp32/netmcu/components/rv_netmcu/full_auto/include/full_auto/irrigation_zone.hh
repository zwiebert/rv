#pragma once
#include "adapter.hh"
#include <cstring>
#include <cstdio>
#include <ctime>
#include <jsmn/jsmn_iterate.hh>
#include "jsoneat/from_to_json_jsmn_cbuf.hh"
#include <utils_time/ut_constants.hh>

/**
 * \brief  Controls a relay for a irrigation zone.
 *
 *  To have more than one magnetic valve per zone, you have to connect them to the same relay.
 *
 *  Zones usually have different schedules and durations. If it happens, that different zones need water at the same time, then
 *  the zone priority comes into play. Also the configured water flow a zone needs may allow other zones to get water too, as long as
 *  the sum of all zone flows not exceeds the configure flow limit of the water source.
 *
 *  TODO: Older code parts (in both esp32 and stm32 code) use the term "valve" instead of "zone."
 *
 */
struct IrrigationZone {
  using self_type = IrrigationZone;

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
