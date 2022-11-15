#include <ESP8266WiFi.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
//librerías del sensor de humedad
#include <DHT.h>
#include <DHT_U.h>

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
float maxtemp = 0;
float mintemp = 100;

DHT dht(SENSOR, DHT22);

//icono de temperatura
const unsigned char temp [] PROGMEM = {
	0x00, 0x0f, 0x80, 0x00, 0x00, 0x1f, 0xc0, 0x00, 0x00, 0x3f, 0xe0, 0x00, 0x00, 0x7f, 0xf0, 0x00, 
	0x07, 0xff, 0xfe, 0x00, 0x0f, 0xff, 0xff, 0x80, 0x1f, 0xff, 0xff, 0x80, 0x3f, 0xff, 0xff, 0xc0, 
	0x3f, 0xff, 0xff, 0xc0, 0x3f, 0xff, 0xff, 0xc0, 0x3f, 0xff, 0xff, 0xf0, 0x7f, 0xff, 0xff, 0xf8, 
	0xff, 0xff, 0xff, 0xfc, 0xff, 0xff, 0xff, 0xfc, 0xff, 0xff, 0xff, 0xfc, 0xff, 0xff, 0xff, 0xfc, 
	0xff, 0xff, 0xff, 0xfc, 0x7f, 0xff, 0xff, 0xfc, 0x3f, 0xff, 0xff, 0xf8, 0x1f, 0xff, 0xff, 0xe0
};


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
  display.setCursor(0, 0);
  display.print("Centro Meteorologico");

  TEMPERATURA = dht.readTemperature();
  HUMEDAD = dht.readHumidity();

  if (isnan(TEMPERATURA) || isnan(HUMEDAD)) {
    display.setTextSize(1);
    display.setCursor(0, 57);
    display.print(F("Problema al leer DHT22"));
    display.display();
  }

  if(TEMPERATURA > maxtemp){
    maxtemp = TEMPERATURA;
  }
  if(TEMPERATURA < mintemp){
    mintemp = TEMPERATURA;
  }

  Serial.print("Temperatura: ");
  Serial.print(TEMPERATURA);
  Serial.print("\tHumedad: ");
  Serial.print(HUMEDAD);
  Serial.println();

  /*u8g2.setFontDirection(0);
  u8g2.setFont(u8g2_font_inb24_mf);
  u8g2.drawStr(0, 15, "T"); --> No funcionó para mostrar letra mas grande*/

  /*display.setTextSize(3);
  display.setCursor(0, 11);
  display.print(TEMPERATURA, 0);
  display.setCursor(37, 11);
  display.setTextSize(2);
  display.print((char)247);
  display.print("C");*/

  display.setCursor(0, 11);
  display.drawBitmap(15, 11, temp, 30, 20, WHITE);

  display.setTextSize(3);
  display.setCursor(0, 35);  
  display.print(TEMPERATURA, 0);
  display.setCursor(37, 35);
  display.setTextSize(2);
  display.print((char)247);
  display.print("C");

  display.setTextSize(1);
  display.setCursor(62, 12);  
  display.print("Temperatura");

  display.setTextSize(1);
  display.setCursor(75, 26);  
  display.print("max:");display.print(maxtemp,1);
  display.setCursor(75, 41);  
  display.print("min:");display.print(mintemp,1);

  display.display();
  delay(2000);
}