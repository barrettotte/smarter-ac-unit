#include <ESP8266WiFi.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <PubSubClient.h>

#include <config.h>

#define MQTT_MAX_PACKET_SIZE 2048 // redefined from 256 in PubSubClient
// Note: this causes a compiler warning, but I couldn't get anything else to work.

void mqttCallback(char* topic, byte* payload, unsigned int payloadLength);
void mqttReconnect();
void mqttDiscover();
void mqttPublishState();

struct EzTimer {
    bool trigger;
    unsigned long currentMs;
    unsigned long prevMs;
};

struct AcState {
    uint8_t mode;  // 0=off, 1=cool
    uint8_t speed; // 0=low, 1=high
    uint8_t temperature;
};

/** globals ***/

WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);
IRsend irSend(IR_LED_PIN);

struct AcState state;

struct EzTimer statePublishTimer;
struct EzTimer irFeedbackTimer;
struct EzTimer manualSyncTimer;

/*** init ***/

void initSerial() {
    Serial.begin(115200);
    Serial.println();

    for (uint8_t t = 3; t > 0; t--) {
        Serial.printf("WAIT %d...\n", t);
        Serial.flush();
        delay(500);
    }
    Serial.println("\n* * * START * * *");
}

void initWifi() {
    WiFi.mode(WIFI_STA); // esp defaults to STA+AP
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    Serial.print("Connecting to WiFi");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.print("\nIP address: ");
    Serial.println(WiFi.localIP());
}

void initMqtt() {
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
}

/*** infrared ***/

// encode state to infrared code
void buildIrCode(uint8_t* code) {
    // 0x830601A20000C000000000000063001100803800A9 (168 bits)
    // uint8_t data[21] = {
    //     0x83, 0x06, 0x01, 0xA2, 0x00, 0x00, 0xC0, 0x00, 
    //     0x00, 0x00, 0x00, 0x00, 0x00, 0x63, 0x00, 0x11,
    //     0x00, 0x80, 0x38, 0x00, 0xA9
    // };
    // code[0] = 0xDE;
    // code[1] = 0xAD;
    // code[2] = 0xBE;
    // code[3] = 0xEF;

    
}

// send infrared code to AC unit
void sendIrCode() {
    uint8_t code[IR_PAYLOAD_SIZE];
    memset(code, 0, IR_PAYLOAD_SIZE);
    buildIrCode(code);

    Serial.print("Sending IR code -> 0x");
    for (uint8_t i = 0; i < IR_PAYLOAD_SIZE; i++) {
        Serial.printf("%02X", code[i]);
    }
    Serial.println();
    // irSend.sendWhirlpoolAC(data, 21);
}

/*** AC state change ***/

void printState() {
    Serial.printf("State: mode=%d,speed=%d,temperature=%d\n", state.mode, state.speed, state.temperature);
}

// manually sync state with both Home Assistant and AC unit
void manualSync() {
    Serial.println("Manually syncing state...");
    printState();
    sendIrCode();
    mqttPublishState();
}

// set AC mode and fire IR signal on state change
void setAcMode(char* message) {
    uint8_t prevMode = state.mode;

    if (strcmp(message, "off") == 0) {
        state.mode = 0;
    } else if (strcmp(message, "on") == 0 || strcmp(message, "cool")) {
        state.mode = 1;
    } else {
        Serial.printf("Warning: Invalid mode '%s'. Mode not set\n", message);
        return;
    }

    // only send IR code on state change
    if (prevMode != state.mode) {
        Serial.printf("Set mode to %d\n", state.mode);
        printState();
        sendIrCode();
    }
}

// set AC temperature and fire IR signal on state change
void setAcTemperature(char* message) {
    uint8_t prevTemp = state.temperature;
    int t = atoi(message); // note: randomly comes in as float sometimes, but this still works

    if (t > AC_TEMP_MAX) {
        Serial.printf("Warning: Temperature too high %d. Clamped to %d", t, AC_TEMP_MAX);
        t = AC_TEMP_MAX;
    } else if (t < AC_TEMP_MIN) {
        Serial.printf("Warning: Temperature too low %d. Clamped to %d", t, AC_TEMP_MIN);
        t = AC_TEMP_MIN;
    }
    state.temperature = t;

    // only send IR code on state change
    if (prevTemp != state.temperature) {
        Serial.printf("Set temperature to %d\n", state.temperature);
        printState();
        sendIrCode();
    }
}

