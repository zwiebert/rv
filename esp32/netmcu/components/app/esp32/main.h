/*
 * main.h
 *
 *  Created on: 24.03.2020
 *      Author: bertw
 */

#ifndef APP_ESP32_MAIN_H_
#define APP_ESP32_MAIN_H_

#include "app_config/proj_app_cfg.h"
#include "app/common.h"
#include "app/rtc.h"
#include "app/timer.h"
#include "app_config/proj_app_cfg.h"
#include "app/net/gw_ping.h"
#include "cli_app/cli_app.h"
#include "config/config.h"
#include "driver/gpio.h"
#include "key_value_store/kvs_wrapper.h"
#include "kvstore.h"
#include "misc/int_macros.h"
#include "misc/int_types.h"
#include "net/ethernet.h"
#include "net/ipnet.h"
#include "net/mqtt/app/mqtt.h"
#include "net/ntp.h"
#include "net/tcp_cli_server.h"
#include "net/wifi_ap.h"
#include "net/wifistation.h"
#include "nvs_flash.h"
#include "stm32/stm32.h"
#include "storage/storage.h"
#include "txtio/inout.h"
#include "watch_dog.h"

#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"

#include <esp_event.h>
#include <esp_system.h>
#include <esp_wifi.h>
#include <esp_sntp.h>
#include <string.h>



#define KEY_BOOT_COUNTER "BOOT_CT"
#ifdef USE_EG
extern EventGroupHandle_t loop_event_group;
#define lf_setBits(v) xEventGroupSetBits(loop_event_group, (v))
#else
#define lf_setBits(v) (loop_flags |= (v))
#endif

#define lf_setBit(v)  lf_setBits(1<<(v))
#define lfPer_setBits(v) (loop_flags_periodic |= (v))
#define lfPer_setBit(v)  lfPer_setBits(1<<(v))

enum loop_flagbits {
  lf_gotIpAddr, lf_lostIpAddr,
#if defined USE_AP_FALLBACK || defined USE_WLAN_AP
  lf_createWifiAp,
#endif
  lf_syncStm32Time,
#ifdef USE_WDG
  lf_pingLoop, lf_loopWatchDog,
#endif
#ifdef USE_TCPS
  lf_loopTcpServer,
#endif
  lf_loopCli,
  lf_mcuRestart,
  //-------------
  lf_Len
};

extern volatile uint32_t loop_flags;
extern uint32_t loop_flags_periodic;
extern i32 boot_counter;
extern bool wifi_ap_active;
extern SemaphoreHandle_t uart_mutex;

void lfa_createWifiAp(void);
void lfa_syncStm32Time(void);
void lfa_gotIpAddr(void);
void lfa_lostIpAddr(void);
void lfa_mcuRestart(void);
void stm32loop(void);
void loop(void);
void tmr_checkNetwork_start();
void tmr_loopPeriodic_start();
void tmr_pingLoop_start();
void loop_eventBits_setup();
u32 loop_eventBits_wait();

void stm32_checkForInput();
void main_setup_ip_dependent();
void mcu_init();


#define D(x) x
#endif /* APP_ESP32_MAIN_H_ */
