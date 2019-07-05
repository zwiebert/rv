/*
 * ping.c
 *
 *  Created on: 21.06.2019
 *      Author: bertw
 */

#include <gw_ping.h>
#include <gw_ping.h>
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include "freertos/event_groups.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "esp_event_loop.h"
#include "nvs_flash.h"
#include "lwip/inet.h"
#include "lwip/ip4_addr.h"
#include "lwip/dns.h"
#include "esp_ping.h"

#include "userio/ipnet.h"
#include "debug/debug.h"

#define D(x)

int ping_init(void);

uint32_t ping_count = 4;  //how many pings per report
uint32_t ping_timeout = 1000; //mS till we consider it timed out
uint32_t ping_delay = 500; //mS between pings
uint32_t waiting_results = 0;


static int recv_count;
static int ping_error_count;
#define PING_ERROR_LIMIT 10

esp_err_t pingResults(ping_target_id_t msgType, esp_ping_found * pf){

  recv_count = pf->recv_count;

  D(printf("AvgTime:%.1fmS Sent:%d Rec:%d Err:%d min(mS):%d max(mS):%d ", (float)pf->total_time/pf->recv_count, pf->send_count, pf->recv_count, pf->err_count, pf->min_time, pf->max_time ));
  D(printf("Resp(mS):%d Timeouts:%d Total Time:%d\n",pf->resp_time, pf->timeout_count, pf->total_time));
  waiting_results = 0;
  return ESP_OK;
}

static void ping_send() {
  char buf[16];
  ip4addr_ntoa_r(&ip4_gateway_address, buf, sizeof buf);
  D(printf("ping to address: %s\n", buf));

  esp_ping_set_target(PING_TARGET_IP_ADDRESS_COUNT, &ping_count, sizeof(uint32_t));
  esp_ping_set_target(PING_TARGET_RCV_TIMEO, &ping_timeout, sizeof(uint32_t));
  esp_ping_set_target(PING_TARGET_DELAY_TIME, &ping_delay, sizeof(uint32_t));
  esp_ping_set_target(PING_TARGET_IP_ADDRESS, &ip4_gateway_address.addr, sizeof(uint32_t));
  esp_ping_set_target(PING_TARGET_RES_FN, &pingResults, sizeof(pingResults));
  waiting_results = 1;
  ping_init();
}

void ping_loop() {
  if (recv_count) {
    recv_count = 0;
    ping_error_count = 0;
    D(db_printf("gw_ping: ping echo received\n"));
  } else if (++ping_error_count > PING_ERROR_LIMIT) {
    ping_error_count = 0;
   //XXX reset tcp/ip adapter here instead of reboot
    extern void  mcu_restart(void);
    mcu_restart();
  }
  ping_send();
}

void ping_setup() {

}
