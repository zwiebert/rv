#include "main.h"

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include <lwip/apps/sntp.h>
#include <lwip/apps/sntp_opts.h>

SemaphoreHandle_t uart_mutex;
i32 boot_counter;
bool wifi_ap_active;

void lfa_createWifiAp() {
  esp_netif_init();
  wifiAp_setup("rv", "12345678");
  wifi_ap_active = true;
}

void lfa_syncStm32Time(void) {
  char buf[80];
  sprintf(buf, "config time=%ld;", time(0));
  dbg_vpf(ets_printf("to-strm32: <%s>", buf));
  stm32_write(buf, strlen(buf));
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

  for (int i = 1; (ptr = realloc(ptr, block_size * i)); ++i) {
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

void appEsp32_main(void) {

  mcu_init();

  tmr_loopPeriodic_start();
  while (1) {
    loop();
#ifndef USE_EG
    vTaskDelay(pdMS_TO_TICKS(LOOP_INTERVAL_MS));
#endif
  }
}

void mcu_restart(void) {
  printf("mcu_restart()\n");
  ets_delay_us(10000);
  esp_restart();
  for (;;) {
  };
}

