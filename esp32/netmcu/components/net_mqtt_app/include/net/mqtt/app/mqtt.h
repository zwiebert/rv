/*
 * mqtt.h
 *
 *  Created on: 16.03.2019
 *      Author: bertw
 */

#ifndef SRC_net_mqtt_app_H_
#define SRC_net_mqtt_app_H_

#include "app_config/proj_app_cfg.h"
#include "net/mqtt/mqtt.h"

void io_mqtt_enable(bool enable);

void io_mqtt_publish_config(const char *json);

#ifdef USE_MQTT
void io_mqtt_publish_config(const char *json);
void io_mqtt_publish_status(const char *json);
void io_mqtt_publish_valve_status(int valve_number, bool state);
void io_mqtt_publish_rain_sensor_status(bool state);
void io_mqtt_publish_pump_status(bool state);
void io_mqtt_publish_stm32_event(const char *event);
#else
#define io_mqtt_publish_config(json)
#define io_mqtt_publish_status(json)
#define io_mqtt_publish_valve_status(valve_number, state)
#define io_mqtt_publish_rain_sensor_status(state)
#define io_mqtt_publish_pump_status(state)
#define io_mqtt_publish_stm32_event(event)
#endif


struct cfg_mqtt_app {
  char topic_root[16];
};

void io_mqttApp_setup(struct cfg_mqtt_app *cfg);

#endif /* SRC_net_mqtt_app_H_ */
