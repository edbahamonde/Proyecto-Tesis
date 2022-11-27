//#include <ESP8266WiFi.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
//librerías del sensor de humedad
#include <DHT.h>
#include <DHT_U.h>
//Libería de SD
//#include <SD.h>
//librerías bmp280
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>
//librería dsb
#include <OneWire.h>
#include <DallasTemperature.h>

#define SCREEN_WIDTH 128  // OLED display width, in pixels
#define SCREEN_HEIGHT 64  // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET LED_BUILTIN  // Reset pin #
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

const char* ssid = "TP-Link_293C";
const char* password = "13904906";

//Variables sensor de hume y temp
int SENSOR = 4;
float TEMPERATURA;
float HUMEDAD;
float maxtemp = 0, maxhum = 0;
float mintemp = 100, minhum = 100;
//Potenciómetro
int valPot = 0;
//Sensor de humedad del suelo
int SENSOR_HS = 39;
int SUELO;
float maxhs = 0;
float minhs = 100;
//Sensor de lluvia
int lluviaAnalog = 35, lluviaDigital = 34, LLUVIANALOG, LLUVIADIGI, maxllu, minllu;
//Sensor de luminosidad
int SENSORLUZ = 13, LUZ;
int maxluz = 0, minluz = 500;
//Sensor de presión
float ALTITUD;
int maxalti = 0, minalti = 500;
float PRESION;
int maxpres = 0, minpres = 500;
#define BMP_SDA 21
#define BMP_SCL 22
Adafruit_BMP280 bmp280;
//Sensor de temperatura de agua
#define SENSOR_PIN 15
OneWire oneWire(SENSOR_PIN);
DallasTemperature DS18B20(&oneWire);
float tempC;  // temperature in Celsius
float tempF;  // temperature in Fahrenheit
//sensor de caudal
int NumPulsos, PinSensor = 32;
float factor_conversion = 7.5;
int frecuencia;
int medidaIntervalo= 2500;

DHT dht(SENSOR, DHT22);

//Pin de SD Card CS
#define CS_PIN 15
int contador = 0;

void ContarPulsos() {
  NumPulsos++;
}

int ObtenerFrecuencia() {  
  NumPulsos = 0;
  interrupts();
  delay(medidaIntervalo);
  noInterrupts();

  return (float)NumPulsos * 1000 / medidaIntervalo;
}

