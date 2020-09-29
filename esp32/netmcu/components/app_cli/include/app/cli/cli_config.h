#pragma once
#include "app/config/proj_app_cfg.h"
#include "uout_app/status_output.h"

//key strings used for parsing and printing config commands by CLI/HTTP/MQTT
//keys must be in same order as their SO_CFG_xxx counterparts in so_msg_t
using cfg_keysT = const char *[SO_CFG_size];
constexpr cfg_keysT cfg_keys = { "rtc", "network", "wlan-ssid", "wlan-password", "lan-phy", "lan-pwr-gpio", "ntp-server", "tz", "verbose", "mqtt-enable",
    "mqtt-url", "mqtt-user", "mqtt-password", "mqtt-client-id", "http-enable", "http-user", "http-password", "stm32-bootgpio-inv" };

#ifdef USE_NETWORK
#include <app/settings/config.h>
using cfg_args_networkT = const char *[nwLEN];
constexpr cfg_args_networkT cfg_args_network = { "none", "wlan", "ap", "lan" };
#endif

#ifdef USE_LAN
#include <net/ethernet.h>
using cfg_args_lanPhyT = const char *[lanPhyLEN];
constexpr cfg_args_lanPhyT cfg_args_lanPhy = { "none", "lan8270", "rtl8201", "ip101", };
#endif
