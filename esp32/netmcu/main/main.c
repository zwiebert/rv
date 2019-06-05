#include "freertos/FreeRTOS.h"
#include "esp_wifi.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_event_loop.h"
#include "nvs_flash.h"
#include "driver/gpio.h"

#include "user_config.h"
#include "userio/http_server.h"
#include "config/config.h"
#include "main/rtc.h"
#include "watch_dog.h"


void uart_setup(void);
void ethernet_setup(void);
void setup_tcp_server(void);
void tcps_loop(void);
void setup_timer(void);
void setup_ntp(void);
void setup_storage(void);
void setup_mqtt(void);
void loop(void);
void wifistation_setup(void);

esp_err_t event_handler(void *ctx, system_event_t *event)
{
    return ESP_OK;
}

static int es_io_putc(char c) {
  putchar(c);
  return 1;
}

static int  es_io_getc(void) {
 return getchar();
}


void app_main(void)
{

	  esp_err_t err = nvs_flash_init();
	  if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
	    // NVS partition was truncated and needs to be erased
	    // Retry nvs_flash_init
	    ESP_ERROR_CHECK(nvs_flash_erase());
	    err = nvs_flash_init();
	  }
	  ESP_ERROR_CHECK( err );

	  io_putc_fun = es_io_putc;
	  io_getc_fun = es_io_getc;
	  io_printf_fun = ets_printf;

	  read_config(~0);
	  io_puts("\r\n\r\n");

    tcpip_adapter_init();

    ESP_ERROR_CHECK( esp_event_loop_init(event_handler, NULL) );

#ifdef USE_WLAN
    wifistation_setup();
#endif
#if 0
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK( esp_wifi_init(&cfg) );
    ESP_ERROR_CHECK( esp_wifi_set_storage(WIFI_STORAGE_RAM) );
    ESP_ERROR_CHECK( esp_wifi_set_mode(WIFI_MODE_STA) );
    wifi_config_t sta_config = {
        .sta = {
            .ssid = CONFIG_ESP_WIFI_SSID,
            .password = CONFIG_ESP_WIFI_PASSWORD,
            .bssid_set = false
        }
    };
    strcpy ((char*)sta_config.sta.ssid, C.wifi_SSID);
    strcpy ((char*)sta_config.sta.password, C.wifi_password);
    ESP_ERROR_CHECK( esp_wifi_set_config(WIFI_IF_STA, &sta_config) );
    ESP_ERROR_CHECK( esp_wifi_start() );
    ESP_ERROR_CHECK( esp_wifi_connect() );
#endif
#ifdef USE_LAN
    ethernet_setup();
#endif
#ifdef USE_SERIAL
    uart_setup();
#endif
#ifdef USE_NTP
  setup_ntp();
#endif
#ifdef USE_TCP
  setup_tcp_server();
#endif
#ifdef USE_MQTT
  setup_mqtt();
#endif
#ifdef USE_WDG
  watchDog_setup();
#endif
  rtc_setup();


  while (1) {
#ifdef USE_TCP
    tcps_loop();
#endif
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
