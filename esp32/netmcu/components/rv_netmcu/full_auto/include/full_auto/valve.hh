#pragma once
#include "adapter.hh"
#include <cstring>
#include <cstdio>
#include <ctime>
#include <jsmn/jsmn_iterate.hh>
#include "jsoneat/from_json_jsmn.hh"
#include <utils_time/ut_constants.hh>

struct MagValve {
  using self_type = MagValve;

  char name[CONFIG_APP_FA_NAMES_MAX_LEN] = "";

  struct {
    bool active = false;  ///< to mark as active (or not disabled temporarily)
    bool exists = false;   ///< to mark as non existent flat value in array
    bool is_due = false;


    template<typename jsmn_iterator>
    bool from_json(jsmn_iterator &it) {
    return jsoneat::from_json::jsmn::deserialize_object(it, JSONEAT_KvPairs(exists, active, is_due));
    }

  } flags;

  struct {
    unsigned duration_s = 0;
    int8_t adapter = 0; ///< adapt duration (0 should always be a neutral adapter)
    unsigned flow_lph = 0;
    int priority = 0;
    unsigned interval_s = SECS_PER_DAY;

    template<typename jsmn_iterator>
    bool from_json(jsmn_iterator &it) {
      return jsoneat::from_json::jsmn::deserialize_object(it, JSONEAT_KvPairs(duration_s, adapter, priority, interval_s));
    }

  } attr;

  struct {
    time_t last_time_wet = 0;
    time_t next_time_scheduled = 0;

    template<typename jsmn_iterator>
    bool from_json(jsmn_iterator &it) {
      return jsoneat::from_json::jsmn::deserialize_object(it, JSONEAT_KvPairs(last_time_wet, next_time_scheduled));
    }

  } state;

  /**
   * \brief            serialize object into JSON formatted null-terminated string.
   * \param dst        output buffer
   * \param dst_size   output buffer size
   * \return           return values are the as standard snprintf(3). The bytes written or if larger than dst_size, the bytes which would have written.
   *                   the terminating null byte is not counted, but its always there even if a data bytes has to be truncated for it.
   *                   so make sure to add one byte to the return value before using it as a parameter for dst_size in a retry call.
   */
  int to_json(char *dst, size_t dst_size) const {
    return snprintf(dst,
        dst_size, //
        R"({"name":"%s","flags":{"active":%d,"exists":%d,"is_due":%d},"attr":{"duration_s":%d,"adapter":%d,"flow_lph":%d,"priority":%d,"interval_s":%d},"state":{"last_time_wet":%llu,"next_time_scheduled":%llu}})", //
        name, //
        flags.active, flags.exists, flags.is_due, //
        attr.duration_s, attr.adapter, attr.flow_lph, attr.priority, attr.interval_s, //
        (long long unsigned) state.last_time_wet, (long long unsigned) state.next_time_scheduled //
        );
  }

  /*
   * \brief     initialize *this by default ctor and then get any values from parsed JSON object
   * \param it  Iterator pointing to the object token (JSMN_OBJECT)
   * \return
   */
  template<typename jsmn_iterator>
  bool from_json(jsmn_iterator &it) {
    assert(it->type == JSMN_OBJECT);

    // if JSON value is null, re-initialize object
    if ((it+1).value_equals_null()) {
      *this =  MagValve();
      return true;
    }

    return jsoneat::from_json::jsmn::deserialize_object(it, JSONEAT_KvPairs(name, flags, attr, state));
  }

};
