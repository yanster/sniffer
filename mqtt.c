/*
 * mqtt.c
 *
 *  Created on: Sep 15, 2015
 *      Author: robert
 */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>

#include "mqtt.h"

IoT_Error_t MQTT_Send_Message(char* topic, char* payload, int payload_len)
{


	MQTTMessageParams Msg;
	Msg.qos = QOS_0;
	Msg.isRetained = false;

	Msg.pPayload = (void *) payload;
	Msg.PayloadLen = payload_len;

	MQTTPublishParams Params;
	Params.pTopic = topic;
	Params.MessageParams = Msg;


	//publish the event to the topic
	return iot_mqtt_publish(&Params);
}

//
//Subscribe for topic
//
IoT_Error_t MQTT_Subscribe(char* topic, QoSLevel qos, iot_message_handler MQTTcallbackHandler)
{

	MQTTSubscribeParams subParams;
	subParams.mHandler = MQTTcallbackHandler;
	subParams.pTopic = topic;
	subParams.qos = qos;

	return iot_mqtt_subscribe(&subParams);
}

//
//
//
MQTT_Connect(char* hostaddr, uint16_t port, char* thingID, char* rootCA, char* clientCRT, char* clientKey)
{
	IoT_Error_t rc = NONE_ERROR;

	//Setting up connection params
	MQTTConnectParams connectParams;
	connectParams.KeepAliveInterval_sec = 100;
	connectParams.isCleansession = true;
	connectParams.MQTTVersion = MQTT_3_1_1;
	connectParams.pClientID = "CSDK-test-device"; //thingID;
	connectParams.pHostURL = hostaddr;
	connectParams.port = port;
	connectParams.isWillMsgPresent = false;
	connectParams.pUserName = NULL;
	connectParams.pPassword = NULL;
	connectParams.pRootCALocation = rootCA;
	connectParams.pDeviceCertLocation = clientCRT;
	connectParams.pDevicePrivateKeyLocation = clientKey;
	connectParams.commandTimeout_ms = 10000;
	connectParams.isSSLHostnameVerify = true;// ensure this is set to true for production

	//Connect
	INFO("Connecting...");
	return iot_mqtt_connect(&connectParams);
}