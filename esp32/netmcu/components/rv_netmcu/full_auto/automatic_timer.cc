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

