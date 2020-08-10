#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

struct lph_arg {
  int *lph_arr;
  int lph_arr_len;
};

bool decode_zoneData(uint8_t *msg_buf, unsigned msgBuf_len, struct lph_arg *lph_arg);
int encode_zoneData(uint8_t *msg_buf, size_t  msg_buf_len, struct lph_arg *lph_arg);


