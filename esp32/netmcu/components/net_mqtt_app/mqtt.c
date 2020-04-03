/*
 * mqtt.c
 *
 *  Created on: 19.03.2019
 *      Author: bertw
 */

#include "app_config/proj_app_cfg.h"
#ifdef USE_MQTT

#include "mqtt.h"
#include "mqtt_imp.h"

#include <string.h>
#include <stdio.h>
#include "cli/cli.h"
#include "cli/mutex.h"
#include "userio/status_json.h"
#include "userio_app/status_output.h"

#ifdef DISTRIBUTION
#define TOPIC_ROOT "rv/"
#else
#define TOPIC_ROOT "rvdb/"
#endif

#define TOPIC_CLI TOPIC_ROOT "cli"
#define TOPIC_STATUS TOPIC_ROOT "status"
#define TOPIC_DUR_END "/duration"
#define TOPIC_DUR_MID "+"
#define TOPIC_CMD TOPIC_ROOT TOPIC_DUR_MID TOPIC_DUR_END

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

  snprintf(topic, 64, "%szone/%d/valve", TOPIC_ROOT, valve_number);

  io_mqtt_publish(topic, state ? "1" : "0");
}

void io_mqtt_publish_rain_sensor_status(bool state) {
  char topic[64]; //, data[16];

  snprintf(topic, 64, "%s/rain", TOPIC_STATUS);

  io_mqtt_publish(topic, state ? "1" : "0");
}

void io_mqtt_publish_pump_status(bool state) {
  char topic[64]; //, data[16];

  snprintf(topic, 64, "%s/pump", TOPIC_STATUS);

  io_mqtt_publish(topic, state ? "1" : "0");
}

void io_mqtt_publish_stm32_event(const char *event) {
  char topic[64]; //, data[16];

  snprintf(topic, 64, "%s/event", TOPIC_STATUS);

  io_mqtt_publish(topic, event);
}



// implementation interface
void io_mqtt_connected () {
  io_mqtt_subscribe(TOPIC_CLI, 0);
  io_mqtt_subscribe(TOPIC_CMD, 0);
  io_mqtt_publish(TOPIC_STATUS, "connected"); // for autocreate (ok???)
}

void io_mqtt_received(const char *topic, int topic_len, const char *data, int data_len) {

  if (!topic_startsWith(topic, topic_len, TOPIC_ROOT)) {
    return; // all topics start with this
  }

  if (mutex_cliTake()) {

    if (topic_endsWith(topic, topic_len, TOPIC_DUR_END)) {
      char *line = set_commandline("x", 1);
      const char *addr = topic + (sizeof TOPIC_ROOT - 1);
      int addr_len = topic_len- ((sizeof TOPIC_ROOT - 1) + (sizeof TOPIC_DUR_END - 1));
      sprintf(line, "cmd dur%.*s=%.*s", addr_len, addr, data_len, data);
      cli_process_cmdline(line, SO_TGT_MQTT);
    } else if (strlen(TOPIC_CLI) == topic_len && 0 == strncmp(topic, TOPIC_CLI, topic_len)) {
      if (strncmp(data, TAG_CLI, TAG_CLI_LEN) == 0) {
        char *line;
        if ((line = set_commandline(data + TAG_CLI_LEN, data_len - TAG_CLI_LEN))) {
          cli_process_cmdline(line, SO_TGT_MQTT);
        }
      } else if ((0 == strncmp(data, TAG_SEND, TAG_SEND_LEN)) || (0 == strncmp(data, TAG_CONFIG, TAG_CONFIG_LEN))
          || (0 == strncmp(data, TAG_TIMER, TAG_TIMER_LEN))) {
        char *line;
        if ((line = set_commandline(data, data_len))) {
          cli_process_cmdline(line, SO_TGT_MQTT);
        }
      }
    }

   // RETURN:
    mutex_cliGive();
  }
}


void io_mqttApp_enable(bool enable) {
  if (enable) {
    sj_callback_onClose_ifNotEmpty = io_mqtt_publish_config;
  } else {
    sj_callback_onClose_ifNotEmpty = 0;
  }
}

void io_mqttApp_disconnected() {
}
void io_mqttApp_subscribed(const char *topic, int topic_len) {
}
void io_mqttApp_unsubscribed(const char *topic, int topic_len) {
}
void io_mqttApp_published(int msg_id) {
}


#ifdef DISTRIBUTION
#define CONFIG_MQTT_CLIENT_ID "rv"
#else
#define CONFIG_MQTT_CLIENT_ID "rvdbg"
#endif


void io_mqttApp_setup(struct cfg_mqtt *cfg_mqtt) {
  io_mqtt_setup(CONFIG_MQTT_CLIENT_ID, cfg_mqtt);
}


#endif // USE_MQTT

