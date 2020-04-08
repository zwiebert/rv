/*
 * status_output.h
 *
 *  Created on: 13.03.2019
 *      Author: bertw
 */

#ifndef USER_MAIN_STATUS_OUTPUT_H_
#define USER_MAIN_STATUS_OUTPUT_H_

#include <stdint.h>
#include <stdbool.h>
#include "userio/status_output.h"


#define SO_CFG_size (SO_CFG_end - SO_CFG_begin - 1)


typedef enum {
  SO_NONE,

  SO_FW_START_MSG_PRINT, //1


  SO_CFG_all, SO_CFG_begin, //13
  SO_CFG_RTC,   SO_CFG_NETWORK,
  SO_CFG_WLAN_SSID, SO_CFG_WLAN_PASSWORD,
  SO_CFG_LAN_PHY, SO_CFG_LAN_PWR_GPIO,
  SO_CFG_NTP_SERVER, //19
  SO_CFG_TZ, SO_CFG_VERBOSE, //25
  SO_CFG_MQTT_ENABLE, SO_CFG_MQTT_URL, SO_CFG_MQTT_USER, SO_CFG_MQTT_PASSWORD, //29
  SO_CFG_HTTP_ENABLE, SO_CFG_HTTP_USER, SO_CFG_HTTP_PASSWORD, //23
  SO_CFG_STM32_BOOTGPIO_INV,
  SO_CFG_end, // values between begin/end will be used for 'all=?'
  SO_CFG_GPIO_PIN,

  SO_KVS_begin, SO_KVS_ZN_ALL, SO_KVS_end,  SO_KVS_ZN_SINGLE,

  SO_CFGPASSWD_OK, SO_CFGPASSWD_WRONG, SO_CFGPASSWD_MISSING,

  SO_INET_PRINT_ADDRESS,

  SO_CMD_begin,
  SO_CMD_ZONE_ON, SO_CMD_ZONE_OFF,
  SO_CMD_end,

  SO_MCU_begin, SO_MCU_RUN_TIME, SO_MCU_VERSION, SO_MCU_OTA, SO_MCU_OTA_STATE, SO_MCU_STM32OTA, SO_MCU_STM32OTA_STATE, SO_MCU_BOOT_COUNT, SO_MCU_end,

  SO_STATUS_OK, SO_STATUS_ERROR,


} so_msg_t;

void so_output_message(so_msg_t mt, const void *arg);
so_msg_t so_parse_config_key(const char *k);


#endif /* USER_MAIN_STATUS_OUTPUT_H_ */
