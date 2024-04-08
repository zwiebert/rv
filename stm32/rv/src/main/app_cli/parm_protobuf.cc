/**
 * \file cli/parm_protobuf.cc
 * \brief handle cli command
 */
#include <peri/uart.h>
#include "user_config.h"
#include <time/real_time_clock.h>
#include <string.h>
#include "cli_imp.h"
#include <stdio.h>
#include "water_pump/water_pump.h"
#include "rv/rv_timer.hh"
#include "rv/rain_sensor.hh"
extern "C" {
#include <utils_misc/base64.h>
}
#include <malloc.h>
#include "protobuf/mcu_com.pb.h"
#include "protobuf/zone_data.hh"

extern bool got_zoneData;


#define warning_unknown_option(x)

#define KEY_ZONE_DATA "zd"

bool decode_zoneData(uint8_t *msg, unsigned msg_len);

int
process_parmProtoBuf(clpar p[], int len, class UoutWriter &td) {

  for (int arg_idx = 1; arg_idx < len; ++arg_idx) {
    const char *key = p[arg_idx].key, *val = p[arg_idx].val;

    if (key == NULL)
      return -1;

    bool requestData = false;
    uint8_t msgBuf[64];
    size_t msgBufLen = 0;

    if (std::strcmp("?", val) == 0)
      requestData = true;
    else {
      int err = mbedtls_base64_decode(msgBuf, sizeof msgBuf, &msgBufLen, (const uint8_t *)val, std::strlen(val));
      if (err)
        continue;
    }
#if 0
    if (std::strcmp(key, KEY_ZONE_DATA) == 0) {
      struct zd_arg lph_arg = { .lph_arr = Lph, .lph_arr_len = RV_VALVE_COUNT };
      if (decode_zoneData(msgBuf, msgBufLen, &lph_arg))
        got_zoneData = true;
      warning_unknown_option(key);
    }
#endif

  }



  return 0;
}