//icono de temperatura
const unsigned char temp[] PROGMEM = {
  0x00, 0x0f, 0x80, 0x00, 0x00, 0x1f, 0xc0, 0x00, 0x00, 0x3f, 0xe0, 0x00, 0x00, 0x7f, 0xf0, 0x00,
  0x07, 0xff, 0xfe, 0x00, 0x0f, 0xff, 0xff, 0x80, 0x1f, 0xff, 0xff, 0x80, 0x3f, 0xff, 0xff, 0xc0,
  0x3f, 0xff, 0xff, 0xc0, 0x3f, 0xff, 0xff, 0xc0, 0x3f, 0xff, 0xff, 0xf0, 0x7f, 0xff, 0xff, 0xf8,
  0xff, 0xff, 0xff, 0xfc, 0xff, 0xff, 0xff, 0xfc, 0xff, 0xff, 0xff, 0xfc, 0xff, 0xff, 0xff, 0xfc,
  0xff, 0xff, 0xff, 0xfc, 0x7f, 0xff, 0xff, 0xfc, 0x3f, 0xff, 0xff, 0xf8, 0x1f, 0xff, 0xff, 0xe0
};
//icono de humedad
const unsigned char hum[] PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x20, 0x00, 0x03, 0x80, 0x70,
  0x00, 0x07, 0xc0, 0xf8, 0x00, 0x07, 0xc1, 0xf8, 0x00, 0x0f, 0xe1, 0xfc, 0x00, 0x0f, 0xe1, 0xfc,
  0x00, 0x1f, 0xf1, 0xfc, 0x00, 0x1f, 0xf8, 0xf8, 0x10, 0x3f, 0xf8, 0x20, 0x38, 0x3f, 0xf8, 0x00,
  0x38, 0x7f, 0xc8, 0x00, 0x7c, 0x7f, 0xc8, 0x00, 0xfe, 0x3f, 0xd8, 0x00, 0xfa, 0x3f, 0x98, 0x00,
  0xfa, 0x3f, 0x38, 0x00, 0x7c, 0x1f, 0xf0, 0x00, 0x38, 0x07, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00
};
//icono de humedad Suelo
const unsigned char humSuelo[] PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x3f, 0xf0, 0x00, 0x00, 0xc0, 0x18, 0x00, 0x01, 0x00, 0x04, 0x00,
  0x03, 0x00, 0x02, 0x00, 0x0f, 0x80, 0x02, 0x00, 0x7c, 0xe0, 0x02, 0x00, 0x4c, 0xd0, 0x02, 0x00,
  0x58, 0x90, 0x02, 0x00, 0x71, 0x10, 0x02, 0x00, 0x7f, 0xf0, 0x02, 0x00, 0x10, 0x40, 0x3f, 0xf0,
  0x10, 0x40, 0xc0, 0x18, 0x10, 0x41, 0x80, 0x0c, 0x10, 0x41, 0x9e, 0x0c, 0x10, 0x41, 0xf3, 0x7c,
  0x10, 0x41, 0x81, 0xcc, 0x10, 0x41, 0x80, 0x8c, 0x00, 0x00, 0xe0, 0x18, 0x00, 0x00, 0x3f, 0xe0
};
//icono de lluvia
const unsigned char lluvia[] PROGMEM = {
  0x00, 0x07, 0x80, 0x00, 0x00, 0x1f, 0xe0, 0x00, 0x00, 0x38, 0x30, 0x00, 0x00, 0x60, 0x1f, 0x80,
  0x00, 0xe0, 0x01, 0xc0, 0x01, 0xc0, 0x00, 0xe0, 0x03, 0x80, 0x00, 0x60, 0x1f, 0x00, 0x00, 0x60,
  0x3a, 0x00, 0x00, 0x78, 0x60, 0x00, 0xc0, 0x18, 0xc0, 0x01, 0xc0, 0x0c, 0xc0, 0x01, 0xe0, 0x0c,
  0xc0, 0x01, 0xc0, 0x0c, 0x60, 0x08, 0x00, 0x18, 0x70, 0x1c, 0x00, 0x38, 0x3f, 0x1c, 0x11, 0xf0,
  0x0f, 0x1c, 0x19, 0xc0, 0x00, 0x00, 0x38, 0x00, 0x00, 0x00, 0x38, 0x00, 0x00, 0x00, 0x18, 0x00
};

const unsigned char nube[] PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xe0, 0x00,
  0x00, 0x0c, 0x70, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x1f, 0x80, 0x07, 0xf0, 0x07, 0xc0,
  0x0e, 0x00, 0x00, 0xe0, 0x0c, 0x00, 0x00, 0x60, 0x08, 0x00, 0x00, 0x30, 0x18, 0x00, 0x00, 0x30,
  0x0c, 0x00, 0x00, 0x60, 0x0c, 0x00, 0x00, 0x60, 0x07, 0x00, 0x00, 0xc0, 0x03, 0xfe, 0x0f, 0x80,
  0x00, 0xe3, 0x80, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
//icono de luminosidad
const unsigned char sol[] PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x23, 0x10, 0x00,
  0x00, 0x1f, 0xf0, 0x00, 0x00, 0x19, 0x30, 0x00, 0x01, 0xef, 0xfe, 0x00, 0x00, 0xdf, 0xfc, 0x00,
  0x00, 0x3f, 0xf4, 0x00, 0x01, 0xbf, 0xff, 0x00, 0x03, 0xbf, 0xff, 0x00, 0x00, 0x3f, 0xf4, 0x00,
  0x00, 0x5f, 0xfc, 0x00, 0x00, 0xff, 0xee, 0x00, 0x00, 0x13, 0xb0, 0x00, 0x00, 0x1f, 0xf0, 0x00,
  0x00, 0x33, 0x10, 0x00, 0x00, 0x21, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
