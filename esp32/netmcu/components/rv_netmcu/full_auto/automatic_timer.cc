#include <full_auto/automatic_timer.hh>
#include <key_value_store/kvs_wrapper.h>

#include <cstdio>

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
int to_json_tmpl(char *buf, size_t buf_size, int &obj_ct, int &rel_obj_idx, T *arr, size_t arr_size, const char *key) {
  const int leave_space = 150;
  if (!(0 <= rel_obj_idx && rel_obj_idx < arr_size && leave_space < buf_size))
    return 0;

  int bi = 0;
  int key_len = 0;
  const int old_rel_obj_idx = rel_obj_idx;

  if (rel_obj_idx == 0) {
    bi += key_len = snprintf(buf + bi, buf_size - bi, R"("%s":)", key);
  }

  for (int &i = rel_obj_idx; leave_space < (buf_size - bi) && i < arr_size; ++i) {
    buf[bi++] = i == 0 ? '[' : ',';
    const auto &v = arr[i];

    if (v.flags.exists) {
      if (int br = v.to_json(buf + bi, buf_size - bi); br)
        bi += br;
      else {
        --bi;  // remove last comma (or bracket)
        goto done;
      }
    } else {
      memcpy(&buf[bi], "null", 4), (bi += 4);
    }

    if (i + 1 == arr_size) {
      buf[bi++] = ']';
      buf[bi++] = ',';
    }
  }

  done: obj_ct += rel_obj_idx - old_rel_obj_idx;
  return bi;
}

int AutoTimer::to_json(char *buf, size_t buf_size, int &obj_ct) {
  int bi = 0;
  const int TOTAL_OBJS = CONFIG_APP_NUMBER_OF_VALVES + CONFIG_APP_FA_MAX_VALVE_GROUPS + CONFIG_APP_FA_MAX_WEATHER_ADAPTERS;
  int obj_rel_idx = obj_ct;

  if (obj_ct < 0) // Check EOF
    return 0;

  if (obj_ct == 0)
    buf[bi++] = '{';

  bi += to_json_tmpl(buf + bi, buf_size - bi, obj_ct, obj_rel_idx, &m_valves[0], CONFIG_APP_NUMBER_OF_VALVES, "valves");
  obj_rel_idx -= CONFIG_APP_NUMBER_OF_VALVES;

  bi += to_json_tmpl(buf + bi, buf_size - bi, obj_ct, obj_rel_idx, &m_valveGroups[0], CONFIG_APP_FA_MAX_VALVE_GROUPS, "valve_groups");
  obj_rel_idx -= CONFIG_APP_FA_MAX_VALVE_GROUPS;

  bi += to_json_tmpl(buf + bi, buf_size - bi, obj_ct, obj_rel_idx, &m_adapters[0], CONFIG_APP_FA_MAX_WEATHER_ADAPTERS, "adapters");

  if (obj_ct >= TOTAL_OBJS) {
    buf[bi - 1] = '}';
    obj_ct = -1; // Set EOF
  }

  return bi;
}
