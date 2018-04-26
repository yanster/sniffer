/*
 * mqtt.h
 *
 *  Created on: Sep 15, 2015
 *      Author: robert
 */

#ifndef MQTT_H_
#define MQTT_H_

#include "mqtt_interface.h"
#include "iot_version.h"
#include "iot_log.h"

int MQTTcallbackHandler(MQTTCallbackParams params);
IoT_Error_t MQTT_Connect(char* hostaddr, uint16_t port, char* thingID, char* rootCA, char* clientCRT, char* clientKey);
IoT_Error_t MQTT_Subscribe(char* topic, QoSLevel qos, iot_message_handler MQTTcallbackHandler);
IoT_Error_t MQTT_Send_Message(char* topic, char* payload, int payload_len);


#endif /* MQTT_H_ */