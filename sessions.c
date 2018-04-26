
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/time.h>
#include <time.h>
#include <errno.h>
#include <err.h>
#include <sys/time.h>
#include "mqtt.h"
#include <cjson/cJSON.h>

#include "hiredis/hiredis.h"
#include "hiredis/async.h"

#define M_PI 3.14159265358979323846

const static char *CERTS="certs";

const static char *REDIS_HOST = "127.0.0.1";
const static int REDIS_PORT = 6379;

const static int SESSION_EXPIRE_SECONDS = 30;
const static int PINGS_TO_NOTIFY_OF_NEW_SESSION = 2;

redisContext *c;
redisReply *reply;
IoT_Error_t rc;

long long current_timestamp() {
    struct timeval te; 
    gettimeofday(&te, NULL); // get current time
    long long milliseconds = te.tv_sec*1000LL + te.tv_usec/1000; // calculate milliseconds
    // printf("milliseconds: %lld\n", milliseconds);
    return milliseconds;
}

void initializeMqtt() {

	rc = MQTT_Connect("a3k5zrbvalc2ff.iot.us-east-1.amazonaws.com", 8883, "monitor1", "certs/rootCA.pem", "certs/cert.crt", "certs/private.key");
	if (NONE_ERROR != rc) {
		printf("Error[%d] connecting", rc);
	} else {
        printf("Connected to MQTT Broker!\n");
    }

    sendMessage("log", "{ \"hotspot\": \"1\", \"status\":\"Sniffer started\" }");

}

void initializeRedis() {

    struct timeval timeout = { 1, 500000 }; // 1.5 seconds
    c = redisConnect(REDIS_HOST, REDIS_PORT);
    if (c == NULL || c->err) {
        if (c) {
            printf("Error: %s\n", c->errstr);
            // handle error
        } else {
            printf("Can't allocate redis context\n");
        }
    } else {
        printf("Connected to Redis!\n");
    }
}

float calc_distance(float freq, float sig) {


    if (freq == 0) {
        return -1;
    }
    
    float lx = 300.0 / freq;
    float distcorr = 1;
    
    float g1 = 0;
    float g2 = 0;
    
    g1 = g1 / 10;
    g1 = pow(10,g1);
    g2 = g2 / 10;
    g2 = pow(10,g2);
    sig = sig / 10;
    sig = pow(10, sig);  
    
    lx = 300.0 / freq;
    
    float dx = lx * sqrt(g1 * g2 / sig) / (4.0 * 3.14);
    
    dx = dx / distcorr;
    
    dx = round(dx * 1000) / 1000; 
    
    if (dx >= 10)
        dx = round(dx * 100) / 100;
    
    if (dx >= 100)
        dx = round(dx * 10) / 10;
    
    if (dx >= 1000) {
        dx = round(dx);
    }

    return dx;
 
}

int sendMessage(char *topic, char *msg) {

    rc = MQTT_Send_Message(topic, msg, strlen(msg));
	if (NONE_ERROR != rc)
		ERROR("Could not publish to topic: %s", topic );
    return rc;
}

float get_average_distance(cJSON *pings) {

    float total_distance=0;
    float average_distance=0;

    for (int i=0; i<cJSON_GetArraySize(pings); i++) {
        cJSON *ping = cJSON_GetArrayItem(pings, i);
        total_distance += calc_distance(cJSON_GetObjectItem(ping, "f")->valueint, cJSON_GetObjectItem(ping, "s")->valueint);
    }

    average_distance = (float)total_distance / cJSON_GetArraySize(pings);

    return average_distance;
}

void closeSession(cJSON *session_details) {
    printf("Need to expire: %s\n", cJSON_GetObjectItem(session_details, "device")->valuestring);

    cJSON_AddNumberToObject(session_details, "averageDistance",  get_average_distance(cJSON_GetObjectItem(session_details, "pings")));

    //TODO: store richer information about a session

    cJSON_DeleteItemFromObject(session_details, "pings");

    sendMessage("close_session", cJSON_Print(session_details));

    return;
}

void newSession(cJSON *session_details) {
    printf("New session with %s\n", cJSON_GetObjectItem(session_details, "device")->valuestring);

    cJSON_AddNumberToObject(session_details, "averageDistance",  get_average_distance(cJSON_GetObjectItem(session_details, "pings")));

    cJSON_DeleteItemFromObject(session_details, "pings");

    sendMessage("open_session", cJSON_Print(session_details));

    return;
}

void checkKeys() {

    printf("Checking all open sessions\n");

    redisReply * keysReply;
    char * key;
    char * device_mac;
    int status=0;
    double time_since_update;

    reply = redisCommand(c,"GET sessions");

    if (!reply->str) {
        return;
    }

    cJSON *list = cJSON_Parse(reply->str);
    cJSON *newList = cJSON_CreateArray();

    for (int i=0; i<cJSON_GetArraySize(list); i++) {
        
        cJSON *session = cJSON_GetArrayItem(list, i);
        device_mac = cJSON_GetObjectItem(session, "device")->valuestring;
        status = cJSON_GetObjectItem(session, "status")->valueint;
        
        reply = redisCommand(c,"GET session_%s", device_mac);

        if (!reply->str) {
            continue;
        }
        
        cJSON *session_details = cJSON_Parse(reply->str);
        time_since_update = current_timestamp() - cJSON_GetObjectItem(session_details, "updated")->valuedouble;
        printf("Last update was %lf seconds ago\n", time_since_update / 1000);



        //if session expired
        if (time_since_update / 1000 > SESSION_EXPIRE_SECONDS) {
            closeSession(session_details);
            redisCommand(c,"DEL session_%s", device_mac);
        } else {
            
            //if session is new and qualifies to be reported on, report
            if ((status == 1) && (cJSON_GetObjectItem(session_details, "pingCount")->valueint > PINGS_TO_NOTIFY_OF_NEW_SESSION)) {
                newSession(session_details);
                status = 0;
            }

            //add the session back to the list
            cJSON_AddItemToArray(newList, session=cJSON_CreateObject());
            cJSON_AddNumberToObject(session, "status", status);
            cJSON_AddStringToObject(session, "device", device_mac);
        }
        
    }

    reply = redisCommand(c,"SET sessions %s", cJSON_Print(newList));


    return;
}

int main(int argc, char** argv) {


    initializeRedis();

    initializeMqtt();

    checkKeys();

    printf("Session check complete\n");

    return 0;
    
}


/*
keysReply = redisCommand(c,"keys sess_*");

for (int i=0; i<keysReply->elements; i++) {
    key = keysReply->element[i]->str;
    reply = redisCommand(c,"GET %s", key);
    printf("keys *: %s\n", key);
}*/
