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

#include "mqtt_interface.h"

// a place store the user's callback
// they pass it on _init() and we need it
// later in our callback handler
typedef void (*state_change_handler)(void);
state_change_handler userStateHandler;
char *userDeltaBuffer;

typedef struct {
  char *pThingId;
  char *pHost;
  int port;
  char *pRootCA;
  char *pClientCRT;
  char *pClientKey;
  state_change_handler stateChangeHandler;
  char *deltaBuffer;
} ShadowParameters;

char shadowRxBuf[256];

IoT_Error_t iot_shadow_init(ShadowParameters *shadowParams);
IoT_Error_t iot_shadow_connect(void);
IoT_Error_t iot_shadow_sync_reported(char *reportedJson);
IoT_Error_t iot_shadow_sync_desired(char *reportedJson);
IoT_Error_t iot_shadow_yield(int timeout);
IoT_Error_t iot_shadow_disconnect(void);