// set AC fan speed and fire IR signal on state change
void setAcFanSpeed(char* message) {
    uint8_t prevSpeed = state.speed;

    if (strcmp(message, "low") == 0) {
        state.speed = 0;
    } else if (strcmp(message, "high") == 0) {
        state.speed = 1;
    } else {
        Serial.printf("Warning: Invalid fan speed '%s'. Fan speed not set\n", message);
        return;
    }

    // only send IR code on state change
    if (prevSpeed != state.speed) {
        Serial.printf("Set fan speed to %d\n", state.speed);
        printState();
        sendIrCode();
    }
}

/*** mqtt ***/

// handle message from subscribed MQTT topics that change state - smarter_ac_unit/*/set
void mqttCallback(char* topic, byte* payload, unsigned int payloadLength) {    

    Serial.printf("Message arrived on topic %s, message: ", topic);
    for (unsigned int i = 0; i < payloadLength; i++) {
        Serial.print((char) payload[i]);
    }
    Serial.println();

    if (payloadLength > MQTT_MAX_COMMAND_SIZE) {
        Serial.printf("Error: Command message size too large; %d > %d\n", payloadLength, MQTT_MAX_COMMAND_SIZE);
        return;
    }

    char message[payloadLength + 1];
    memcpy(message, payload, payloadLength);
    message[payloadLength] = '\0';

    if (strcmp(topic, MQTT_TOPIC_MODE_COMMAND) == 0) {
        setAcMode(message);
    } else if (strcmp(topic, MQTT_TOPIC_TEMP_COMMAND) == 0) {
        setAcTemperature(message);
    } else if (strcmp(topic, MQTT_TOPIC_FAN_COMMAND) == 0) {
        setAcFanSpeed(message);
    } else {
        Serial.printf("Warning: Listening to unsupported topic %s. State unchanged\n", topic);
        return;
    }
}

// reconnect to MQTT broker and subscribe to command topics
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

// publish discovery message for Home Assistant
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

// publish AC state values to their respective MQTT topics
void mqttPublishState() {
    Serial.printf("Publishing state -> ");
    printState();

    char stateBuffer[8];
    sprintf(stateBuffer, "%s", state.speed ? "high" : "low");
    mqttClient.publish(MQTT_TOPIC_FAN_STATE, stateBuffer, true);

    memset(stateBuffer, 0, 8);
    sprintf(stateBuffer, "%s", state.mode ? "on" : "off");
    mqttClient.publish(MQTT_TOPIC_MODE_COMMAND, stateBuffer, true);

    memset(stateBuffer, 0, 8);
    sprintf(stateBuffer, "%d", state.temperature);
    mqttClient.publish(MQTT_TOPIC_TEMP_COMMAND, stateBuffer, true);
}

/*** main ***/

void setup() {
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, HIGH);
    pinMode(IR_FEED_LED_PIN, OUTPUT);
    pinMode(MANUAL_SYNC_PIN, INPUT_PULLUP);

    initSerial();
    initWifi();
    irSend.begin();
    initMqtt();

    // init state
    state.mode = 0;
    state.speed = 0;
    state.temperature = AC_TEMP_DEFAULT;

    digitalWrite(LED_BUILTIN, LOW);
    Serial.println("Setup done.");
}

void loop() {
    if (!mqttClient.connected()) {
        mqttReconnect(); // will block on failed connections, but probably fine for this
    }

    // reset IR feedback LED if active and enough time passed
    irFeedbackTimer.currentMs = millis();
    if ((irFeedbackTimer.currentMs - irFeedbackTimer.prevMs) > IR_FEEDBACK_MS) {
        irFeedbackTimer.prevMs = irFeedbackTimer.currentMs;
        irFeedbackTimer.trigger = false;
        digitalWrite(IR_FEED_LED_PIN, LOW);
    }

    // handle debounced button for manual sync
    manualSyncTimer.currentMs = millis();
    if (((manualSyncTimer.currentMs - manualSyncTimer.prevMs) > MANUAL_SYNC_DEBOUNCE_MS) && digitalRead(MANUAL_SYNC_PIN) == LOW) {
        manualSyncTimer.prevMs = manualSyncTimer.currentMs;

        if (!irFeedbackTimer.trigger) {
            irFeedbackTimer.trigger = true;
            digitalWrite(IR_FEED_LED_PIN, HIGH);
        }
        manualSync();
    }

    // handle periodic state publish to home assistant
    statePublishTimer.currentMs = millis();
    if ((statePublishTimer.currentMs - statePublishTimer.prevMs) > MQTT_PUBLISH_STATE_MS) {
        statePublishTimer.prevMs = statePublishTimer.currentMs;
        mqttPublishState();
    }
    mqttClient.loop();
    delay(100);
}
