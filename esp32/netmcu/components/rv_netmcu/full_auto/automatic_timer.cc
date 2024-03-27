#include <full_auto/automatic_timer.hh>
#include <key_value_store/kvs_wrapper.h>
#include <cstdio>

#include <debug/log.h>
#ifdef CONFIG_RV_NETMCU_DEBUG
#define DEBUG
#define D(x) x
#else
#define D(x)
#endif
#define logtag "rv.full_auto"

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

  done: auto objs_done = rel_obj_idx - old_rel_obj_idx;
  if (!objs_done && key_len)
    bi -= key_len;

  obj_ct += objs_done;
  return bi;
}

int AutoTimer::to_json(char *buf, size_t buf_size, int &obj_ct) {
  int bi = 0;
  if (obj_ct < 0) // Check EOF
    return 0;
  if (obj_ct == 0)
    buf[bi++] = '{';


  int obj_rel_idx = obj_ct;


  bi += to_json_tmpl(buf + bi, buf_size - bi, obj_ct, obj_rel_idx, &m_magval[0], CONFIG_APP_NUMBER_OF_VALVES, "valves");
  obj_rel_idx -= CONFIG_APP_NUMBER_OF_VALVES;


  bi += to_json_tmpl(buf + bi, buf_size - bi, obj_ct, obj_rel_idx, &m_adapters[0], CONFIG_APP_FA_MAX_WEATHER_ADAPTERS, "adapters");

  if (obj_ct >= TOTAL_OBJS) {
    buf[bi - 1] = '}';
    obj_ct *= -1; // Set EOF (minus sign on obj_ct)
  }

  return bi;
}

#include <debug/dbg.h>

void AutoTimer::dev_random_fill_data() {
  auto tnow = time(0);
  for (auto &o : m_magval) {
    snprintf(o.name, sizeof o.name, "ObjectName-%d", rando(100, 1000));
    o.flags.exists = rando(0, 2);
    o.flags.active = rando(0, 2);
    o.state.last_time_wet = rando(tnow - SECS_PER_DAY * 7, tnow - SECS_PER_DAY);
    //o.state.next_time_scheduled = rando(tnow + SECS_PER_HOUR, tnow + SECS_PER_HOUR * 2);
  }
  for (auto &o : m_adapters) {
    if (o.flags.read_only)
      continue;
    snprintf(o.name, sizeof o.name, "ObjectName-%d", rando(100, 1000));
    o.flags.exists = rando(0, 2);
  }
}


void AutoTimer::todo_loop() {
  // TODO: the factor should be valve dependent (dry_time as parameter)
  m_f = m_wi ? m_wi->get_simple_irrigation_factor(36) : 1.0;

  // first pass: mark all due valves with flag.is_due
  for (MagValve &mv : m_magval) {
     mv.flags.is_due = should_valve_be_due(mv, time(0));
  }
  sort_magval_idxs();
  D(db_logi(logtag, "used_valves_count=%d, due_valves_count=%u", m_used_valves_count, m_due_valves_count));

  for (auto ip : m_magval_due_idxs) {
     auto &v = m_magval[ip.idx];
     if (!v.flags.exists || !v.flags.is_due)
       break;
     D(db_logi(logtag, "Schedule valve number %d (%s). prio=%d", ip.idx, v.name, ip.prio));
  }
}
