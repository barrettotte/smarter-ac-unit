// simple IR sniffer that outputs codes to SD1306 OLED

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>

Adafruit_SSD1306 display(128, 64, &Wire, -1);

void setup() {
    Serial.begin(9600);
    while(!Serial) {}
    Serial.println("\n* * * START * * *");

    Serial.println("Initializing display...");
    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
        Serial.println("SSD1306 allocation failed.");
        while(1) {}
    }
    delay(1000);

    display.clearDisplay();
    display.setTextColor(WHITE);
    display.setCursor(0, 16);
    display.println("Hello world");
    display.display();

    delay(500);
    Serial.println("Done Setup.");
}

void loop() {
    Serial.print(".");
    // TODO: check and display infrared
    delay(500);
}
