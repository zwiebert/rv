#include "main.h"
#include "config/config.h"
#include "stm32/stm32.h"
#include "stm32_com/com_task.h"

void loop_setBit_mcuRestart() {
  lf_setBit(lf_mcuRestart);
}

void ntpApp_sync_time_cb(struct timeval *tv) {
  ets_printf("ntp synced: %ld\n", time(0));
  lf_setBit(lf_syncStm32Time);
}

void ntpApp_setup(void) {
  sntp_set_time_sync_notification_cb(ntpApp_sync_time_cb);
  config_setup_ntpClient();
}

void lfa_gotIpAddr_cb() {
  lf_setBit(lf_gotIpAddr);
}
void lfa_lostIpAddr_cb() {
  lf_setBit(lf_lostIpAddr);
}


void main_setup_ip_dependent() {
  static int once;
  if (!once) {
    once = 1;

#ifdef USE_NTP
    ntpApp_setup();
#endif
#ifdef USE_MQTT
    config_setup_mqttAppClient();
#endif
#ifdef  USE_HTTP_GET
  void httpGet_setup(void);
  httpGet_setup();
#endif
#ifdef  USE_HTTP_CLIENT
    void httpClient_setup(void);
    httpClient_setup();
#endif
#if defined USE_TCPS || defined USE_TCPS_TASK
  config_setup_cliTcpServer();
#endif
#ifdef USE_HTTP
  config_setup_httpServer();
#endif
  }
  tmr_pingLoop_start();
}

void mcu_init() {
  uart_mutex = xSemaphoreCreateRecursiveMutex();
#ifdef USE_EG
  loop_eventBits_setup();
#endif

  kvs_setup();
  config_setup_txtio();
  config_setup_global();

#ifdef USE_SERIAL
  struct cfg_stm32 *cfg_stm32 = calloc(1, sizeof (struct cfg_stm32));
  assert(cfg_stm32);
  *cfg_stm32 = (struct cfg_stm32) {
      .uart_tx_gpio = STM32_UART_TX_PIN,
      .uart_rx_gpio = STM32_UART_RX_PIN,
      .boot_gpio_is_inverse = STM32_BOOT_PIN_INV,
      .boot_gpio = STM32_BOOT_PIN,
      .reset_gpio = STM32_RESET_PIN,
  };
  stm32_setup(cfg_stm32);

#ifdef USE_STM32COM
  struct cfg_stm32com cfg_stm32com = { .enable = true };
  stm32com_setup_task(&cfg_stm32com);
#endif
#endif

  io_puts("\r\n\r\n");

  ESP_ERROR_CHECK(esp_event_loop_create_default());

  lfPer_setBit(lf_loopWatchDog);
  lfPer_setBit(lf_loopCli);
#ifndef USE_STM32COM
  lfPer_setBit(lf_loopStm32);
#endif
#ifdef USE_TCPS
  lfPer_setBit(lf_loopTcpServer);
#endif

  ipnet_cbRegister_gotIpAddr(lfa_gotIpAddr_cb);
  ipnet_cbRegister_lostIpAddr(lfa_lostIpAddr_cb);

void loop_setBit_mcuRestart() {
  lf_setBit(lf_mcuRestart);
}
#ifdef USE_NETWORK
#ifdef USE_AP_FALLBACK
  esp_netif_init();
#else
  if (C.network != nwNone)
    esp_netif_init();
#endif

  ipnet_cbRegister_gotIpAddr(lfa_gotIpAddr_cb);
  ipnet_cbRegister_lostIpAddr(lfa_lostIpAddr_cb);
  switch (C.network) {
#ifdef USE_WLAN
  case nwWlanSta:
    config_setup_wifiStation();
    break;
#endif
#ifdef USE_WLAN_AP
  case nwWlanAp:
    lfa_createWifiAp(); // XXX: Create the fall-back AP. Should we have a regular configured AP also?
    break;
#endif
#ifdef USE_LAN
  case nwLan:
    config_setup_ethernet();
#endif
    break;
  default:
    break;
  }
#endif

#ifdef USE_CLI_MUTEX
  void mutex_setup(void);
  mutex_setup();
#endif

#ifdef USE_AP_FALLBACK
  if (C.network != nwWlanAp)
    tmr_checkNetwork_start();
#endif
  ////Orig

  io_puts("\r\n\r\n");

#ifdef USE_FS
  stor_setup();
#endif

  rtc_setup();

  kvs_get_int32(KEY_BOOT_COUNTER, &boot_counter), kvs_store_int32(KEY_BOOT_COUNTER, ++boot_counter);
}
