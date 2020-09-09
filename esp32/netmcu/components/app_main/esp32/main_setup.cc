#include "main.h"
#include "config/config.h"
#include "stm32/stm32.h"
#include "stm32_com/com_task.h"
#include "cli_app/cli_app.h"
#include "cli/mutex.h"
#include "net/http_client.h"
#include "config/config.h"
#include "net/http/server/content/setup.h"

void mcu_restart() {
  lf_setBit(lf_mcuRestart);
}

void ntpApp_setup(void) {
  sntp_set_time_sync_notification_cb([](struct timeval *tv) {
    ets_printf("ntp synced: %ld\n", time(0));
    lf_setBit(lf_syncStm32Time);
  });

  config_setup_ntpClient();
}

extern "C" void main_setup_ip_dependent() { //XXX called from library
  static int once;
  if (!once) {
    once = 1;

    if constexpr (use_NTP)
      ntpApp_setup();

    if constexpr (use_MQTT)
      config_setup_mqttAppClient();

    if constexpr (use_HTTP_CLIENT)
      httpClient_setup();

    if constexpr (use_TCPS || use_TCPS_TASK)
      config_setup_cliTcpServer();

    if constexpr (use_HTTP)
      config_setup_httpServer();

  }
  tmr_pingLoop_start();
}

void mcu_init() {

  if constexpr (use_EG)
    loop_eventBits_setup();

  kvs_setup();
  config_setup_txtio();
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

  lfPer_setBit(lf_loopWatchDog);
  lfPer_setBit(lf_loopCli);
  if constexpr (use_TCPS)
    lfPer_setBit(lf_loopTcpServer);

  if constexpr (use_NETWORK) {
    if (use_AP_FALLBACK || C.network != nwNone)
      esp_netif_init();

    //XXX reset tcp/ip adapter here instead of reboot?
    ping_restart_cb = [] {
      lf_setBit(lf_mcuRestart);
    };
    ipnet_gotIpAddr_cb = [] {
      lf_setBit(lf_gotIpAddr);
    };
    ipnet_lostIpAddr_cb = [] {
      lf_setBit(lf_lostIpAddr);
    };

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
