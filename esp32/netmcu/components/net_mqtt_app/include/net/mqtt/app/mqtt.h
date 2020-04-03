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


void io_mqtt_publish_config(const char *json);
void io_mqtt_publish_status(const char *json);
void io_mqtt_publish_valve_status(int valve_number, bool state);
void io_mqtt_publish_rain_sensor_status(bool state);
void io_mqtt_publish_pump_status(bool state);
void io_mqtt_publish_stm32_event(const char *event);

// interface implemented in ./mqtt.c called by MQTT implementation (mcu specific)
void io_mqtt_received(const char *topic, int topic_len, const char *data, int data_len);

void io_mqttApp_setup(struct cfg_mqtt *cfg_mqtt);

#endif /* SRC_net_mqtt_app_H_ */
