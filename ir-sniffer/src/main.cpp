// quick and dirty IR sniffer that outputs IR codes to OLED

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Arduino.h>
// #include <IRremote.h>
#include <SPI.h>
#include <Wire.h>

Adafruit_SSD1306 display(128, 64, &Wire, -1); // SDA=A4,SCL=A5

void setup() {
    // init serial
    Serial.begin(9600);
    while(!Serial) {}
    Serial.println("\n* * * START * * *");

    // init display
    Serial.println("Initializing display...");
    // if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    //     Serial.println("SSD1306 allocation failed.");
    //     while(1) {}
    // }
    // delay(1000);
    display.clearDisplay();
    display.setTextColor(WHITE);
    display.setCursor(0, 16);
    display.println("Hello world");
    display.display();

    // init IR
    // IrReceiver.begin(8, ENABLE_LED_FEEDBACK); // D6

    delay(500);
    Serial.println("Done Setup.");
}

void loop() {
    // if (IrReceiver.decode()) {
    //     Serial.println(IrReceiver.decodedIRData.decodedRawData, HEX);

    //     display.clearDisplay();
    //     display.setCursor(0, 16);
    //     display.println(IrReceiver.decodedIRData.decodedRawData, HEX);
    //     display.display();
    //     IrReceiver.resume();
    // }
    delay(500);
}
