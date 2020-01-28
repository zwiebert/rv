#include "freertos/FreeRTOS.h"
#include "esp_wifi.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_event.h"
#include "nvs_flash.h"
#include "driver/gpio.h"

#include "user_config.h"
#include "userio/http_server.h"
#include "config/config.h"
#include "main/rtc.h"
#include "watch_dog.h"
#include "stm32/stm32.h"
#include "kvstore.h"

#define KEY_BOOT_COUNTER "BOOT_CT"
i32 boot_counter;


void loop(void);

esp_err_t event_handler(void *ctx, system_event_t *event) {
  return ESP_OK;
}

static int es_io_putc(char c) {
  putchar(c);
  return 1;
}

static int es_io_getc(void) {
  return getchar();
}

void main_setup_ip_dependent() {
  static int once;
  if (!once) {
    once = 1;

#ifdef USE_NTP
  void setup_ntp(void);
  setup_ntp();
#endif
#ifdef USE_MQTT
  void setup_mqtt(void);
  setup_mqtt();
#endif
#ifdef  USE_HTTP_GET
  void httpGet_setup(void);
  httpGet_setup();
#endif
#ifdef  USE_HTTP_CLIENT
  void httpClient_setup(void);
  httpClient_setup();
#endif
  }

}

void app_main(void) {
#ifdef USE_SERIAL
  void stm32_setup(void);
  stm32_setup();
#endif
  esp_err_t err = nvs_flash_init();
  if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
    // NVS partition was truncated and needs to be erased
    // Retry nvs_flash_init
    ESP_ERROR_CHECK(nvs_flash_erase());
    err = nvs_flash_init();
  }
  ESP_ERROR_CHECK(err);

  io_putc_fun = es_io_putc;
  io_getc_fun = es_io_getc;
  io_printf_fun = ets_printf;

  read_config(~0);
  io_puts("\r\n\r\n");

  esp_netif_init();
  // Create default event loop that running in background
  ESP_ERROR_CHECK(esp_event_loop_create_default());

#ifdef USE_WLAN
  void wifistation_setup(void);
  wifistation_setup();
#endif
#ifdef USE_LAN
    void ethernet_setup(void);
    ethernet_setup();
#endif
    main_setup_ip_dependent();
#ifdef USE_WDG
  watchDog_setup();
#endif
#ifdef USE_FS
  void fs_setup(void);
  fs_setup();
#endif

  rtc_setup();

  kvs_get_int32(KEY_BOOT_COUNTER, &boot_counter),  kvs_store_int32(KEY_BOOT_COUNTER, ++boot_counter);

  while (1) {
    loop();
    vTaskDelay(25 / portTICK_PERIOD_MS);
  }

}

void mcu_restart(void) {
  printf("mcu_restart()\n");
  ets_delay_us(10000);
  esp_restart();
  for (;;) {
  };
}