const unsigned char nublado[] PROGMEM = {
  0x00, 0x10, 0x00, 0x00, 0x00, 0x30, 0x00, 0x00, 0x00, 0x30, 0x00, 0x00, 0x0c, 0x00, 0x60, 0x00,
  0x0c, 0xfd, 0xf8, 0x00, 0x01, 0xff, 0x9f, 0x00, 0x03, 0x03, 0x8f, 0xc0, 0x06, 0x01, 0x80, 0xc0,
  0x06, 0x01, 0x80, 0x60, 0x76, 0x00, 0xfc, 0x60, 0x06, 0x01, 0xce, 0xe0, 0x06, 0x0f, 0x83, 0xe0,
  0x03, 0x3f, 0x03, 0xf0, 0x01, 0xf0, 0x00, 0x18, 0x0c, 0xe0, 0x00, 0x18, 0x0c, 0x60, 0x00, 0x08,
  0x00, 0x60, 0x00, 0x18, 0x00, 0x30, 0x00, 0x38, 0x00, 0x3f, 0xff, 0xf0, 0x00, 0x0f, 0xff, 0xc0
};
const unsigned char noche[] PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x60, 0x00, 0x00, 0xc1, 0xf8, 0x00,
  0x01, 0xc1, 0xf8, 0x00, 0x03, 0xc0, 0xf0, 0x80, 0x07, 0xc0, 0xf1, 0xc0, 0x07, 0xc0, 0x10, 0xc0,
  0x07, 0xe0, 0x00, 0x40, 0x0f, 0xe0, 0x00, 0x00, 0x0f, 0xf0, 0x00, 0x00, 0x0f, 0xf8, 0x00, 0x00,
  0x0f, 0xfc, 0x00, 0x00, 0x07, 0xff, 0x80, 0x00, 0x07, 0xff, 0xe0, 0x00, 0x03, 0xff, 0xc0, 0x00,
  0x01, 0xff, 0xc0, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
//icono de presión atmosférica
const unsigned char altitud[] PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x07, 0x80, 0x00, 0x00,
  0x07, 0x80, 0x00, 0x00, 0x0f, 0xc0, 0x00, 0x00, 0x1b, 0x60, 0x38, 0x00, 0x1b, 0x60, 0x7c, 0x00,
  0x03, 0x00, 0xfe, 0x00, 0x03, 0x21, 0xc7, 0x00, 0x03, 0x73, 0x83, 0x80, 0x03, 0xff, 0x01, 0xc0,
  0x03, 0xde, 0x00, 0xe0, 0x03, 0x0c, 0x00, 0x70, 0x03, 0x00, 0x00, 0x30, 0x0f, 0x00, 0x00, 0x00,
  0x1c, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
const unsigned char presion[] PROGMEM = {
  0x08, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x08, 0x00, 0x80, 0x00, 0x0e, 0x05, 0xaf, 0x00,
  0x08, 0x05, 0xaf, 0x80, 0x08, 0x0d, 0xef, 0xc0, 0x08, 0x2d, 0xef, 0xe0, 0x08, 0x35, 0xaf, 0xe0,
  0x08, 0x15, 0xaf, 0xe0, 0x0e, 0x04, 0xab, 0xc0, 0x08, 0x04, 0x20, 0x00, 0x08, 0x0c, 0x60, 0x00,
  0x0c, 0x0c, 0x60, 0x00, 0x08, 0x04, 0x20, 0x00, 0x08, 0x0e, 0x70, 0x00, 0x08, 0x0e, 0x70, 0x00,
  0x0f, 0x84, 0x20, 0x00, 0x08, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00
};

