#include "main.h"
#include <cli/cli.h>
#include <config_kvs/config.h>

EventGroupHandle_t loop_event_group;
#define EVT_BITS  ((1 << lf_Len) - 1)

#ifndef USE_EG
volatile uint32_t loop_flags;
#endif
uint32_t loop_flags_periodic;

typedef void (*lfa_funT)(void);

static  lfa_funT lfa_table[lf_Len] = {
    ipnet_connected,
    ipnet_disconnected,
    lfa_createWifiAp,
    lfa_syncStm32Time,
    ping_loop, watchDog_loop,
    cli_loop,
    [] { mcu_delayedRestart(1500); },
#ifdef USE_LAN
    config_setup_ethernet,
#endif
#ifdef USE_MQTT
    config_setup_mqttAppClient,
#endif
#ifdef USE_HTTP
    config_setup_httpServer,
#endif
    config_setup_txtio,
};

#ifndef USE_EG
void loop_checkFlags() {
  u32 lf = loop_flags;
  for (int i = 0; lf; ++i, (lf >>= 1)) {
    if (!GET_BIT(lf, 0))
      continue;

    CLR_BIT(loop_flags, i);

    if (lfa_table[i])
      (lfa_table[i])();
  }
}
#else
void loop_eventBits_setup() {
  loop_event_group = xEventGroupCreate();
}
u32 loop_eventBits_wait() {
  EventBits_t bits = xEventGroupWaitBits(loop_event_group, EVT_BITS, pdTRUE, pdFALSE, portMAX_DELAY);
  return bits;
}

void loop_eventBits_check() {
  u32 lf = loop_eventBits_wait();
  for (int i = 0; lf; ++i, (lf >>= 1)) {
    if (!GET_BIT(lf, 0))
      continue;

    if (lfa_table[i])
      (lfa_table[i])();
  }
}
#endif

void tmr_checkNetwork_start() {
  const int interval = pdMS_TO_TICKS(1000 * CHECK_NETWORK_INTERVAL);
  TimerHandle_t tmr = xTimerCreate("CheckNetworkTimer", interval, pdFALSE, (void*) lf_createWifiAp, [](TimerHandle_t xTimer) {
    if (!ipnet_isConnected()) {
      lf_setBit(lf_createWifiAp);
    }
  });
  if (!tmr || xTimerStart(tmr, 10 ) != pdPASS) {
    printf("CheckNetworkTimer start error");
  }
}

void tmr_pingLoop_start() {
  const int interval = pdMS_TO_TICKS(1000 * PING_INTERVAL);
  TimerHandle_t tmr = xTimerCreate("PingLoopTimer", interval, pdTRUE, nullptr, [] (TimerHandle_t xTimer) {
    if (!wifi_ap_active) {
      lf_setBit(lf_pingLoop);
    }
  });
  if (!tmr || xTimerStart(tmr, 10 ) != pdPASS) {
    printf("PingLoopTimer start error");
  }
}


void tmr_loopPeriodic_start() {
  const int interval = pdMS_TO_TICKS(LOOP_PERIODIC_INTERVAL_MS);
  TimerHandle_t tmr = xTimerCreate("PingLoopTimer", interval, pdTRUE, nullptr, [](TimerHandle_t xTimer) {
    lf_setBits(loop_flags_periodic);
  });
  if (!tmr || xTimerStart(tmr, 10 ) != pdPASS) {
    printf("PingLoopTimer start error");
  }
}

void stm32loop() {
  if (stm32_mutexTakeTry()) {
    stm32_checkForInput();
    stm32_mutexGive();
  }
}

void loop(void) {
#ifdef USE_EG
  loop_eventBits_check();
#else
  loop_checkFlags();
#endif
}

