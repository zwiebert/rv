#include "main.h"

volatile uint32_t loop_flags;
uint32_t loop_flags_periodic;

typedef void (*lfa_funT)(void);

static const lfa_funT lfa_table[lf_Len] = {
#if defined USE_AP_FALLBACK || defined USE_WLAN_AP
    lfa_createWifiAp,
#endif
    lfa_syncStm32Time, wifistation_loop,
#ifdef USE_LAN
    ethernet_loop,
#endif
#ifdef USE_WDG
    ping_loop, watchDog_loop,
#endif
#ifdef USE_TCPS
    tcps_loop,
#endif
    stm32loop, cli_loop, };

void loop_checkFlags() {
  u32 lf = loop_flags;
  for (int i = 0; lf; ++i, (lf >>= 1)) {
    if (!GET_BIT(lf, 0))
      continue;

    CLR_BIT(loop_flags, i);

    if (lfa_table[i])
      (lfa_table[i])();

    switch (i) {
    case lf_createWifiAp:
      break;
    default:
      break;
    }
  }
}

static void tmr_checkNetwork_cb(TimerHandle_t xTimer) {
  if (!wifi_ap_active && !ipnet_isConnected()) {
    SET_BIT(loop_flags, lf_createWifiAp);
  }
}

void tmr_checkNetwork_start() {
  TimerHandle_t tmr;
  int interval = pdMS_TO_TICKS(1000 * CHECK_NETWORK_INTERVAL);
  tmr = xTimerCreate("CheckNetworkTimer", interval, pdFALSE, (void*) lf_createWifiAp, tmr_checkNetwork_cb);
  if (xTimerStart(tmr, 10 ) != pdPASS) {
    printf("CheckNetworkTimer start error");
  }
}

static void tmr_pingLoop_cb(TimerHandle_t xTimer) {
  if (!wifi_ap_active) {
    SET_BIT(loop_flags, lf_pingLoop);
  }
}

void tmr_pingLoop_start() {
  TimerHandle_t tmr;
  int interval = pdMS_TO_TICKS(1000 * PING_INTERVAL);
  tmr = xTimerCreate("PingLoopTimer", interval, pdTRUE, (void*) lf_pingLoop, tmr_pingLoop_cb);
  if (xTimerStart(tmr, 10 ) != pdPASS) {
    printf("PingLoopTimer start error");
  }
}

static void tmr_loopPeriodic_cb(TimerHandle_t xTimer) {
  loop_flags |= loop_flags_periodic;
}

void tmr_loopPeriodic_start() {
  TimerHandle_t tmr;
  int interval = pdMS_TO_TICKS(LOOP_PERIODIC_INTERVAL_MS);
  tmr = xTimerCreate("PingLoopTimer", interval, pdTRUE, (void*) 0, tmr_loopPeriodic_cb);
  if (xTimerStart(tmr, 10 ) != pdPASS) {
    printf("PingLoopTimer start error");
  }
}

void stm32loop() {
  if (xSemaphoreTakeRecursive(uart_mutex, 0)) {
    stm32_checkForInput();
    xSemaphoreGiveRecursive(uart_mutex);
  }
}

void loop(void) {
  loop_checkFlags();
}

