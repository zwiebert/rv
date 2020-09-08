/*
 * ping.c
 *
 *  Created on: 21.06.2019
 *      Author: bertw
 */

#include "app/net/gw_ping.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "freertos/event_groups.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "esp_event.h"
#include "lwip/inet.h"
#include "lwip/ip4_addr.h"
#include "lwip/dns.h"
#include "esp_ping.h"
#include "esp_netif_ip_addr.h"

#include "net/ipnet.h"
#include "debug/dbg.h"

#include "misc/int_types.h"

#define D(x)

void (*ping_restart_cb)();

extern "C" int ping_init(void);

u32 ping_count = 4;  //how many pings per report
u32 ping_timeout = 1000; //mS till we consider it timed out
u32 ping_delay = 500; //mS between pings
u32 waiting_results = 0;


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

extern esp_ip4_addr_t ip4_address, ip4_gateway_address, ip4_netmask;

static void ping_send() {
  char buf[16];

  ip4addr_ntoa_r(reinterpret_cast<const ip4_addr_t*>(&ip4_gateway_address), buf, sizeof buf);
  D(printf("ping to address: %s\n", buf));

  esp_ping_set_target(PING_TARGET_IP_ADDRESS_COUNT, &ping_count, sizeof(u32));
  esp_ping_set_target(PING_TARGET_RCV_TIMEO, &ping_timeout, sizeof(u32));
  esp_ping_set_target(PING_TARGET_DELAY_TIME, &ping_delay, sizeof(u32));
  esp_ping_set_target(PING_TARGET_IP_ADDRESS, &ip4_gateway_address.addr, sizeof(u32));
  esp_ping_set_target(PING_TARGET_RES_FN, reinterpret_cast<void *>(&pingResults), sizeof(&pingResults));
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

    if (ping_restart_cb)
      ping_restart_cb();
  }
  ping_send();
}

