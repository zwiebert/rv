
#include <utils_misc/int_macros.h>
#include <utils_misc/cstring_utils.hh>
#include <cli/mutex.hh>
#include <txtio/txtio_mutex.hh>
#include <app_uout/so_msg.h>
#include <app_uout/so_types.h>
#include <uout/status_json.hh>
#include <stdio.h>
#include <string.h>
#include <debug/dbg.h>

#define UOUT_PROTECTED
#include <uout/uo_callbacks.h>
#include <app_uout/callbacks.h>


void uoAPp_publish_valve_change(so_arg_valve_state_t vs) {
  uo_flagsT flags;
  flags.evt.valve_change = true;

  flags.fmt.raw = true;
  if (auto idxs = uoCb_filter(flags); idxs.size) {
    uoCb_publish(idxs, &vs, flags);
  }

  flags.fmt.raw = false;
  flags.fmt.json = true;
  if (auto idxs = uoCb_filter(flags); idxs.size) {
    char buf[64];
    snprintf(buf, sizeof buf, "{\"valve\":{\"v%d\":%d}}", vs.valve_number, vs.is_open);
    uoCb_publish(idxs, buf, flags);
  }
}

