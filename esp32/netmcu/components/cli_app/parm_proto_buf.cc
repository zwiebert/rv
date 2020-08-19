extern "C" {
#include "app_config/proj_app_cfg.h"
#include "txtio/inout.h"
#include "net/mqtt/app/mqtt.h"
#include "app_config/proj_app_cfg.h"
#include "cli_imp.h"
#include "cli/cli.h"
#include "userio_app/status_output.h"
#include "userio/status_json.h"
#include "debug/debug.h"
#include "misc/int_macros.h"
#include <stm32/stm32.h>
#include "misc/base64.h"
}

#include <malloc.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>

#include "pb_decode.h"
#include "pb_encode.h"
#include "protobuf/mcu_com.pb.h"
#include "protobuf/zone_data.hh"

#define warning_unknown_option(x)

#define KEY_ZONE_DATA "zd"

#define RV_VALVE_COUNT 14



#define LPH_10VAN_90 140
#define LPH_10VAN_180 290
#define LPH_12VAN_90 130
#define LPH_12VAN_180 260
#define LPH_15LCS 111
#define LPH_15SST 250

#define LPH_XS 93
#define LPH_SXB360 50

#define LPH_LAWN_WEST (6 * LPH_12VAN_90 + 3 * LPH_12VAN_180)  // 1560
#define LPH_LAWN_EAST (4 * LPH_12VAN_90 + 2 * LPH_15LCS + 2 * LPH_15SST) // 1242

#define LPH_HORTENS (1 * LPH_XS) // 93
#define LPH_FLOWERS_SOUTH (9 * LPH_XS)  // 837
#define LPH_FLOWERS_WEST (11 * LPH_XS) // 1023
#define LPH_POTS_NORTH (7 * LPH_SXB360 / 2) // 175 (choked to 1/2)

int Lph[RV_VALVE_COUNT] = {
    LPH_LAWN_WEST, //0
    LPH_HORTENS, //1
    LPH_FLOWERS_SOUTH, //2
    LPH_LAWN_EAST, //3
    1000, //4
    1000, //5
    1000, //6
    1000, //7
    1000, //8
    LPH_FLOWERS_WEST, //9
    1000, //10
    LPH_POTS_NORTH, //11
    };

static void stm32Send_zoneData() {
#define BUF_SIZE 128
  char buf[BUF_SIZE] = "";
  uint8_t msgBuf[128];
  struct lph_arg lph_arg = { .lph_arr = Lph, .lph_arr_len = RV_VALVE_COUNT };
  int msgBufLen = encode_zoneData(msgBuf, sizeof(msgBuf), &lph_arg);

  if (msgBufLen > 0) {
    uint8_t b64[64];
    size_t b64Len = 0;

    int err = mbedtls_base64_encode(b64, sizeof b64, &b64Len, msgBuf, msgBufLen);
    if (!err) {
      io_printf("msgBufLen=%d, zd=<%*s>\n", msgBufLen, b64Len, b64);
      snprintf(buf, BUF_SIZE, "{\"pbuf\":{\"zd\":\"%*s\"}};\n", b64Len, b64);
      stm32_write(buf, strlen(buf));
    }
  }
}

extern "C" int
process_parmProtoBuf(clpar p[], int len) {

  so_output_message(SO_PBUF_begin, NULL);

  for (int arg_idx = 1; arg_idx < len; ++arg_idx) {
    const char *key = p[arg_idx].key, *val = p[arg_idx].val;

    if (key == NULL)
      return -1;

    bool requestData = false;
    uint8_t msgBuf[64];
    size_t msgBufLen = 0;


    if (strcmp("?", val) == 0)
      requestData = true;
    else {
      int err = mbedtls_base64_decode(msgBuf, sizeof msgBuf, &msgBufLen, (const uint8_t *)val, strlen(val));
      if (err)
        continue;
    }

    if (strcmp(key, KEY_ZONE_DATA) == 0) {
      if (requestData) {
        if (so_tgt_test(SO_TGT_STM32)) { //XXX
          stm32Send_zoneData();
        } else {
          uint8_t msgBuf[128];
          struct lph_arg lph_arg = { .lph_arr = Lph, .lph_arr_len = RV_VALVE_COUNT };
          int msgBufLen = encode_zoneData(msgBuf, sizeof(msgBuf), &lph_arg);
          so_arg_pbuf_t pba = { .key = "zd", .buf = msgBuf, .buf_len = msgBufLen };
          so_output_message(SO_PBUF_KV64, &pba);
        }
      } else {
        struct lph_arg lph_arg = { .lph_arr = Lph, .lph_arr_len = RV_VALVE_COUNT };
        decode_zoneData(msgBuf, msgBufLen, &lph_arg);
      }
    } else {
      warning_unknown_option(key);
    }
  }

  so_output_message(SO_PBUF_end, NULL);

  return 0;
}



