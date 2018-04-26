/*
 * Copyright 2010-2015 Amazon.com, Inc. or its affiliates. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License").
 * You may not use this file except in compliance with the License.
 * A copy of the License is located at
 *
 *  http://aws.amazon.com/apache2.0
 *
 * or in the "license" file accompanying this file. This file is distributed
 * on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 * express or implied. See the License for the specific language governing
 * permissions and limitations under the License.
 */

#ifndef AWS_IOT_SDK_SRC_IOT_MQTT_INTERFACE_H_
#define AWS_IOT_SDK_SRC_IOT_MQTT_INTERFACE_H_

#include "iot_error.h"
#include "stddef.h"
#include "stdbool.h"
#include "stdint.h"

typedef enum {
	MQTT_3_1 = 3, MQTT_3_1_1 = 4
} MQTT_Ver_t;

typedef enum {
	QOS_0, QOS_1, QOS_2
} QoSLevel;

typedef struct {
	const char *pTopicName;
	const char *pMessage;
	bool isRetained;
	QoSLevel qos;
} MQTTwillOptions;

typedef struct {
	char *pHostURL;
	uint16_t port;
	char *pRootCALocation;
	char *pDeviceCertLocation;
	char *pDevicePrivateKeyLocation;
	char *pClientID;
	char *pUserName;
	char *pPassword;
	MQTT_Ver_t MQTTVersion;
	uint16_t KeepAliveInterval_sec;
	bool isCleansession;
	bool isWillMsgPresent;
	MQTTwillOptions will;
	uint32_t commandTimeout_ms;
	bool isSSLHostnameVerify;
} MQTTConnectParams;

typedef struct {
	QoSLevel qos;
	bool isRetained;
	bool isDuplicate;
	uint16_t id;
	void *pPayload;
	uint32_t PayloadLen;
} MQTTMessageParams;

typedef struct {
	char *pTopicName;
	uint16_t TopicNameLen;
	MQTTMessageParams MessageParams;
} MQTTCallbackParams;

typedef int32_t (*iot_message_handler)(MQTTCallbackParams params);

typedef struct {
	char *pTopic;
	QoSLevel qos;
	iot_message_handler mHandler;
} MQTTSubscribeParams;

typedef struct {
	char *pTopic;
	MQTTMessageParams MessageParams;
} MQTTPublishParams;

IoT_Error_t iot_mqtt_connect(MQTTConnectParams *pParams);
IoT_Error_t iot_mqtt_publish(MQTTPublishParams *pParams);
IoT_Error_t iot_mqtt_subscribe(MQTTSubscribeParams *pParams);
IoT_Error_t iot_mqtt_unsubscribe(char *pTopic);
IoT_Error_t iot_mqtt_disconnect(void);
IoT_Error_t iot_mqtt_yield(int timeout);

#endif /* AWS_IOT_SDK_SRC_IOT_MQTT_INTERFACE_H_ */