void setup() {
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.display();

  Serial.begin(9600);

  Wire.begin();
  boolean status = bmp280.begin(0x76);

  if (!status) {
    Serial.println("no conectado bpm");
  }

  dht.begin();
  pinMode(lluviaDigital, INPUT);
  pinMode(PinSensor, INPUT);
  attachInterrupt(digitalPinToInterrupt(PinSensor), ContarPulsos, RISING);

  DS18B20.begin();

  //Verifica la SD Card
  /*if (!SD.begin(CS_PIN)) {
    Serial.println("Problemas con existencia de tarjeta SD!");
    return;
  }
  Serial.println("Tarjeta iniciada correctamente");*/

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
  display.print("C M");

  TEMPERATURA = dht.readTemperature();
  HUMEDAD = dht.readHumidity();
  SUELO = map(analogRead(SENSOR_HS), 0, 4095, 100, 0);
  LLUVIANALOG = map(analogRead(lluviaAnalog), 0, 4095, 100, 0);
  LLUVIADIGI = digitalRead(lluviaDigital);
  LUZ = analogRead(SENSORLUZ);
  ALTITUD = bmp280.readAltitude(1011.18);
  PRESION = (bmp280.readPressure() / 100);
  DS18B20.requestTemperatures();
  tempC = DS18B20.getTempCByIndex(0);
  tempF = tempC * 9 / 5 + 32;
  float frecuencia = ObtenerFrecuencia();             //obtenemos la Frecuencia de los pulsos en Hz
  float caudal_L_m = frecuencia / factor_conversion;  //calculamos el caudal en L/m
  //float caudal_L_h = caudal_L_m * 60;                 //calculamos el caudal en L/h

  if (isnan(TEMPERATURA) || isnan(HUMEDAD)) {
    display.setTextSize(1);
    display.setCursor(0, 57);
    display.print(F("Problema con DHT22"));
    display.display();
  }

  if (TEMPERATURA > maxtemp) {
    maxtemp = TEMPERATURA;
  }
  if (TEMPERATURA < mintemp) {
    mintemp = TEMPERATURA;
  }
  if (HUMEDAD > maxhum) {
    maxhum = HUMEDAD;
  }
  if (HUMEDAD < minhum) {
    minhum = HUMEDAD;
  }
  if (SUELO > maxhs) {
    maxhs = SUELO;
  }
  if (SUELO < minhs) {
    minhs = SUELO;
  }
  if (LLUVIANALOG > maxllu) {
    maxllu = LLUVIANALOG;
  }
  if (LLUVIANALOG < minllu) {
    minllu = LLUVIANALOG;
  }
  if (LUZ > maxluz) {
    maxluz = LUZ;
  }
  if (LUZ < minluz) {
    minluz = LUZ;
  }
  if (ALTITUD > maxalti) {
    maxalti = ALTITUD;
  }
  if (ALTITUD < minalti) {
    minalti = ALTITUD;
  }
  if (PRESION > maxpres) {
    maxpres = PRESION;
  }
  if (PRESION < minpres) {
    minpres = PRESION;
  }

  valPot = map(analogRead(36), 0, 4095, 0, 6);

  display.setCursor(84, 0);
  display.print("Op: ");
  display.setCursor(106, 0);
  display.print(valPot);

  if (valPot == 0) {
    temperatura();
  }
  if (valPot == 1) {
    humedad();
  }
  if (valPot == 2) {
    humeSuelo();
  }
  if (valPot == 3) {
    lluviaS();
  }
  if (valPot == 4) {
    luzS();
  }
  if (valPot == 5) {
    altitudS();
  }
  if (valPot == 6) {
    presionS();
  }
  /*if (valPot == 6) { !!!!!!!FALTA PONER EL DEL AGUA!!!_------
    presionS();
  }*/
  /*if (valPot == 2) {
    //-----------TARJETA SD-----------
    File dataFile = SD.open("Llog.txt", FILE_WRITE);
    if (dataFile) {
      Serial.println("Archivo abierto correctamente");
      dataFile.print(contador);
      dataFile.print(",");
      dataFile.print(TEMPERATURA);
      dataFile.print(",");
      dataFile.print(HUMEDAD);
      dataFile.close();
      contador = contador + 1;
    } else {
      Serial.println("Falla al abrir el archivo");
    }
    delay(2000);
  }*/

  Serial.print(" ");
  Serial.print(contador);
  Serial.print(" ::");
  Serial.print("Temperatura: ");
  Serial.print(TEMPERATURA);
  Serial.print("\tHumedad: ");
  Serial.print(HUMEDAD);
  Serial.print("\tHumedad Suelo: ");
  Serial.print(SUELO);
  Serial.print("\tLluvia D: ");
  Serial.print(LLUVIADIGI);
  Serial.print("\tLluvia A: ");
  Serial.print(LLUVIANALOG);
  Serial.print("\tLuz: ");
  Serial.print(LUZ);
  Serial.print("\tAltitud: ");
  Serial.print(ALTITUD);
  Serial.print("\tPresion: ");
  Serial.print(PRESION);
  Serial.print("\tTemp Agua: ");
  Serial.print(tempC);
  Serial.print("FrecuenciaPulsos: ");
  Serial.print(frecuencia, 0);
  Serial.print(" Hz\tCaudal: ");
  Serial.print(caudal_L_m, 3);
  Serial.print(" L/min");
  //Serial.print(caudal_L_h, 3);
  //Serial.println("L/h");
  Serial.print("\tPotenciometro: ");
  Serial.print(valPot);
  Serial.println();

  delay(2000);
}

void temperatura() {

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
  display.print("max:");
  display.print(maxtemp, 1);
  display.setCursor(75, 41);
  display.print("min:");
  display.print(mintemp, 1);

  display.display();
}

