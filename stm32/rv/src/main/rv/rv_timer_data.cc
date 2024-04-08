#include "rv_timer.hh"
#include <uout/uout_builder_json.hh>
#include <stdlib.h>

RvTimerData::SetArgs::SetArgs(cstr_pair kvs[], int len) {

  SetArgs &args = *this;

  for (int arg_idx = 0; arg_idx < len; ++arg_idx) {
    const char *key = kvs[arg_idx].first, *val = kvs[arg_idx].second;

    if (key == NULL) {
      break;
    } else if (std::strcmp(key, "vn") == 0) {
      args.valve_number = atoi(val);
    } else if (std::strcmp(key, "tn") == 0) {
      args.timer_number = atoi(val);
    } else if (std::strcmp(key, "d1") == 0) {
      args.on_duration = atoi(val);
    } else if (std::strcmp(key, "d0") == 0) {
      args.off_duration = atoi(val);
    } else if (std::strcmp(key, "r") == 0) {
      args.repeats = atoi(val);
    } else if (std::strcmp(key, "per") == 0) {
      args.period = atoi(val);
    } else if (std::strcmp(key, "di") == 0) {
      args.mDaysInterval = atoi(val);
    } else if (std::strcmp(key, "sb") == 0) {
      args.mTodSpanBegin = atoi(val);
    } else if (std::strcmp(key, "se") == 0) {
      args.mTodSpanEnd = atoi(val);
    } else if (std::strcmp(key, "ir") == 0) {
      args.ignoreRainSensor = std::strcmp("true", val) == 0;
    } else if (std::strcmp(key, "ip") == 0) {
      args.ignorePumpPause = std::strcmp("true", val) == 0;
    }
  }
}

bool RvTimerData::to_json(UoutBuilderJson &sj)  const {
  const SetArgs &args = mArgs;

  sj.add_object();

  sj.put_kv("nr", mNextRun);
  sj.put_kv("lr", mLastRun);

  args.to_json(sj, true);

  sj.close_object();

  return true;
}

bool RvTimerData::SetArgs::to_json(UoutBuilderJson &sj, bool content_only) const {
  const SetArgs &args = *this;

  if (!content_only)
  sj.add_object();

  sj.put_kv("vn", args.valve_number);

  if (args.timer_number) {
    sj.put_kv("tn", args.timer_number);
  }

  if (args.on_duration) {
    sj.put_kv("d1", args.on_duration);
  }

  if (args.off_duration) {
    sj.put_kv("d0", args.off_duration);
  }

  if (args.repeats) {
    sj.put_kv("r", args.repeats);
  }

  if (args.period) {
    sj.put_kv("per", args.period);
  }

  if (args.mDaysInterval) {
    sj.put_kv("di", args.mDaysInterval);
  }

  if (args.mTodSpanBegin) {
    sj.put_kv("sb", args.mTodSpanBegin);
  }

  if (args.mTodSpanEnd) {
    sj.put_kv("se", args.mTodSpanEnd);
  }

  if (args.ignoreRainSensor) {
    sj.put_kv("ir", true);
  }

  if (args.ignorePumpPause) {
    sj.put_kv("ip", true);
  }

  if (!content_only)
    sj.close_object();

  return true;
}

char* RvTimerData::SetArgs::toJSON(char *buf, size_t buf_size, bool no_brackets) const {
  const SetArgs &args = *this;
  char *result = buf;

  if (!no_brackets) {
    *buf++ = '{';
    --buf_size;
  }

  {
    const int n = std::snprintf(buf, buf_size, "\"vn\":%d,", args.valve_number);
    if (n < 0)
      return 0;
    buf_size -= n;
    buf += n;
  }

  if (args.timer_number) {
    const int n = std::snprintf(buf, buf_size, "\"tn\":%d,", args.timer_number);
    if (n < 0)
      return 0;
    buf_size -= n;
    buf += n;
  }

  if (args.on_duration) {
    const int n = std::snprintf(buf, buf_size, "\"d1\":%d,", args.on_duration);
    if (n < 0)
      return 0;
    buf_size -= n;
    buf += n;
  }

  if (args.off_duration) {
    const int n = std::snprintf(buf, buf_size, "\"d0\":%d,", args.off_duration);
    if (n < 0)
      return 0;
    buf_size -= n;
    buf += n;
  }

  if (args.repeats) {
    const int n = std::snprintf(buf, buf_size, "\"r\":%d,", args.repeats);

    if (n < 0)
      return 0;
    buf_size -= n;
    buf += n;
  }

  if (args.period) {
    const int n = std::snprintf(buf, buf_size, "\"per\":%d,", args.period);
    if (n < 0)
      return 0;
    buf_size -= n;
    buf += n;
  }

  if (args.mDaysInterval) {
    const int n = std::snprintf(buf, buf_size, "\"di\":%d,", args.mDaysInterval);
    if (n < 0)
      return 0;
    buf_size -= n;
    buf += n;
  }

  if (args.mTodSpanBegin) {
    const int n = std::snprintf(buf, buf_size, "\"sb\":%d,", args.mTodSpanBegin);
    if (n < 0)
      return 0;
    buf_size -= n;
    buf += n;
  }

  if (args.mTodSpanEnd) {
    const int n = std::snprintf(buf, buf_size, "\"se\":%d,", args.mTodSpanEnd);
    if (n < 0)
      return 0;
    buf_size -= n;
    buf += n;
  }
  if (args.ignoreRainSensor) {
    const int n = std::snprintf(buf, buf_size, "\"ir\":true,");
    if (n < 0)
      return 0;
    buf_size -= n;
    buf += n;
  }

  if (args.ignorePumpPause) {
    const int n = std::snprintf(buf, buf_size, "\"ip\":true,");
    if (n < 0)
      return 0;
    buf_size -= n;
    buf += n;
  }

  buf[-1] = no_brackets ? '\0' : '}';

  return result;
}

char* RvTimerData::toJSON(char *buf, size_t buf_size, bool no_brackets) const {
  const SetArgs &args = mArgs;
  char *result = buf;

  if (!no_brackets) {
    *buf++ = '{';
    --buf_size;
  }

  {
    const int n = std::snprintf(buf, buf_size, "\"nr\":%d,\"lr\":%d,", static_cast<int>(mNextRun), static_cast<int>(mLastRun));
    if (n < 0)
      return 0;
    buf_size -= n;
    buf += n;
  }

  args.toJSON(buf, buf_size - 1, true);

  if (!no_brackets)
    std::strcat(buf, "}");

  return result;
}
