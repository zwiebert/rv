#include "user_config.h"
#include "cli/cli.h"
#include "debug/debug.h"
#include "uart.h"
#include "userio/http_server.h"
#include "watch_dog.h"
#include <time.h>
#include <stdio.h>

#define BUF_SIZE 256
char buf[BUF_SIZE];

//#include "freertos/task.h"
#define D(x) x


void loop(void) {
    watchDog_loop();
    cli_loop();
    httpServer_loop();
#ifdef USE_WLAN
    extern void wifistation_loop(void);
    wifistation_loop();
#endif
#ifdef USE_LAN
    extern void ethernet_loop(void);
    ethernet_loop();
#endif
#ifdef USE_NTP
    extern void ntp_loop(void);
    ntp_loop();
#endif


    int n = stm32_read(buf, BUF_SIZE - 1);
    if (n > 0) {
        buf[n] = '\0';

        if (watchDog_checkCommandLine(buf)) {
        } else if (cli_checkStm32CommandLine(buf)) {
        }


#ifdef USE_MQTT

#endif
        D(db_printf("from-stm32: <%s>\n", buf));
    }


}
