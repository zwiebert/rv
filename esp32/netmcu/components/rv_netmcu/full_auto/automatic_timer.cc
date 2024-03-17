#include <full_auto/automatic_timer.hh>
#include <key_value_store/kvs_wrapper.h>

static constexpr char kvs_name[] = "full_auto";

bool AutoTimer::save_this(const char *key) {
  bool result = false;
  if (auto h = kvs_open(kvs_name, kvs_WRITE)) {
    if (kvs_set_blob(h, key, this, sizeof *this)) {
      result = true;
    }
    kvs_commit(h);
    kvs_close(h);
  }

  return result;

}
bool AutoTimer::restore_this(const char *key) {
  bool result = false;
  if (auto h = kvs_open(kvs_name, kvs_READ)) {
    if (kvs_get_blob(h, key, this, sizeof *this)) {
      result = true;
    }
    kvs_close(h);
  }
  return result;
}

template<typename T>
int to_json_tmpl(char *buf, size_t buf_size, int &obj_ct, T *arr, size_t arr_size) {
  int bi = 0;
  const int leave_space = 150;
  for (int &i = obj_ct; leave_space < (buf_size - bi) && i < arr_size; ++i) {
    buf[bi++] = i == 0 ? '[' : ',';
    const auto &v = arr[i];

    if (v.flags.exists) {
      if (int br = v.to_json(buf + bi, buf_size - bi); br)
        bi += br;
      else
        return br - 1; // remove last comma (or bracket)
    } else {
      memcpy(&buf[bi], "null", 4), (bi += 4);
    }

    if (i + 1 == arr_size) {
      buf[bi++] = ']';
      buf[bi++] = ',';
    }
  }

  return bi;
}

int AutoTimer::to_json(char *buf, size_t buf_size, int &obj_ct) {
  int bi = 0;
  const int leave_space = 150;
  const int TOTAL_OBJS = CONFIG_APP_NUMBER_OF_VALVES + CONFIG_APP_FA_MAX_VALVE_GROUPS + CONFIG_APP_FA_MAX_WEATHER_ADAPTERS;
  if (obj_ct < 0)
    return 0;  // EOF already reached

  if (obj_ct == 0)
    buf[bi++] = '{';

  // 1st object: m_valves
  int obj_rel_idx = obj_ct;

  if (0 <= obj_rel_idx && obj_rel_idx < CONFIG_APP_NUMBER_OF_VALVES) {
    int ct = obj_rel_idx;
    if (ct == 0)
      memcpy(buf + bi, R"("valves":)", 9), bi += 9;
    bi += to_json_tmpl(buf + bi, buf_size - bi, ct, &m_valves[0], CONFIG_APP_NUMBER_OF_VALVES);
    obj_ct += ct - obj_rel_idx;
    if (leave_space < buf_size - bi)
      return bi;
  }


  obj_rel_idx -= CONFIG_APP_NUMBER_OF_VALVES;

  if (0 <= obj_rel_idx && obj_rel_idx < CONFIG_APP_FA_MAX_VALVE_GROUPS) {
    int ct = obj_rel_idx;
    if (ct == 0)
      memcpy(buf + bi, R"("valve_groups":)", 15), bi += 15;
    bi += to_json_tmpl(buf + bi, buf_size - bi, ct, &m_valveGroups[0], CONFIG_APP_FA_MAX_VALVE_GROUPS);
    obj_ct += ct - obj_rel_idx;
    if (leave_space < buf_size - bi)
      return bi;
  }

  obj_rel_idx -= CONFIG_APP_FA_MAX_VALVE_GROUPS;

  if (0 <= obj_rel_idx && obj_rel_idx < CONFIG_APP_FA_MAX_WEATHER_ADAPTERS) {
    int ct = obj_rel_idx;
    if (ct == 0)
      memcpy(buf + bi, R"("adapters":)", 11), bi += 11;
    bi += to_json_tmpl(buf + bi, buf_size - bi, ct, &m_adapters[0], CONFIG_APP_FA_MAX_WEATHER_ADAPTERS);
    obj_ct += ct - obj_rel_idx;
    if (ct == 0)
      bi -= 11; // remove key again
  }

  if (obj_ct >= TOTAL_OBJS) {
    obj_ct = -1; // signal we are done
    buf[bi - 1] = '}';
  }

  return bi;
}
