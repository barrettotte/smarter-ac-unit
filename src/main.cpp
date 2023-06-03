#include <ESP8266WiFi.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <PubSubClient.h>

#include <config.h>

#define MQTT_MAX_PACKET_SIZE 2048 // redefined from 256 in PubSubClient

struct AcState {
    bool mode;  // 0=off, 1=cool
    bool speed; // 0=low, 1=high
    uint8_t temperature;
};

/** globals ***/

WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);
IRsend irSend(IR_LED_PIN);

/*** utils ***/

void mqttCallback(char* topic, byte* payload, unsigned int length) {
    Serial.print("Message arrived on topic: ");
    Serial.println(topic);

    // TODO: strcmp topics, else log warning
    // TODO: extract message and set AC state

    Serial.print("Message: ");
    for (unsigned int i = 0; i < length; i++) {
        Serial.print((char) payload[i]);
    }
    Serial.println();
}

void mqttReconnect() {
    String clientId = MQTT_BASE_CLIENT_ID;
    clientId += "-" + String(WiFi.macAddress());

    while (!mqttClient.connected()) {
        Serial.print("Connecting to MQTT broker...");

        if (!mqttClient.connect(clientId.c_str(), MQTT_USER, MQTT_PASSWORD)) {
            Serial.printf("Failed to connect to MQTT broker. rc=%d\nRetrying in 5 seconds...\n", mqttClient.state());
            delay(5000);
        }
    }
    Serial.println("Connected to MQTT!");

    mqttClient.subscribe(MQTT_TOPIC_MODE_COMMAND);
    mqttClient.subscribe(MQTT_TOPIC_TEMP_COMMAND);
    mqttClient.subscribe(MQTT_TOPIC_FAN_COMMAND);
}

void mqttDiscover() {
    char mqttPayload[MQTT_MAX_PACKET_SIZE];

    sprintf(mqttPayload, MQTT_PAYLOAD_DISCOVER, 
        HA_NAME, HA_UNIQUE_ID,
        AC_TEMP_MIN, AC_TEMP_MAX, AC_TEMP_DEFAULT,
        MQTT_TOPIC_MODE_COMMAND, MQTT_TOPIC_MODE_STATE,
        MQTT_TOPIC_TEMP_COMMAND, MQTT_TOPIC_TEMP_COMMAND,
        MQTT_TOPIC_FAN_COMMAND, MQTT_TOPIC_FAN_STATE
    );
    Serial.printf("Sending discovery payload %s\n", mqttPayload);
    mqttClient.publish(MQTT_TOPIC_HA_DISCOVER, mqttPayload);
}

// TODO: void mqttPublish(char* topic, uint8_t* payload)
// TODO: uint8_t* buildIrCode()

// TODO: void sendIrCode(uint8_t* irCode)
// irSend.sendWhirlpoolAC(data, 21);
//
// 0x830601A20000C000000000000063001100803800A9 (168 bits)
// uint8_t data[21] = {
//     0x83, 0x06, 0x01, 0xA2, 0x00, 0x00, 0xC0, 0x00, 
//     0x00, 0x00, 0x00, 0x00, 0x00, 0x63, 0x00, 0x11,
//     0x00, 0x80, 0x38, 0x00, 0xA9
// };

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
    mqttClient.setBufferSize(MQTT_MAX_PACKET_SIZE);

    // home assistant discovery
    if (USE_MQTT_DISCOVERY) {
        mqttReconnect();
        mqttDiscover();
    }
    Serial.println("Setup done.");
}

void loop() {
    if (!mqttClient.connected()) {
        mqttReconnect();
    }

    // TODO: publish mode, fan, temperature to state topics on timer

    mqttClient.loop();
    delay(50);
}
