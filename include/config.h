#pragma once

#include "secrets.h"

#define USE_MQTT_DISCOVERY 1

#define IR_LED_PIN D2 // IR LED transmit pin

#define AC_TEMP_MAX 86
#define AC_TEMP_MIN 60
#define AC_TEMP_DEFAULT 70

const char* HA_NAME = "Bedroom AC (LG LP1017WSR)";
const char* HA_UNIQUE_ID = "bedroom_ac_unit";

const char* MQTT_SERVER = "giza.agartha";
const int MQTT_PORT = 1883;
const char* MQTT_BASE_CLIENT_ID = "smarter-ac-unit-esp8266";

const char* MQTT_TOPIC_HA_DISCOVER = "homeassistant/climate/smarter_ac_unit/config";
const char* MQTT_TOPIC_MODE_COMMAND = "smarter_ac_unit/mode/set";
const char* MQTT_TOPIC_MODE_STATE = "smarter_ac_unit/mode/state";
const char* MQTT_TOPIC_TEMP_COMMAND = "smarter_ac_unit/temperature/set";
const char* MQTT_TOPIC_TEMP_STATE = "smarter_ac_unit/temperature/state";
const char* MQTT_TOPIC_FAN_COMMAND = "smarter_ac_unit/fan/set";
const char* MQTT_TOPIC_FAN_STATE = "smarter_ac_unit/fan/state";

const char* MQTT_PAYLOAD_DISCOVER = ""\
    "{"\
        "\"name\":\"%s\","\
        "\"unique_id\":\"%s\","\
        "\"modes\":[\"off\",\"cool\"],"\
        "\"fan_modes\":[\"low\",\"high\"],"\
        "\"min_temp\":%d,"\
        "\"max_temp\":%d,"\
        "\"initial\":%d,"\
        "\"temperature_unit\":\"F\","\
        "\"temp_step\":1,"\
        "\"optimistic\":true,"\
        "\"mode_command_topic\":\"%s\","\
        "\"mode_command_template\":\"{{ value if value=='off' else 'on' }}\","\
        "\"mode_state_topic\":\"%s\","\
        "\"temperature_command_topic\":\"%s\","\
        "\"temperature_state_topic\":\"%s\","\
        "\"fan_mode_command_topic\":\"%s\","\
        "\"fan_mode_state_topic\":\"%s\""\
    "}";
