#pragma once

#include <sdkconfig.h>

#ifdef CONFIG_APP_USE_HTTP
#ifdef __cplusplus
constexpr bool use_HTTP = true;
#else
#define use_HTTP 1
#endif
#else
#define NO_USE_HTTP
#ifdef __cplusplus
constexpr bool use_HTTP = false;
#else
#define use_HTTP 0
#endif
#endif
#ifdef CONFIG_APP_USE_LAN
#ifdef __cplusplus
constexpr bool use_LAN = true;
#else
#define use_LAN 1
#endif
#else
#define NO_USE_LAN
#ifdef __cplusplus
constexpr bool use_LAN = false;
#else
#define use_LAN 0
#endif
#endif
#ifdef CONFIG_APP_USE_WLAN
#ifdef __cplusplus
constexpr bool use_WLAN = true;
#else
#define use_WLAN 1
#endif
#else
#define NO_USE_WLAN
#ifdef __cplusplus
constexpr bool use_WLAN = false;
#else
#define use_WLAN 0
#endif
#endif
#ifdef CONFIG_APP_USE_WLAN_AP
#ifdef __cplusplus
constexpr bool use_WLAN_AP = true;
#else
#define use_WLAN_AP 1
#endif
#else
#define NO_USE_WLAN_AP
#ifdef __cplusplus
constexpr bool use_WLAN_AP = false;
#else
#define use_WLAN_AP 0
#endif
#endif
#ifdef CONFIG_APP_USE_NTP
#ifdef __cplusplus
constexpr bool use_NTP = true;
#else
#define use_NTP 1
#endif
#else
#define NO_USE_NTP
#ifdef __cplusplus
constexpr bool use_NTP = false;
#else
#define use_NTP 0
#endif
#endif
#ifdef CONFIG_APP_USE_JSON
#define  USE_JSON
#ifdef __cplusplus
constexpr bool use_JSON = true;
#else
#define use_JSON 1
#endif
#else
#define NO_USE_JSON
#ifdef __cplusplus
constexpr bool use_JSON = false;
#else
#define use_JSON 0
#endif
#endif
#ifdef CONFIG_APP_USE_MQTT
#ifdef __cplusplus
constexpr bool use_MQTT = true;
#else
#define use_MQTT 1
#endif
#else
#define NO_USE_MQTT
#ifdef __cplusplus
constexpr bool use_MQTT = false;
#else
#define use_MQTT 0
#endif
#endif
#ifdef CONFIG_APP_USE_POSIX_TIME
#define  USE_POSIX_TIME
#ifdef __cplusplus
constexpr bool use_POSIX_TIME = true;
#else
#define use_POSIX_TIME 1
#endif
#else
#define NO_USE_POSIX_TIME
#ifdef __cplusplus
constexpr bool use_POSIX_TIME = false;
#else
#define use_POSIX_TIME 0
#endif
#endif
#ifdef CONFIG_STM32_HAVE_COMPONENT
#define  USE_SERIAL
#ifdef __cplusplus
constexpr bool use_SERIAL = true;
#else
#define use_SERIAL 1
#endif
#else
#define NO_USE_SERIAL
#ifdef __cplusplus
constexpr bool use_SERIAL = false;
#else
#define use_SERIAL 0
#endif
#endif
#ifdef CONFIG_APP_USE_WDG
#define  USE_WDG
#ifdef __cplusplus
constexpr bool use_WDG = true;
#else
#define use_WDG 1
#endif
#else
#define NO_USE_WDG
#ifdef __cplusplus
constexpr bool use_WDG = false;
#else
#define use_WDG 0
#endif
#endif
#ifdef CONFIG_APP_USE_FS
#define  USE_FS
#ifdef __cplusplus
constexpr bool use_FS = true;
#else
#define use_FS 1
#endif
#else
#define NO_USE_FS
#ifdef __cplusplus
constexpr bool use_FS = false;
#else
#define use_FS 0
#endif
#endif
#ifdef CONFIG_APP_USE_HTTP_CLIENT
#define  USE_HTTP_CLIENT
#ifdef __cplusplus
constexpr bool use_HTTP_CLIENT = true;
#else
#define use_HTTP_CLIENT 1
#endif
#else
#define NO_USE_HTTP_CLIENT
#ifdef __cplusplus
constexpr bool use_HTTP_CLIENT = false;
#else
#define use_HTTP_CLIENT 0
#endif
#endif
#ifdef CONFIG_APP_USE_OTA
#define  USE_OTA
#ifdef __cplusplus
constexpr bool use_OTA = true;
#else
#define use_OTA 1
#endif
#else
#define NO_USE_OTA
#ifdef __cplusplus
constexpr bool use_OTA = false;
#else
#define use_OTA 0
#endif
#endif
#ifdef CONFIG_APP_USE_STM32OTA
#define  USE_STM32OTA
#ifdef __cplusplus
constexpr bool use_STM32OTA = true;
#else
#define use_STM32OTA 1
#endif
#else
#define NO_USE_STM32OTA
#ifdef __cplusplus
constexpr bool use_STM32OTA = false;
#else
#define use_STM32OTA 0
#endif
#endif
#ifdef CONFIG_APP_USE_TCPS
#define  USE_TCPS
#ifdef __cplusplus
constexpr bool use_TCPS = true;
#else
#define use_TCPS 1
#endif
#else
#define NO_USE_TCPS
#ifdef __cplusplus
constexpr bool use_TCPS = false;
#else
#define use_TCPS 0
#endif
#endif
#ifdef CONFIG_APP_USE_TCPS_TASK
#define  USE_TCPS_TASK
#ifdef __cplusplus
constexpr bool use_TCPS_TASK = true;
#else
#define use_TCPS_TASK 1
#endif
#else
#define NO_USE_TCPS_TASK
#ifdef __cplusplus
constexpr bool use_TCPS_TASK = false;
#else
#define use_TCPS_TASK 0
#endif
#endif
#ifdef CONFIG_APP_USE_EG
#define  USE_EG
#ifdef __cplusplus
constexpr bool use_EG = true;
#else
#define use_EG 1
#endif
#else
#define NO_USE_EG
#ifdef __cplusplus
constexpr bool use_EG = false;
#else
#define use_EG 0
#endif
#endif
#ifdef CONFIG_APP_USE_LPH
#ifdef __cplusplus
constexpr bool use_LPH = true;
#else
#define use_LPH 1
#endif
#else
#define NO_USE_LPH
#ifdef __cplusplus
constexpr bool use_LPH = false;
#else
#define use_LPH 0
#endif
#endif
#ifdef CONFIG_APP_USE_WS
#define  USE_WS
#ifdef __cplusplus
constexpr bool use_WS = true;
#else
#define use_WS 1
#endif
#else
#define NO_USE_WS
#ifdef __cplusplus
constexpr bool use_WS = false;
#else
#define use_WS 0
#endif
#endif
#ifdef CONFIG_APP_USE_CLI_MUTEX
#define  USE_CLI_MUTEX
#ifdef __cplusplus
constexpr bool use_CLI_MUTEX = true;
#else
#define use_CLI_MUTEX 1
#endif
#else
#define NO_USE_CLI_MUTEX
#ifdef __cplusplus
constexpr bool use_CLI_MUTEX = false;
#else
#define use_CLI_MUTEX 0
#endif
#endif
#ifdef CONFIG_APP_USE_NETWORK
#ifdef __cplusplus
constexpr bool use_NETWORK = true;
#else
#define use_NETWORK 1
#endif
#else
#define NO_USE_NETWORK
#ifdef __cplusplus
constexpr bool use_NETWORK = false;
#else
#define use_NETWORK 0
#endif
#endif
#ifdef CONFIG_APP_USE_STM32COM
#define  USE_STM32COM
#ifdef __cplusplus
constexpr bool use_STM32COM = true;
#else
#define use_STM32COM 1
#endif
#else
#define NO_USE_STM32COM
#ifdef __cplusplus
constexpr bool use_STM32COM = false;
#else
#define use_STM32COM 0
#endif
#endif
#ifdef CONFIG_APP_USE_AP_FALLBACK
#define  USE_AP_FALLBACK
#ifdef __cplusplus
constexpr bool use_AP_FALLBACK = true;
#else
#define use_AP_FALLBACK 1
#endif
#else
#define NO_USE_AP_FALLBACK
#ifdef __cplusplus
constexpr bool use_AP_FALLBACK = false;
#else
#define use_AP_FALLBACK 0
#endif
#endif
