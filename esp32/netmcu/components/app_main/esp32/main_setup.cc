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

void ntpApp_setup(void) {
  sntp_set_time_sync_notification_cb([](struct timeval *tv) {
    ets_printf("ntp synced: %ld\n", time(0));
    mainLoop_callFun(lfa_syncStm32Time);
  });

  config_setup_ntpClient();
}

void main_setup_ip_dependent() {
  static int once;
  if (!once) {
    once = 1;

    if constexpr (use_NTP)
      ntpApp_setup();

    if constexpr (use_MQTT)
      config_setup_mqttAppClient();

    if constexpr (use_TCPS_TASK)
      config_ext_setup_cliTcpServer();

    if constexpr (use_HTTP)
      config_setup_httpServer();

  }
  tmr_pingLoop_start();
}


void lfPer100ms_mainFun() {
  cli_loop();
  watchDog_loop();
}


void mcu_init() {
  mainLoop_setup(32);

  kvs_setup();
  config_ext_setup_txtio();
  config_setup_global();

  if constexpr (use_SERIAL) {
    struct cfg_stm32 cfgStm32 = { .uart_tx_gpio = STM32_UART_TX_PIN, .uart_rx_gpio = STM32_UART_RX_PIN, .boot_gpio_is_inverse = STM32_BOOT_PIN_INV, .boot_gpio =
    STM32_BOOT_PIN, .reset_gpio = STM32_RESET_PIN, };
    stm32_setup(&cfgStm32);

    struct cfg_stm32com cfg_stm32com = { .enable = true };
    stm32com_setup_task(&cfg_stm32com);
  }

  io_puts("\r\n\r\n");

  ESP_ERROR_CHECK(esp_event_loop_create_default());

  if constexpr (use_NETWORK) {

    ipnet_CONNECTED_cb = main_setup_ip_dependent;

    if (use_AP_FALLBACK || C.network != nwNone)
      esp_netif_init();

#ifdef PING_NOT_BROKEN // pings will timeout with latest ESP-IDF (was working April 2020, broken April 2021 or earlier)
    //XXX reset tcp/ip adapter here instead of reboot?
    ping_restart_cb = [] {
      lf_setBit(lf_mcuRestart);
    };
#endif

    if constexpr (use_WLAN) {
      if (C.network == nwWlanSta)
        config_setup_wifiStation();
    }

    if constexpr (use_WLAN_AP) {
      if (C.network == nwWlanAp)
        lfa_createWifiAp(); // XXX: Create the fall-back AP. Should we have a regular configured AP also?
    }

    if constexpr (use_LAN) {
      if (C.network == nwLan)
        config_setup_ethernet();
    }

    if constexpr (use_AP_FALLBACK) {
      if (C.network != nwWlanAp)
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

  kvs_get_int32(KEY_BOOT_COUNTER, &boot_counter), kvs_store_int32(KEY_BOOT_COUNTER, ++boot_counter);
}
