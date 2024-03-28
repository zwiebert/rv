#include "main.h"
#include <cli/cli.h>
#include "app_settings/config.h"
#include "stm32/stm32.h"
#include "stm32_com/com_task.h"
#include "app_cli/cli_app.h"
#include "cli/mutex.h"
#include "net/http_client.h"
#include "app_settings/config.h"
#include "app_http_server/setup.h"
#include "../app_private.h"
#include "main_loop/main_queue.hh"
#include <esp_netif.h>
#include <esp_log.h>

#define logtag "main"

void ntpApp_setup(void) {
  sntp_set_time_sync_notification_cb([](struct timeval *tv) {
    ESP_LOGI(logtag, "ntp synced: %lld", (long long int)time(0));

    static bool once;
    for (int i=0; !once && i < SNTP_MAX_SERVERS; ++i, once = true) {
      if (auto name = esp_sntp_getservername(i); name) {
        ESP_LOGE(logtag, "ntp server #%d is <%s>", i, name);
      }
    }
    mainLoop_callFun(lfa_syncStm32Time);
  });

  config_setup_ntpClient();
}

void main_setup_ip_dependent() {
  static int once;
  if (!once) {
    once = 1;

#ifdef CONFIG_APP_USE_NTP
    ntpApp_setup();
#endif
#ifdef CONFIG_APP_USE_MQTT
    config_setup_mqttAppClient();
#endif
#ifdef CONFIG_APP_USE_TCPS_TASK
    config_ext_setup_cliTcpServer();
#endif
#ifdef CONFIG_APP_USE_HTTP
  config_setup_httpServer();
#endif

  }
#ifdef CONFIG_APP_PING_THE_GATEWAY
  tmr_pingLoop_start();
#endif
}


void lfPer100ms_mainFun() {
#ifndef CONFIG_APP_USE_CLI_TASK
  cli_loop();
#endif
  watchDog_loop();
}


void mcu_init() {
  mainLoop_setup(32);

  kvs_setup();
  config_ext_setup_txtio();

#ifdef CONFIG_STM32_USE_COMPONENT
  config_setup_stm32();

  struct cfg_stm32com cfg_stm32com = { .enable = true };
  stm32com_setup_task(&cfg_stm32com);
#endif

  io_puts("\r\n\r\n");

  ESP_ERROR_CHECK(esp_event_loop_create_default());

  if constexpr (use_NETWORK) {

    ipnet_CONNECTED_cb = main_setup_ip_dependent;
    enum nwConnection network = config_read_network_connection();

    if (use_AP_FALLBACK || network != nwNone)
      esp_netif_init();

#ifdef PING_NOT_BROKEN // pings will timeout with latest ESP-IDF (was working April 2020, broken April 2021 or earlier)
    //XXX reset tcp/ip adapter here instead of reboot?
    ping_restart_cb = [] {
      lf_setBit(lf_mcuRestart);
    };
#endif

    if constexpr (use_WLAN) {
      if (network == nwWlanSta)
        config_setup_wifiStation();
    }

    if constexpr (use_WLAN_AP) {
      if (network == nwWlanAp)
        lfa_createWifiAp(); // XXX: Create the fall-back AP. Should we have a regular configured AP also?
    }

    if constexpr (use_LAN) {
      if (network == nwLan)
        config_setup_ethernet();
    }

    if constexpr (use_AP_FALLBACK) {
      if (network != nwWlanAp)
        tmr_checkNetwork_start();
    }

    if constexpr (use_HTTP) {
      hts_setup_content();
    }

  }
  ////Orig

  io_puts("\r\n\r\n");

  if constexpr (use_FS)
    stor_setup();

  rtc_setup();
  cliApp_setup();
#ifdef CONFIG_APP_USE_CLI_TASK
  cli_setup_task(true);
#else
#error "currently unsupported" // FIXME: support CLI without its own task, or remove that APP_USE_CLI_TASK kconfig option
#endif

  kvs_get_int32(KEY_BOOT_COUNTER, &boot_counter), kvs_store_int32(KEY_BOOT_COUNTER, ++boot_counter);

}
