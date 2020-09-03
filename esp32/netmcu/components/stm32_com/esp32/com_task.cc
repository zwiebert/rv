#include "app_config/proj_app_cfg.h"
#include "stm32_com/com_task.h"

#include "stm32/stm32.h"
#include "app/common.h"
#include "cli/cli.h"
#include "cli/mutex.hh"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/task.h"
#include "lwip/netdb.h"
#include "lwip/sockets.h"
#include "misc/int_types.h"
#include "net/http/server/http_server.h"
#include "net/tcp_cli_server.h"
#include "time.h"
#include "txtio/inout.h"
#include "userio/status_json.h"
#include <errno.h>
#include <string.h>
#include <sys/select.h>

#define printf(...) io_printf_v(vrb3, __VA_ARGS__)
#define perror(s)   io_printf_v(vrb3, "%s: %s\n", s, strerror(errno))

#ifdef DISTRIBUTION
#define D(x)
#define DD(x)
#else
#define D(x) x
#define DD(x) x
#endif

#define TRACE_MARKER "trace:"
#define TRACE_MARKER_LEN (sizeof TRACE_MARKER - 1)

#define BUF_SIZE 512
static char line[BUF_SIZE];

static bool stmTrace_checkCommandLine(const char *line) {
  if (strncmp(TRACE_MARKER, line, TRACE_MARKER_LEN) != 0)
    return false;

  printf("stm32:%s\n", line);
  return true;
}


static void do_work() {
  int c;

  int i = 0;

  for (i = 0; ((c = stm32_getc(true)) != '\n'); ++i) {
    if (c == -1)
      return; // looks like a timeout. throw away received data
    if (i + 2 >= BUF_SIZE)
      return;
    line[i] = c;
  }
  line[i] = '\0';

  if (watchDog_checkCommandLine(line))
    return;
  if (stmTrace_checkCommandLine(line))
    return;

  D(printf("stm32com:recv: <%s>\n", line));



  char *json = strstr(line, "{\"status\":");
  if (!json)
    json = strstr(line, "{\"pbuf\":");
  if (!json)
    json = strstr(line, "{\"kvs\":");
  if (!json)
    json = strstr(line, "{\"to\":\"cli\",");

  if (json) {
    auto lock = ThreadLock(cli_mutex);
    DD(printf("stm32com:request: <%s>\n", json));
    cli_process_json(json, static_cast<so_target_bits>(SO_TGT_ANY | SO_TGT_STM32));

    if (sj_get_json()) {
      DD(printf("stm32com:response: <%s>\n", sj_get_json()));
      if (stm32_mutexTake()) {
        stm32_write(sj_get_json(), strlen(sj_get_json()));
        stm32_write("\n", 2);
        stm32_mutexGive();
        sj_free_buffer();
      }
    }
  }

  char *reply = strstr(line, "{\"data\":");
  if (!reply)
    reply = strstr(line, "{\"update\":");
  if (reply) {
    ws_print_json(reply);
    DD(printf("stm32com:recv:####REPLY####: <%s>\n", reply));
  }
}


static void stm32com_task(void *pvParameters) {
  for (;;) {
    do_work();
  }
}

static TaskHandle_t xHandle = NULL;
#define STACK_SIZE  3000

void stm32com_setup_task(const struct cfg_stm32com *cfg_stm32com) {
  static uint8_t ucParameterToPass;

  if (!cfg_stm32com || !cfg_stm32com->enable) {
    if (xHandle) {
      vTaskDelete(xHandle);
      xHandle = NULL;
    }
    return;
  }

  xTaskCreate(stm32com_task, "stm32com", STACK_SIZE, &ucParameterToPass, tskIDLE_PRIORITY, &xHandle);
  configASSERT( xHandle );

}


