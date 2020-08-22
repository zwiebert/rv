#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

struct zd_arg {
  int *lph_arr;
  int lph_arr_len;
};

bool decode_zoneData(const uint8_t *src_buf, size_t src_buf_len, const struct zd_arg *dst_zd_arg);
int encode_zoneData(uint8_t *dst_msg_uf, size_t  dst_msg_buf_len, const struct zd_arg *zd_arg);


