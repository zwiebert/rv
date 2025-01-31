#pragma once
#include "adapter.hh"
#include <cstring>
#include <cstdio>
#include <ctime>
#include <jsmn/jsmn_iterate.hh>
#include <utils_time/ut_constants.hh>

struct MagValve {
  using self_type = MagValve;

  char name[CONFIG_APP_FA_NAMES_MAX_LEN] = "";

  struct {
    bool active = false;  ///< to mark as active (or not disabled temporarily)
    bool exists = false;   ///< to mark as non existent flat value in array
    bool is_due = false;
  } flags;

  struct {
    unsigned duration_s = 0;
    int8_t adapter = 0; ///< adapt duration (0 should always be a neutral adapter)
    unsigned flow_lph = 0;
    int priority = 0;
    unsigned interval_s = SECS_PER_DAY;
  } attr;

  struct {
    time_t last_time_wet = 0;
    time_t next_time_scheduled = 0;
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
   * \brief parse JSON and then calls this->from_json(JsmnBase::Iterator &it)
   */
  template<typename T, typename std::enable_if<!std::is_class<T> { }, bool>::type = true>
  bool from_json(T json) {
    auto jsmn = Jsmn<32, T>(json);

    if (!jsmn)
      return false;

    auto it = jsmn.begin();
    return from_json(it);
  }

  /*
   * \brief     initialize *this by default ctor and then get any values from parsed JSON object
   * \param it  Iterator pointing to the object token (JSMN_OBJECT)
   * \return
   */
  template<typename jsmn_iterator = Jsmn_String::Iterator, typename std::enable_if<std::is_class<jsmn_iterator> { }, bool>::type = true>
  bool from_json(jsmn_iterator &it) {
    assert(it->type == JSMN_OBJECT);

    using token_handler_fun_type = bool (*)(MagValve &self, jsmn_iterator &it, int &err);
    static const token_handler_fun_type tok_processRootChilds_funs[] = { //

        [](MagValve &self, jsmn_iterator &it, int &err) -> bool {
          if (it.keyIsEqual("flags", JSMN_OBJECT)) {
            auto count = it[1].size;
            for (it += 2; count > 0 && it; --count) {
              if (!(it.takeValue(self.flags.active, "active") //
              || it.takeValue(self.flags.exists, "exists") //
                  || it.takeValue(self.flags.is_due, "is_due"))) {
                ++err;
                it.skip_key_and_value();
              }
            }
            return true;
          }
          return false;
        },

        [](MagValve &self, jsmn_iterator &it, int &err) -> bool {
          if (it.keyIsEqual("attr", JSMN_OBJECT)) {
            auto count = it[1].size;
            for (it += 2; count > 0 && it; --count) {
              if (!(it.takeValue(self.attr.duration_s, "duration_s") //
              || it.takeValue(self.attr.adapter, "adapter") //
                  || it.takeValue(self.attr.flow_lph, "flow_lph") //
                  || it.takeValue(self.attr.priority, "priority") //
                  || it.takeValue(self.attr.interval_s, "interval_s") //
              )) {
                ++err;
                it.skip_key_and_value();
              }
            }
            return true;
          }
          return false;
        },

        [](MagValve &self, jsmn_iterator &it, int &err) -> bool {
          if (it.keyIsEqual("state", JSMN_OBJECT)) {
            auto count = it[1].size;
            for (it += 2; count > 0 && it; --count) {
              if (!(it.takeValue(self.state.last_time_wet, "last_time_wet") //
              || it.takeValue(self.state.next_time_scheduled, "next_time_scheduled") //
              )) {
                ++err;
                it.skip_key_and_value();
              }
            }
            return true;
          }
          return false;
        },

        [](self_type &self, jsmn_iterator &it, int &err) -> bool {
          if (it.takeValue(self.name, "name")) {
            return true;
          }
          return false;
        },

        [](self_type &self, jsmn_iterator &it, int &err) -> bool {
          return it.skip_key_and_value();
        } };

    int err = 0;
    auto count = it->size;
    for (++it; count > 0 && it; --count) {
      for (auto fun : tok_processRootChilds_funs) {
        if (fun(*this, it, err))
          break;
      }
    }
    return !err;

  }

};
