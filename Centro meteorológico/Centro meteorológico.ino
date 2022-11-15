#include <ESP8266WiFi.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
//librerías del sensor de humedad
#include <DHT.h>
#include <DHT_U.h>

#include <U8g2lib.h>

#define SCREEN_WIDTH 128  // OLED display width, in pixels
#define SCREEN_HEIGHT 64  // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET LED_BUILTIN  // Reset pin #
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

const char* ssid = "TP-Link_293C";
const char* password = "13904906";

//Variables sensor de hume y temp
int SENSOR = 2;
float TEMPERATURA;
float HUMEDAD;

DHT dht(SENSOR, DHT22);

void setup() {
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.display();

  Serial.begin(9600);
  
  dht.begin();

  //Titulo del circuito
  /*display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.print("Centro Meteorologico");*/
  display.display();
  delay(1000);
}

void loop() {
  
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.print("Centro Meteorologico");

  TEMPERATURA = dht.readTemperature();
  HUMEDAD = dht.readHumidity();
  Serial.print("Temperatura: "); Serial.print(TEMPERATURA);
  Serial.print("\tHumedad: "); Serial.print(HUMEDAD);
  Serial.println();

  // Clear the buffer.
  
  display.setTextSize(3);
  //display.setTextColor(WHITE);
  display.setCursor(0, 15);
  //display.print("T: ");
  display.print(TEMPERATURA, 0);
  display.setCursor(64, 15);
  display.setTextSize(2);
  display.print("C");
  display.setTextSize(3);
  display.setCursor(0, 30);
  //display.print("H: ");
  /*display.print(HUMEDAD, 0);
  display.setCursor(64, 30);
  display.setTextSize(2);
  display.print("%");*/
  display.display();
  delay(2000);
}