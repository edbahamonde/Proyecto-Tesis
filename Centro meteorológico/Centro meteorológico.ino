#include <ESP8266WiFi.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET LED_BUILTIN // Reset pin #
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

const char* ssid = "TP-Link_293C";
const char* password = "13904906";

void setup() {
  
  display.clearDisplay();
  display.display();
  Serial.begin(9600);

  display.setCursor(0,0);
  display.setTextSize(2);
  display.print("Centro Meteorológico");
  delay(1000);

}

void loop() {
  // put your main code here, to run repeatedly:

}
