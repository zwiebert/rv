#include "rv_timer.hh"
#include <uout/uout_builder_json.hh>
#include <stdlib.h>



bool RvTimerData::to_json(UoutBuilderJson &sj)  const {
  const SetArgs &args = mArgs;

  sj.add_object();

  sj.put_kv("nr", mNextRun);
  sj.put_kv("lr", mLastRun);

  args.to_json(sj, true);

  sj.close_object();

  return true;
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
