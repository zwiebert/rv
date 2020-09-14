/*
 * config.c
 *
 *  Created on: 10.09.2018
 *      Author: bertw
 */


/*
 * config_defaults.c
 *
 *  Created on: 16.09.2017
 *      Author: bertw
 */
#include "app_config/proj_app_cfg.h"
#include "config/config.h"
#include "config/config_defaults.h"
#include "misc/base64.h"

#define D(x) 

#define CI(cb) static_cast<configItem>(cb)
config C = {

};

#ifdef USE_NETWORK
enum nwConnection config_read_network_connection() {
  return static_cast<enum nwConnection>(config_read_item_i8(CI(CB_NETWORK_CONNECTION), MY_NETWORK_CONNECTION));
}
#endif

#ifdef POSIX_TIME
const char* config_read_tz(char *d, unsigned d_size) {
  return config_read_item_s(CI(CB_TZ), d, d_size, MY_GEO_TZ);
}
#endif

bool config_read_stm32_inv_bootpin() {
  return !!config_read_item_i8(CI(CB_STM32_INV_BOOTPIN), MY_STM32_INV_BOOTPIN);
}

#ifdef USE_LPH
bool config_read_lph(uint16_t lph[14]) {
  return config_read_item_b(CI(CB_LPH), lph, sizeof(uint16_t) * 14, 0) != 0;
}
bool config_save_lph(uint16_t lph[14]) {
  return config_save_item_b(CI(CB_LPH), lph, sizeof(uint16_t) * 14);
}
#endif


bool config_save_pb64(enum configItem item, const char *pb64) {
  return config_save_item_s(item, pb64);
}

bool config_read_pb64(enum configItem item, char *pb64_buf, size_t buf_size) {
  return config_read_item_s(item, pb64_buf, buf_size, "");
}

bool config_save_pb(enum configItem item, const uint8_t *pb, size_t pb_len) {
  unsigned char pb64[pb_len/2 + pb_len];

  int err = mbedtls_base64_encode(pb64, sizeof pb64, &pb_len, pb, pb_len);
  if (err)
    return false;
  return config_save_pb64(item, (const char *)pb64);
}

bool config_read_pb(enum configItem item, uint8_t *pb_buf, size_t *pb_size) {

  if (!config_read_pb64(item, (char *)pb_buf, *pb_size))
    return false;

  int err = mbedtls_base64_decode(pb_buf, *pb_size, pb_size, pb_buf, strlen((char*)pb_buf));

  if (err)
    return false;

  return true;
}
