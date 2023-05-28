#include <Arduino.h>

void setup() {
    Serial.begin(9600);
    Serial.println();

    for (uint8_t t = 3; t > 0; t--) {
        Serial.printf("WAIT %d...\n", t);
        Serial.flush();
        delay(500);
    }
    Serial.println("\n* * * START * * *");

    pinMode(LED_BUILTIN, OUTPUT);

    Serial.println("\nSetup done.");
}

void loop() {
    digitalWrite(LED_BUILTIN, 1);
    delay(1000);
    digitalWrite(LED_BUILTIN, 0);
    delay(1000);
}
