#include "cli/cli.h"
#include "debug/debug.h"
#include "uart.h"
#include "userio/http_server.h"

#define BUF_SIZE 256
char buf[BUF_SIZE];

//#include "freertos/task.h"


void loop(void) {
	cli_loop();
        httpServer_loop();

	db_printf(".");
	int n = stm32_read(buf, BUF_SIZE-1);
	if (n > 0) {
		buf[n] = '\0';
#ifdef USE_MQTT

#endif
		db_printf("from-stm32: <%s>\n", buf);
	}


}
