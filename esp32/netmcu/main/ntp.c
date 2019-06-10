/*
 * not_implemented.c
 *
 *  Created on: 12.09.2017
 *      Author: bertw
 */

#include <ctype.h>
//#include "lwip/apps/sntp/sntp.h"
#include <lwip/apps/sntp.h>
#include <lwip/apps/sntp_opts.h>
#include <lwip/ip_addr.h>
#include <esp32/rom/ets_sys.h>
#include <string.h>
#include <uart.h>

#include "user_config.h"
#include "config/config.h"
#include "esp_event.h"
#include "esp_event_loop.h"
#include "esp_system.h"
#include "esp_wifi.h"

#define printf ets_printf
#define D(x) x

static void set_server_by_config() {
  int server_number = 0;
  ip_addr_t addr[SNTP_MAX_SERVERS];

#if SNTP_MAX_SERVERS > 1
  static char servers[sizeof C.ntp_server];

  memcpy(servers,C.ntp_server, sizeof servers);
  const char *server = servers;
  char *sep[SNTP_MAX_SERVERS-1];

  int i;

  for (i=0; i < SNTP_MAX_SERVERS-1; ++i) {
    if ((sep[i] = strchr((i == 0 ? server : sep[i-1]+1), ','))) {
      *sep[i] = '\0';
    } else {
      break;
    }
  }

  for (server_number = 0; server_number < SNTP_MAX_SERVERS; ++server_number) {
    if (server_number != 0) {
      if (sep[server_number - 1])
      server = sep[server_number - 1] + 1;
      else
      break;
    }
#else
  {
    const char *server = C.ntp_server;
#endif
    bool use_dhcp = strcmp(server, "dhcp") == 0;
    bool use_ipaddr = isdigit((int )server[0]);

    sntp_servermode_dhcp(use_dhcp);

    if (use_ipaddr) {
      ipaddr_aton(server, &addr[server_number]);
      sntp_setserver(server_number, &addr[server_number]);
    } else if (use_dhcp) {
#if SNTP_MAX_SERVERS > 1
      break;
#endif
    } else {
      sntp_setservername(server_number, server);
    }
  }
  sntp_init();
}

  volatile bool ntp_sync_done;

  void sntp_sync_time_cb (struct timeval *tv) {
      ets_printf("ntp synced: %ld\n", time(0));
      ntp_sync_done = true;
  }

void setup_ntp(void) {
  static int once;
  if (once == 0) {
    once = 1;
    sntp_setoperatingmode(SNTP_OPMODE_POLL);
    sntp_set_time_sync_notification_cb(sntp_sync_time_cb);
    set_server_by_config();

    D(ets_printf("server:<%s> <%s> <%s>\n",sntp_getservername(0), sntp_getservername(1), sntp_getservername(2)));
  }
}

void ntp_loop() {
    if (ntp_sync_done) {
        char buf[80];
        sprintf(buf, "config time=%ld;", time(0));
        D(ets_printf("to-strm32: <%s>", buf));
        stm32_write(buf, strlen(buf));
        ntp_sync_done = false;
    }
}
