/*
 * mqtt.c
 *
 *  Created on: 19.03.2019
 *      Author: bertw
 */

#include "app_config/proj_app_cfg.h"
#ifdef USE_MQTT

#include "net/mqtt/app/mqtt.h"

#include <string.h>
#include <stdio.h>
#include "cli/cli.h"
#include "cli/mutex.hh"
#include "userio/status_json.h"
#include "userio_app/status_output.h"

#include <array>

static char topic_root[16] = "rv/";
static size_t topic_root_len = 3;

#define TOPIC_CLI  "cli"
#define TOPIC_STATUS  "status"
#define TOPIC_DUR_END "/duration"
#define TOPIC_DUR_MID "+"
#define TOPIC_CMD  TOPIC_DUR_MID TOPIC_DUR_END

#define TAG_CLI "cli "
#define TAG_CLI_LEN (sizeof(TAG_CLI) - 1)
#define TAG_SEND "cmd "
#define TAG_SEND_LEN (sizeof(TAG_SEND) - 1)
#define TAG_CONFIG "config "
#define TAG_CONFIG_LEN (sizeof(TAG_CONFIG) - 1)
#define TAG_TIMER "timer "
#define TAG_TIMER_LEN (sizeof(TAG_TIMER) - 1)

void io_mqtt_publish_config(const char *s)  {
  if (so_tgt_test(SO_TGT_MQTT)) //FIXME: implement this better
    io_mqtt_publish("tfmcu/config_out", s);
}


void io_mqtt_publish_valve_status(int valve_number, bool state) {
  char topic[64]; //, data[16];

  snprintf(topic, 64, "%szone/%d/valve", topic_root, valve_number);

  io_mqtt_publish(topic, state ? "on" : "off");
}

void io_mqtt_publish_rain_sensor_status(bool state) {
  char topic[64]; //, data[16];

  snprintf(topic, 64, "%s%s/rain", topic_root, TOPIC_STATUS);

  io_mqtt_publish(topic, state ? "on" : "off");
}

void io_mqtt_publish_pump_status(bool state) {
  char topic[64]; //, data[16];

  snprintf(topic, 64, "%s%s/pump", topic_root, TOPIC_STATUS);

  io_mqtt_publish(topic, state ? "on" : "off");
}

void io_mqtt_publish_stm32_event(const char *event) {
  char topic[64]; //, data[16];

  snprintf(topic, 64, "%s%s/event", topic_root, TOPIC_STATUS);

  io_mqtt_publish(topic, event);
}



// implementation interface
static void io_mqtt_connected () {
  std::array<char,80> buf;

  io_mqtt_subscribe(strcat(strcpy(buf.data(), topic_root), TOPIC_CLI), 0);
  io_mqtt_subscribe(strcat(strcpy(buf.data(), topic_root), TOPIC_CMD), 0);
  io_mqtt_publish(strcat(strcpy(buf.data(), topic_root), TOPIC_CMD), "connected"); // for autocreate (ok???)
}

static void io_mqtt_received(const char *topic, int topic_len, const char *data, int data_len) {

  if (!topic_startsWith(topic, topic_len, topic_root)) {
    return; // all topics start with this
  }

  topic += topic_root_len;
  topic_len -= topic_root_len;

  { LockGuard lock(cli_mutex); 
    char line[40 + data_len];
    if (topic_endsWith(topic, topic_len, TOPIC_DUR_END)) {
      const char *addr = topic;
      int addr_len = topic_len - (sizeof TOPIC_DUR_END - 1);
      sprintf(line, "cmd dur%.*s=%.*s", addr_len, addr, data_len, data);
      cli_process_cmdline(line, SO_TGT_MQTT);
    } else if (strlen(TOPIC_CLI) == topic_len && 0 == strncmp(topic, TOPIC_CLI, topic_len)) {
      if (data_len > TAG_CLI_LEN && strncmp(data, TAG_CLI, TAG_CLI_LEN) == 0) {
        data += TAG_CLI_LEN;
        data_len -= TAG_CLI_LEN;
      }
      memcpy(line, data, data_len);
      line[data_len] = '\0';
      cli_process_cmdline(line, SO_TGT_MQTT);
    } else if ((0 == strncmp(data, TAG_SEND, TAG_SEND_LEN)) || (0 == strncmp(data, TAG_CONFIG, TAG_CONFIG_LEN))
        || (0 == strncmp(data, TAG_TIMER, TAG_TIMER_LEN))) {
      memcpy(line, data, data_len);
      line[data_len] = '\0';
      cli_process_cmdline(line, SO_TGT_MQTT);
    }

    // RETURN:
  }
}


void io_mqttApp_setup(struct cfg_mqtt_app *cfg) {
  topic_root_len = strlen(cfg->topic_root);
  strcpy(topic_root, cfg->topic_root);
  io_mqtt_received_cb = io_mqtt_received;
  io_mqtt_connected_cb = io_mqtt_connected;
}


#endif // USE_MQTT

