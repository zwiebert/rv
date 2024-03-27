#include "app_config/proj_app_cfg.h"
#include "stm32_com/com_task.h"

#include "stm32/stm32.h"
#include "app_misc/common.h"
#include "cli/cli.h"
#include "cli/mutex.hh"
//#include "esp_event.h"
#include "esp_log.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/task.h"
#include "lwip/netdb.h"
#include "lwip/sockets.h"
#include "utils_misc/int_types.h"
#include "net_http_server/http_server_setup.h"
#include "net/tcp_cli_server.h"
#include <uout/uo_callbacks.h>
#include "time.h"
#include "txtio/inout.h"
#include "uout/status_json.hh"
#include <errno.h>
#include <string.h>
#include <sys/select.h>

#ifdef CONFIG_RV_NETMCU_DEBUG
#define DEBUG
#define D(x) x
#define DD(x) x
#else
#define D(x)
#define DD(x)
#endif
#define logtag "rv.stm32_com"

#define TRACE_MARKER "trace:"
#define TRACE_MARKER_LEN (sizeof TRACE_MARKER - 1)

#define BUF_SIZE 512
static char line[BUF_SIZE];

class UoutWriterStm32 final: public UoutWriter {
public:
  UoutWriterStm32(so_target_bits tgt) :
      UoutWriter(static_cast<so_target_bits>(tgt | SO_TGT_STM32)) {
  }

  UoutWriterStm32(const UoutWriterStm32&) = delete;
  virtual ~UoutWriterStm32() = default;

private:
  virtual int priv_write(const char *s, ssize_t len, bool final) override {
    return stm32_write(s, len);
  }
};

static bool stmTrace_checkCommandLine(const char *line) {
  if (strncmp(TRACE_MARKER, line, TRACE_MARKER_LEN) != 0)
    return false;

  D(ESP_LOGI(logtag, "trace command line: <%s>", line));
  return true;
}

static bool stm32com_get_commandline(char *buf, size_t buf_size) {
  auto n = stm32_read_line(buf, buf_size, 1000);
  if (n <= 0)
    return false;

  if (buf[n - 1] == '\n') {
    buf[n - 1] = '\0';
    return true;
  } else {
    ESP_LOGE(logtag, "%s: buffer full before line end. %d characters received", __func__, n);
    return false;
  }
  return false;

}

static bool do_work() {
  if (!stm32com_get_commandline(line, sizeof line)) {
    return false;
  }
  if (watchDog_checkCommandLine(line))
    return true;
  if (stmTrace_checkCommandLine(line))
    return true;

  D(ESP_LOGI(logtag, "from_rv: received: <%s>", line));

  char *json = strstr(line, "{\"status\":");
  if (!json)
    json = strstr(line, "{\"pbuf\":");
  if (!json)
    json = strstr(line, "{\"kvs\":");
  if (!json)
    json = strstr(line, "{\"to\":\"cli\",");

  if (json) {
    LockGuard lock(cli_mutex);

    UoutWriterStm32 td { static_cast<so_target_bits>(SO_TGT_FLAG_JSON) };
    DD(ESP_LOGI(logtag, "from_rv:request: <%s>", json));
    cli_process_json(json, td);

    if (td.sj().get_json()) {
      DD(ESP_LOGI(logtag, "from_netmcu:response: <%s>", td.sj().get_json()));
      LockGuard lock(stm32_mutex);

      stm32_write(td.sj().get_json(), strlen(td.sj().get_json()));
      stm32_write("\n", 2);

      td.sj().free_buffer();
    }
  }

  char *reply = strstr(line, "{\"data\":");
  if (!reply)
    reply = strstr(line, "{\"update\":");
  if (reply) {
    uoCb_publish_wsJson(reply);
    D(ESP_LOGI(logtag, "to_webstream:reply: <%s>", reply));
  }

  return true;
}

static void stm32com_task(void *pvParameters) {
  for (;;) {
    if (!do_work()) {
      // it seems the UART might not work right not (could be in BOOTLOADER mode with no event queue)
      // just wait before trying again to avoid busy-loop
      vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
  }
}

static TaskHandle_t xHandle = NULL;
#define STACK_SIZE  5000

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
  configASSERT(xHandle);

}

