#include "app_config/proj_app_cfg.h"
#include "freertos/FreeRTOS.h"

#include "esp_wifi.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_event.h"
#include "nvs_flash.h"
#include "driver/gpio.h"
#include "string.h"

#include "txtio/inout.h"
#include "net/wifistation.h"
#include "config/config.h"
#include "key_value_store/kvs_wrapper.h"
#include "config/config.h"
#include "storage/storage.h"
#include "net/tcp_server.h"
//#include "net/mqtt/mqtt.h"
#include "net/wifistation.h"
#include "net/ethernet.h"
#include "app/timer.h"
#include "app/common.h"
#include "misc/int_types.h"

#include "config/config.h"
#include "net/mqtt/app/mqtt.h"


#include "cli_app/cli_app.h"
//orig
#include "app_config/proj_app_cfg.h"
#include "config/config.h"
#include "app/rtc.h"
#include "watch_dog.h"
#include "stm32/stm32.h"
#include "kvstore.h"
#include "net/ntp.h"

#define KEY_BOOT_COUNTER "BOOT_CT"
i32 boot_counter;

#define BUF_SIZE 2048 // XXX
#define D(x) x

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

SemaphoreHandle_t uart_mutex;

#include <lwip/apps/sntp.h>
#include <lwip/apps/sntp_opts.h>

volatile bool ntp_sync_done;

void ntpApp_sync_time_cb(struct timeval *tv) {
  ets_printf("ntp synced: %ld\n", time(0));
  ntp_sync_done = true;
}

void ntpApp_setup(void) {
  sntp_set_time_sync_notification_cb(ntpApp_sync_time_cb);
  ntp_setup();
}

void ntpApp_loop() {
  if (ntp_sync_done) {
    char buf[80];
    sprintf(buf, "config time=%ld;", time(0));
    D(ets_printf("to-strm32: <%s>", buf));
    stm32_write(buf, strlen(buf));
    ntp_sync_done = false;
  }
}

bool has_networking;

void main_setup_ip_dependent() {
  static int once;
  if (!once) {
    once = 1;

#ifdef USE_NTP
  ntpApp_setup();
#endif
#ifdef USE_MQTT
  io_mqttApp_setup();
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
  has_networking = true;
}


void
mcu_init() {
 uart_mutex = xSemaphoreCreateRecursiveMutex();

#ifdef USE_SERIAL
  void stm32_setup(void);
  stm32_setup();
#endif

  kvs_setup();
  txtio_setup();
  config_setup();

  io_puts("\r\n\r\n");

  ESP_ERROR_CHECK(esp_event_loop_create_default());

#ifdef USE_NETWORK
  void wifiAp_setup(void);
  switch (C.network) {
#ifdef USE_WLAN
  case nwWlanSta:
    esp_netif_init();
    wifistation_setup();
    break;
#endif
#ifdef USE_WLAN_AP
  case nwWlanAp:
    esp_netif_init();
    wifiAp_setup();
    break;
#endif
#ifdef USE_LAN
  case nwLan:
    esp_netif_init();
    ethernet_setup(C.lan_phy, C.lan_pwr_gpio);
#endif
    break;
  default:
    break;
  }
#else
  wifistation_setup();
#endif

#ifdef USE_MUTEX
  void mutex_setup(void);
  mutex_setup();
#endif

  ////Orig

  read_config(~0);
  io_puts("\r\n\r\n");

#ifdef USE_WDG
  watchDog_setup();
#endif
#ifdef USE_FS
  stor_setup();
#endif

  rtc_setup();

  kvs_get_int32(KEY_BOOT_COUNTER, &boot_counter), kvs_store_int32(KEY_BOOT_COUNTER, ++boot_counter);
}


void stm32loop() {
  if (xSemaphoreTakeRecursive(uart_mutex, 0)) {
    char c;
    if (stm32_read(&c, 1) == 1) {

      char *buf = malloc(BUF_SIZE);
      if (buf) {

        *buf = c;

        int n = stm32_read(buf + 1, BUF_SIZE - 2) + 1;
        if (n > 0) {
          buf[n] = '\0';

          if (watchDog_checkCommandLine(buf)) {

          } else if (cli_checkStm32CommandLine(buf)) {

          }
        }

        D(db_printf("from-stm32: <%s>\n", buf));
        free(buf);
      }
    }
    xSemaphoreGiveRecursive(uart_mutex);
  }
}

void loop(void) {
  watchDog_loop();
  cli_loop();
#ifdef USE_WLAN
  extern void wifistation_loop(void);
  wifistation_loop();
#endif
#ifdef USE_LAN
  extern void ethernet_loop(void);
  ethernet_loop();
#endif
#ifdef USE_NTP
  ntpApp_loop();
#endif
#ifdef USE_TCPS
    tcps_loop();
#endif
  stm32loop();
}

void appEsp32_main(void) {

  mcu_init();

  while (1) {
    loop();
    vTaskDelay(25 / portTICK_PERIOD_MS);
  }
}

void  mcu_restart(void) {
  printf("mcu_restart()\n");
  ets_delay_us(10000);
  esp_restart();
  for (;;) {
  };
}

