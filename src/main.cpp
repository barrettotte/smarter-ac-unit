#include <WiFiManager.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>

#define WIFI_TRIGGER_PIN D1   // pin to trigger wifi config portal
#define IR_LED_PIN D2         // IR LED transmit pin
#define WIFI_PORTAL_SECS 300  // max wifi portal time

/** globals ***/

WiFiManager wifiManager;
IRsend irSend(IR_LED_PIN);

/*** utils ***/

void handleWifiReconfigure() {
    if (!wifiManager.startConfigPortal("smarter-ac-unit", "0xDEADBEEF")) {
        Serial.println("Failed to connect to WiFi.");
        delay(3000);
        ESP.restart();
        delay(5000);
    }
    Serial.println("Connected to WiFi.");
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
    wifiManager.setConfigPortalTimeout(WIFI_PORTAL_SECS);

    // init infrared LED
    irSend.begin();

    Serial.println("\nSetup done.");
}

// 0x830601A20000C000000000000063001100803800A9 (168 bits)
uint8_t data[21] = {
    0x83, 0x06, 0x01, 0xA2, 0x00, 0x00, 0xC0, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x63, 0x00, 0x11,
    0x00, 0x80, 0x38, 0x00, 0xA9
};

void loop() {
    if (digitalRead(WIFI_TRIGGER_PIN) == LOW) {
        handleWifiReconfigure();
    }

    irSend.sendWhirlpoolAC(data, 21);
    Serial.println("Sent infrared signal");
    delay(2000);
}
