
#include <misc/int_macros.h>
#include <misc/cstring_utils.hh>
#include <cli/mutex.hh>
#include <txtio/txtio_mutex.hh>
#include <app/uout/so_msg.h>
#include <app/uout/so_types.h>
#include <uout/status_json.hh>
#include <stdio.h>
#include <string.h>
#include <debug/dbg.h>

#define UOUT_PROTECTED
#include <uout/callbacks.h>
#include <app/uout/callbacks.h>


static void publish(uoCb_cbT cb, const void *ptr, uo_flagsT flags) {
  std::scoped_lock lock { cli_mutex, txtio_mutex };
  uoCb_msgT  msg { .cv_ptr = ptr, .flags = flags };
  cb(msg);
}


void uoAPp_publish_valve_change(so_arg_valve_state_t vs) {
  for (auto const &it : uoCb_cbs) {
    if (!it.cb)
      continue;
    if (!it.flags.evt.valve_change)
      continue;
    //TODO: add json output for this (RF msg received)

    if (it.flags.fmt.obj) {
      uo_flagsT flags;
      flags.fmt.obj = true;
      flags.evt.valve_change = true;
      publish(it.cb, &vs, flags);
    }

    if (it.flags.fmt.json) {
      uo_flagsT flags;
      flags.fmt.json = true;
      flags.evt.valve_change = true;

      char buf[64];
      snprintf(buf, sizeof buf, "{\"valve\":{\"v%d\":%d}}", vs.valve_number, vs.is_open);
      publish(it.cb, buf, flags);
    }
  }
}
