/**
 * \file   cli/cli_config.h
 * \brief  Options strings for CLI config command
 */

#pragma once
#include "app_config/proj_app_cfg.h"
#include "app/uout/status_output.h"

//key strings used for parsing and printing config commands by CLI/HTTP/MQTT
//keys must be in same order as their SO_CFG_xxx counterparts in so_msg_t
using cfg_keysT = const char *[SO_CFG_size];
constexpr cfg_keysT cfg_keys = { "rtc", "network", "wlan-ssid", "wlan-password", "lan-phy", "lan-pwr-gpio", "ntp-server", "tz", "verbose", "mqtt-enable",
    "mqtt-url", "mqtt-user", "mqtt-password", "mqtt-client-id", "http-enable", "http-user", "http-password", "stm32-bootgpio-inv" };



