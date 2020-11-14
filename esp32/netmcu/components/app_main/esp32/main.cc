#include "main.h"

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include <lwip/apps/sntp.h>
#include <lwip/apps/sntp_opts.h>
#include <net/tcp_cli_server_setup.hh>

i32 boot_counter;
#define WIFI_AP_SSID "rv"
#define WIFI_AP_PASSWD "12345678"

bool wifi_ap_active;
void lfa_createWifiAp() {
  if (!wifi_ap_active) {
    wifi_ap_active = true;
    wifiAp_setup(WIFI_AP_SSID, WIFI_AP_PASSWD);

#ifdef USE_TCPS_TASK
    struct cfg_tcps cfg_tcps = { .enable = true }; // XXX: user-flags not set
    tcpCli_setup_task(&cfg_tcps);
#endif

    struct cfg_http cfg_http = { .enable = true };
    hts_setup(&cfg_http);
  }
}

void lfa_syncStm32Time(void) {
  char buf[80];
  sprintf(buf, "{\"config\":{\"time\":%ld}};", time(0));
  dbg_vpf(ets_printf("to-strm32: <%s>", buf));
  if (stm32_mutexTake()) {
    stm32_write(buf, strlen(buf));
    stm32_mutexGive();
  }
}

void  mcu_delayedRestart(unsigned delay_ms) {
  printf("mcu_restart()\n");
  vTaskDelay(pdMS_TO_TICKS(delay_ms));
  esp_restart();
  for (;;) {
  }
}

static void stm32_processInputLine(char *line) {
  if (watchDog_checkCommandLine(line)) {
    return; // command-line consumed by watch-dog
  }

  if (cli_checkStm32CommandLine(line)) {
    return; // command-line consumed by cli
  }
}

void stm32_checkForInput() {
  char c;
  char *ptr = 0, *buf = 0;
  const int block_size = 32;
  int length, n;

  if ((length = stm32_read(&c, 1)) <= 0)
    return;

  for (int i = 1; (ptr = static_cast<char*>(realloc(ptr, block_size * i))); ++i) {
    buf = ptr;

    if (i == 1)
      *ptr = c;

    int req_size = (i == 1) ? block_size - 2 : block_size;

    length += (n = stm32_read(ptr + length, req_size));
    if (n >= req_size)
      continue; // more to read
    if (n < 0)
      break; // error

    ptr[length] = '\0';
    stm32_processInputLine(ptr);
    dbg_vpf(db_printf("from-stm32: <%s>\n", buf));
    break; // done
  }

  free(buf);
}

extern "C" void app_main(void) {

  mcu_init();

  tmr_loopPeriodic_start();
  while (1) {
    loop();
if constexpr (!use_EG)
    vTaskDelay(pdMS_TO_TICKS(LOOP_INTERVAL_MS));
  }
}



