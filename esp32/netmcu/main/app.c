#include "cli/cli.h"
#include "debug/debug.h"
#include "uart.h"

#define BUF_SIZE 256
char buf[BUF_SIZE];

void loop(void) {
	cli_loop();

	int n = stm32_read(buf, BUF_SIZE-1);
	if (n > 0) {
		buf[n] = '\0';
		db_printf("from-stm32: <%s>\n", buf);
	}

}
