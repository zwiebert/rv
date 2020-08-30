#include "app_config/proj_app_cfg.h"
#include "userio_app/status_output.h"
#include "config/config.h"
#include "app/rtc.h"
#include "cli_imp.h"
#include "cli/cli.h"
#include "net/mqtt/app/mqtt.h"
#include "net/http/server/http_server.h"
#include "app/kvstore.h"
#include "misc/int_macros.h"
#include "misc/int_types.h"
#include "app/loop.h"
#define ENABLE_RESTART 1 // allow software reset

const char *cfg_keys[SO_CFG_size] = { "rtc", "network", "wlan-ssid", "wlan-password", "lan-phy", "lan-pwr-gpio", "ntp-server", "tz", "verbose", "mqtt-enable",
    "mqtt-url", "mqtt-user", "mqtt-password", "mqtt-client-id", "http-enable", "http-user", "http-password", "stm32-bootgpio-inv" };

#ifdef USE_NETWORK
const char *cfg_args_network[nwLEN] = { "none", "wlan", "ap", "lan" };
#endif

#ifdef CONFIG_GPIO_SIZE
//PIN_DEFAULT=0, PIN_INPUT, PIN_INPUT_PULLUP, PIN_OUTPUT, PIN_ERROR, PIN_READ, PIN_CLEAR, PIN_SET, PIN_TOGGLE

const char pin_state_args[] = "dipo ?01t";
#endif

const char cli_help_parmConfig[] = "'config' sets or gets options. Use: config option=value ...; to set. Use: config option=? ...; to get, if supported\n\n"
    "rtc=(ISO_TIME|?)   set local time it NTP is not working.;\n"
#ifdef USE_NETWORK
    "network=(wlan|ap|lan) ap opens wlan access-point at 192.168.4.1 (restart required)\n"
#endif
#ifdef USE_WLAN
    "wlan-ssid=(SSID|?)\n"
    "wlan-password=PW\n"
#endif
#ifdef USE_NTP
    "ntp-server=(dhcp|IP4ADDR|NAME) default: ntp.pool.org (TODO: implement list of servers)\n"
#endif
#ifdef USE_MQTT
    "mqtt-enable=(0|1) enable MQTT\n"
    "mqtt-url=URL      broker/server URL (e.g. mqtt://192.168.1.42:7777)\n"
    "mqtt-user=NAME    user name on server\n"
    "mqtt-password=PW  user password on server\n"
    "mqtt-client-id=CID  default: rv\n"
#endif
#ifdef USE_HTTP
    "http-enable=(0|1) enable HTTP\n"
    "http-user=NAME    user name on server\n"
    "http-password=PW  user password on server\n"
#endif
#ifdef POSIX_TIME
    "tz=(POSIX_TZ|?)    time zone for RTC/NTP\n"
#endif
    "verbose=(0..5|?)   diagnose output verbosity level\n"
    "set-pw=password    set a config password. if set every config commands needs the pw option\n"
    "pw=PW              example: config pw=my_passw dst=eu;\n"
#if ENABLE_RESTART
    "restart            restart MCU\n"
#endif
#ifdef ACCESS_GPIO
    "gpioN=(i|p|o|0|1|d|?) Set gpio pin as input (i,p) or output (o,0,1) or use default\n"
#endif
    "stm32-bootgpio-inv   Invert Boot-Pin for STM32 OTA\n"
//  "set-expert-password=\n"
;

#define isValid_optStr(cfg, new) true
#define set_optStr(v, cb) config_save_item_s(static_cast<configItem>(cb), v)
#define set_optBlob(v, cb) config_save_item_b(static_cast<configItem>(cb), &v, sizeof v)
#define set_optStr_ifValid set_optStr
#define set_opt(t, v, cb) (config_save_item_##t(static_cast<configItem>(cb),v) && config_item_modified(static_cast<configItem>(cb)))
#define set_optN(t, v, cb) (config_save_item_n_##t(static_cast<configItem>(cb),v) && config_item_modified(static_cast<configItem>(cb)))

#define has_changed() SET_BIT(*changed_mask, so_key)

bool process_parmKvsConfig(so_msg_t so_key, const char *val, u32 *changed_mask);

