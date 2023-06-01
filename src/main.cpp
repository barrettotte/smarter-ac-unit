#include <ESP8266WiFi.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <PubSubClient.h>

#include <config.h>

#define AC_MODEL 0x8306

enum AcCommand {
    CMD_TEMP_CHANGE = 0x02,
    CMD_PWR_TOGGLE = 0x25
};

enum FanSpeed {
    SPEED_LOW = 1,
    SPEED_HIGH = 3
};

enum FanMode {
    MODE_COOL = 2,
    MODE_DRY = 3,
    MODE_FAN = 4
};

struct AcConfig {
    FanSpeed speed;     // low=0, high=1
    bool power;          // power off/on
    AcCommand command;   // AC command
    uint8_t temperature; // temperature (F)
};

/** globals ***/

WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);
IRsend irSend(IR_LED_PIN);

/*** utils ***/

void mqttCallback(char* topic, byte* payload, unsigned int length) {
    // TODO: listen for messages on: power, fan speed, temperature
    // parse json
    // update config struct

    Serial.print("Message arrived on topic: ");
    Serial.println(topic);
    Serial.print("Message: ");

    for (unsigned int i = 0; i < length; i++) {
        Serial.print((char) payload[i]);
    }
    Serial.println();
}

void mqttReconnect() {
    while (!mqttClient.connected()) {
        Serial.print("Connecting to MQTT...");

        String clientId = MQTT_CLIENT_ID;
        clientId += "-" + String(WiFi.macAddress());

        if (!mqttClient.connect(clientId.c_str(), MQTT_USER, MQTT_PASSWORD)) {
            Serial.print("Failed to connect to MQTT. rc=");
            Serial.println(mqttClient.state());
            Serial.println("Retrying in 5 seconds...");
            delay(5000);
        }
    }
    Serial.println("Connected to MQTT!");
    mqttClient.subscribe(MQTT_TOPIC_CONTROL);
}

void mqttDiscover() {
    char mqttPayload[256];
    sprintf(mqttPayload, MQTT_PAYLOAD_DISCOVER, HA_NAME, MQTT_CLIENT_ID, AC_TEMP_MIN, AC_TEMP_MAX, 
        MQTT_TOPIC_CONTROL, MQTT_TOPIC_STATE);
    mqttClient.publish(MQTT_TOPIC_HA_DISCOVER, mqttPayload);
}

void mqttPublish() {
    // {
    //     "power": "off",
    //     "fan_speed": "high",
    //     "temperature": 70
    // }
}

/*** main ***/

void setup() {
    // init serial
    Serial.begin(115200);
    Serial.println();

    for (uint8_t t = 3; t > 0; t--) {
        Serial.printf("WAIT %d...\n", t);
        Serial.flush();
        delay(500);
    }
    Serial.println("\n* * * START * * *");

    // init pins
    pinMode(LED_BUILTIN, OUTPUT);
    pinMode(WIFI_TRIGGER_PIN, INPUT_PULLUP);

    // init wifi
    WiFi.mode(WIFI_STA); // esp defaults to STA+AP
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    Serial.print("Connecting to Wi-Fi");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.print("\nIP address: ");
    Serial.println(WiFi.localIP());

    // init infrared LED
    irSend.begin();

    // init MQTT client
    mqttClient.setServer(MQTT_SERVER, MQTT_PORT);
    mqttClient.setCallback(mqttCallback);
    mqttClient.setKeepAlive(60);
    mqttClient.setSocketTimeout(10);
    // mqttReconnect();

    // home assistant discover
    // mqttDiscover();

    Serial.println("Setup done.");
}

// 0x830601A20000C000000000000063001100803800A9 (168 bits)
uint8_t data[21] = {
    0x83, 0x06, 0x01, 0xA2, 0x00, 0x00, 0xC0, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x63, 0x00, 0x11,
    0x00, 0x80, 0x38, 0x00, 0xA9
};

void loop() {
    if (!mqttClient.connected()) {
        mqttReconnect();
    } else {
        // TODO: publish timer
        // mqttPublish();
    }
    mqttClient.loop();

    // irSend.sendWhirlpoolAC(data, 21);
    delay(1000);
    // TODO: delay(50);
}