void humedad() {

  display.setCursor(0, 11);
  display.drawBitmap(15, 11, hum, 30, 20, WHITE);

  display.setTextSize(3);
  display.setCursor(0, 35);
  display.print(HUMEDAD, 0);
  if (HUMEDAD == 100) {
    display.setCursor(55, 35);
  } else {
    display.setCursor(37, 35);
  }
  display.setTextSize(2);
  display.print("%");

  display.setTextSize(1);
  display.setCursor(62, 12);
  display.print("Humedad");

  display.setTextSize(1);
  display.setCursor(75, 26);
  display.print("max:");
  display.print(maxhum, 1);
  display.setCursor(75, 41);
  display.print("min:");
  display.print(minhum, 1);

  display.display();
}

void humeSuelo() {
  display.setCursor(0, 11);
  display.drawBitmap(10, 11, humSuelo, 30, 20, WHITE);

  display.setTextSize(3);
  display.setCursor(0, 35);
  display.print(SUELO);
  if (SUELO == 100) {
    display.setCursor(55, 35);
  } else {
    display.setCursor(37, 35);
  }
  display.setTextSize(2);
  display.print("%");

  display.setTextSize(1);
  display.setCursor(50, 12);
  display.print("Humedad Suelo");

  display.setTextSize(1);
  display.setCursor(75, 26);
  display.print("max:");
  display.print(maxhs, 0);
  display.setCursor(75, 41);
  display.print("min:");
  display.print(minhs, 0);

  display.display();
}

void lluviaS() {
  display.setCursor(0, 11);
  if (LLUVIADIGI == 0) {
    display.drawBitmap(15, 11, lluvia, 30, 20, WHITE);
  } else {
    display.drawBitmap(15, 11, nube, 30, 20, WHITE);
  }

  display.setTextSize(3);
  display.setCursor(0, 35);
  display.print(LLUVIANALOG, 0);
  if (LLUVIANALOG == 100) {
    display.setCursor(55, 35);
  } else {
    display.setCursor(37, 35);
  }
  display.setTextSize(2);
  //para la unidad de lluviua
  display.print("");

  display.setTextSize(1);
  display.setCursor(55, 12);
  display.print("Precipitacion");

  display.setTextSize(1);
  display.setCursor(75, 26);
  display.print("max:");
  display.print(maxllu, 1);
  display.setCursor(75, 41);
  display.print("min:");
  display.print(minllu, 1);

  display.display();
}

void luzS() {
  display.setCursor(0, 11);
  if (LUZ < 112) {
    display.drawBitmap(10, 11, noche, 30, 20, WHITE);
  } else if (LUZ >= 350) {
    display.drawBitmap(15, 11, sol, 30, 20, WHITE);
  } else {
    display.drawBitmap(15, 11, nublado, 30, 20, WHITE);
  }

  display.setTextSize(3);
  display.setCursor(0, 35);
  display.print(LUZ, 0);

  display.setTextSize(1);
  display.setCursor(55, 12);
  display.print("Luminosidad");

  display.setTextSize(1);
  display.setCursor(75, 26);
  display.print("max:");
  display.print(maxluz, 1);
  display.setCursor(75, 41);
  display.print("min:");
  display.print(minluz, 1);

  display.display();
}

void altitudS() {
  display.setCursor(0, 11);
  display.drawBitmap(10, 11, altitud, 30, 20, WHITE);

  display.setTextSize(3);
  display.setCursor(0, 35);
  display.print(ALTITUD, 0);
  display.setCursor(55, 35);
  display.setTextSize(2);
  display.print("m");

  display.setTextSize(1);
  display.setCursor(55, 12);
  display.print("Altitud");

  display.setTextSize(1);
  display.setCursor(75, 26);
  display.print("max:");
  display.print(maxalti, 1);
  display.setCursor(75, 41);
  display.print("min:");
  display.print(minalti, 1);

  display.display();
}

void presionS() {
  display.setCursor(0, 11);
  display.drawBitmap(10, 11, presion, 30, 20, WHITE);

  display.setTextSize(3);
  display.setCursor(0, 35);
  display.print(PRESION, 0);
  display.setCursor(55, 35);
  display.setTextSize(1);
  display.print("mb");

  display.setTextSize(1);
  display.setCursor(55, 12);
  display.print("Presion");

  display.setTextSize(1);
  display.setCursor(75, 26);
  display.print("max:");
  display.print(maxpres, 1);
  display.setCursor(75, 41);
  display.print("min:");
  display.print(minpres, 1);

  display.display();
}