int process_parmConfig(clpar p[], int len) {
  int arg_idx;
  int errors = 0;
  so_msg_t so_key = SO_NONE;
  u32 changed_mask = 0;
  bool flag_isValid = 0;
#ifdef USE_LAN
#define hasChanged_ethernet (changed_mask & (BIT(CB_LAN_PHY)|BIT(CB_LAN_PWR_GPIO)))
#else
#define hasChanged_ethernet false
#endif
#ifdef USE_MQTT
#define hasChanged_mqttClient (changed_mask & (BIT(CB_MQTT_ENABLE)|BIT(CB_MQTT_PASSWD)|\
    BIT(CB_MQTT_USER)|BIT(CB_MQTT_URL)|BIT(CB_MQTT_CLIENT_ID)))
#else
#define hasChanged_mqttClient false
#endif
#ifdef USE_HTTP
#define hasChanged_httpServer (changed_mask & (BIT(CB_HTTP_ENABLE)|BIT(CB_HTTP_PASSWD)|BIT(CB_HTTP_USER)))
#else
#define hasChanged_httpServer false
#endif
#define hasChanged_txtio (changed_mask & (BIT(CB_VERBOSE)))

  so_output_message(SO_CFG_begin, NULL);

  for (arg_idx = 1; arg_idx < len; ++arg_idx) {
    const char *key = p[arg_idx].key, *val = p[arg_idx].val;

    if (key == NULL || val == NULL) {  // don't allow any default values in config
      return cli_replyFailure();
#if ENABLE_RESTART
    } else if (strcmp(key, "restart") == 0) {
      extern void mcu_restart(void);
      mcu_restart();
#endif

    } else if (strcmp(key, "all") == 0) {
      if (*val == '?') {
        so_output_message(SO_CFG_all, "cj");
      }
    } else if (SO_NONE != (so_key = so_parse_config_key(key))) {
      if (0 == strcmp("?", val)) {
        so_output_message(so_key, NULL);
      } else if (process_parmKvsConfig(so_key, val, &changed_mask)) {

      } else
        switch (so_key) {

        case SO_CFG_RTC: {
          cli_replyResult(val ? rtc_set_by_string(val) : false);
        }
          break;

#ifdef USE_NETWORK
        case SO_CFG_NETWORK: {
          NODEFAULT();
          bool success = false;
          for (int i = 0; i < nwLEN; ++i) {
            if constexpr (!use_LAN)
              if (i == nwLan) {
                continue;
              }

            if constexpr (!use_WLAN)
              if (i == nwWlanSta || i == nwWlanAp) {
                continue;
              }
            if (strcmp(val, cfg_args_network[i]) == 0) {
              (void) (set_optN(i8, i, CB_NETWORK_CONNECTION));
              success = true;
              break;
            }
          }
          if (!success)
            cli_replyFailure();
        }
          break;
#endif

        case SO_CFG_TZ:
          if constexpr (use_POSIX_TIME) {
            if (set_optStr_ifValid(val, CB_TZ)) {
              rtc_setup();
            }

            if (!flag_isValid)
              cli_replyFailure();
          }
          break;

        case SO_CFG_STM32_BOOTGPIO_INV: {
          if (set_optN(i8, (*val == '1'), CB_STM32_INV_BOOTPIN)) {
          } //TODO
        }
          break;

        default:
          break;
        }

    } else if (strcmp(key, "set-pw") == 0) {
      if (set_optStr_ifValid(val, CB_CFG_PASSWD)) {
      }

      if (!flag_isValid)
        cli_replyFailure();
    } else {
      ++errors;
      cli_warning_optionUnknown(key);
    }
  }

  if constexpr (use_LAN)
    if (hasChanged_ethernet) {
      config_setup_ethernet();
    }

  if constexpr (use_MQTT)
    if (hasChanged_mqttClient) {
      config_setup_mqttAppClient();
    }

  if constexpr (use_HTTP)
    if (hasChanged_httpServer) {
      config_setup_httpServer();
    }

  if (hasChanged_txtio) {
    config_setup_txtio();
  }

  so_output_message(SO_CFG_end, NULL);
  cli_replyResult(errors == 0);
  return 0;
}
