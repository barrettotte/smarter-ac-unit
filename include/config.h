#pragma once

#include "secrets.h"

#define WIFI_TRIGGER_PIN D1  // pin to trigger wifi config portal
#define IR_LED_PIN D2        // IR LED transmit pin

#define WIFI_PORTAL_SECS 300 // max wifi portal time

#define AC_TEMP_MAX 86
#define AC_TEMP_MIN 60
#define AC_TEMP_DEFAULT 70

const char* HA_NAME = "Bedroom AC (LG LP1017WSR)";

const char* MQTT_SERVER = "giza.agartha";
const int MQTT_PORT = 1883;
const char* MQTT_CLIENT_ID = "esp8266";

const char* MQTT_TOPIC_HA_DISCOVER = "homeassistant/climate/config";
const char* MQTT_TOPIC_CONTROL = "smarter-ac-unit/control";
const char* MQTT_TOPIC_STATE = "smarter-ac-unit/state";

const char* MQTT_PAYLOAD_DISCOVER = ""\
    "{"\
        "\"name\":\"%s\","\
        "\"unique_id\":\"%s\","\
        "\"modes\":[\"off\",\"on\"],"\
        "\"fan_modes\":[\"low\",\"high\"],"\
        "\"min_temp\":%d,"\
        "\"max_temp\":%d,"\
        "\"command_topic\":\"%s\","\
        "\"state_topic\":\"%s\""\
    "}";

const char* MQTT_PAYLOAD_STATE = ""\
    "{"\
        "\"power\":\"%s\","\
        "\"fan_speed\":\"%s\","\
        "\"temperature\":\"%s\""\
    "}";
