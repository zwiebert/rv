#pragma once
/**
 * \file app_uout/callbacks.h
 * \brief pub/sub interface to publish asynchronously to MQTT servers, HTTP-client web-sockets, USB-console, TCP-sockets/consoles
 */

#include <stdint.h>
#include <stddef.h>
#include <app_uout/status_output.h>
#include <uout/uo_callbacks.h>
#include <type_traits>

struct so_arg_valve_state_t {
  uint8_t valve_number;
  bool is_open;
};


inline const so_arg_valve_state_t *uoCb_valveState_FromMsg(const uoCb_msgT msg) {
  if (msg.flags.evt.valve_change && msg.flags.fmt.raw)
    return static_cast<const so_arg_valve_state_t *>(msg.cptr);
  return nullptr;
}

void uoAPp_publish_valve_change(so_arg_valve_state_t vs);
