#include "main.h"
#include "config/config.h"
#include "stm32/stm32.h"

void ntpApp_sync_time_cb(struct timeval *tv) {
  ets_printf("ntp synced: %ld\n", time(0));
  lf_setBit(lf_syncStm32Time);
}

void ntpApp_setup(void) {
  sntp_set_time_sync_notification_cb(ntpApp_sync_time_cb);
  ntp_setup(cfg_getNtpClient());
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
    io_mqttApp_setup(cfg_getMqttClient());
#endif
#ifdef  USE_HTTP_GET
  void httpGet_setup(void);
  httpGet_setup();
#endif
#ifdef  USE_HTTP_CLIENT
    void httpClient_setup(void);
    httpClient_setup();
#endif
#ifdef USE_TCPS
  tcps_startServer();
#endif
  }
  tmr_pingLoop_start();
}

void mcu_init() {
  uart_mutex = xSemaphoreCreateRecursiveMutex();
#ifdef USE_EG
  loop_eventBits_setup();
#endif

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
#endif

  kvs_setup();
  txtio_setup(cfg_getTxtio());
  config_setup();

  io_puts("\r\n\r\n");

  ESP_ERROR_CHECK(esp_event_loop_create_default());

  lfPer_setBit(lf_loopWatchDog);
  lfPer_setBit(lf_loopCli);
  lfPer_setBit(lf_loopStm32);
#ifdef USE_TCPS
  lfPer_setBit(lf_loopTcpServer);
#endif

  ipnet_cbRegister_gotIpAddr(lfa_gotIpAddr_cb);
  ipnet_cbRegister_lostIpAddr(lfa_lostIpAddr_cb);
#ifdef USE_NETWORK
  switch (C.network) {
#ifdef USE_WLAN
  case nwWlanSta:
    esp_netif_init();
    wifistation_setup(cfg_getWlan());
    break;
#endif
#ifdef USE_WLAN_AP
  case nwWlanAp:
    esp_netif_init();
    wifiAp_setup("rv", "12345678");
    wifi_ap_active = true;
    break;
#endif
#ifdef USE_LAN
  case nwLan:
    esp_netif_init();
    ethernet_setup(cfg_getLan());
#endif
    break;
  default:
    break;
  }
#else
  wifistation_setup();
#endif
#ifdef USE_AP_FALLBACK
  tmr_checkNetwork_start();
#endif
#ifdef USE_CLI_MUTEX
  void mutex_setup(void);
  mutex_setup();
#endif

  ////Orig

  read_config(~0);
  io_puts("\r\n\r\n");

#ifdef USE_FS
  stor_setup();
#endif

  rtc_setup();

  kvs_get_int32(KEY_BOOT_COUNTER, &boot_counter), kvs_store_int32(KEY_BOOT_COUNTER, ++boot_counter);
}
