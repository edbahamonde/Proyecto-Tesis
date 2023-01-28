#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
//librerías del sensor de humedad
#include <DHT.h>
#include <DHT_U.h>
//Libería de SD
#include <SD.h>
//librerías bmp280
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>
//librería dsb
#include <OneWire.h>
#include <DallasTemperature.h>
// librería para el menú
#include <OneButton.h>
//Librerías para el envío a Firebase
#include <WiFi.h>
#include <Firebase_ESP_Client.h>
#include "time.h"
#include "addons/TokenHelper.h"  //--> Generación de tokens
#include "addons/RTDBHelper.h"   //--> Impresión de funciones auxiliares
//Librería para el envío a Google Sheets
#include <HTTPClient.h>

#define SCREEN_WIDTH 128  // OLED display width, in pixels
#define SCREEN_HEIGHT 64  // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET LED_BUILTIN  // Reset pin #
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

//Variables sensor de hume y temp
int SENSORT_H = 4;
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
int lluviaAnalog = 34, LLUVIANALOG, maxllu, minllu = 100;
//Sensor de luminosidad
int SENSOR_LUZ = 35, LUZ, maxluz = 0, minluz = 500;
//Sensor de viento
int SENSOR_ANE = 25, ANEMOMETRO, maxane = 0, minane = 500, entrada;
long int contadore;
bool paso = 0;
//Sensor de presión
float ALTITUD;
int maxalti = 0, minalti = 500;
float PRESION;
float maxpres = 0, minpres = 500;
#define BMP_SDA 21
#define BMP_SCL 22
Adafruit_BMP280 bmp280;
int TEMP_BPM, mintemp_bpm = 500, maxtemp_bpm = 0;
//Sensor de temperatura de agua
#define SENSOR_PIN 15
OneWire oneWire(SENSOR_PIN);
DallasTemperature DS18B20(&oneWire);
float TEMP_AGUA;  // temperature in Celsius
float mintemp_agua = 500, maxtemp_agua = 0;
float tempF;  // temperature in Fahrenheit
//sensor de caudal
int NumPulsos, CAUDAL = 32;
float factor_conversion = 7.5, caudal_L_m;
int frecuencia, mincaudal = 500, maxcaudal = 0, medidaIntervalo = 2500;
//sensores de luminosidad
int SENSOR1_LUZ = 33, SENSOR2_LUZ = 25, minluz1_reflejada = 5, maxluz1_reflejada = 0;
int LUZ_DIRECTA, mindirecta = 30, maxdirecta = 0;
//Sdk
#define CS_PIN 5
String registro;
bool sd_iniciada;
String nombreArchivo = "/dataESP32_G.txt";
int contador = 0, linea = 0;
byte caracter;
int cabeceraCreada = 0;
//para el tiempo en ejecución -> para el guardado de datos
int countTime = 0;
//MENU
int pic = 0;
int maxPics_L1 = 4;
int maxPics_L2 = 2;
int button_brd = 14;
long lastmillis = 0;
long maxtime = 30000;
String tipoModalidad = "";
//Envío a Firebase
#define WIFI_SSID "des"
#define WIFI_PASSWORD "desdesdes"
//#define WIFI_SSID "GALBATOR"
//#define WIFI_PASSWORD "1753632718-001@."
//#define WIFI_SSID "TP-Link_293C"
//#define WIFI_PASSWORD "13904906"

#define API_KEY "AIzaSyASpyvFug26lUAYfnODjtJLw0Jg8iLZ7og"
//#define API_KEY "AIzaSyB9bpLXN8fZQC3F4st2ClGuMXtUkL5kDkY"
#define USER_EMAIL "edbahamonde@espe.edu.ec"
#define USER_PASSWORD "172596dD"
#define DATABASE_URL "https://esp-firebase-demo-78fab-default-rtdb.firebaseio.com"
//#define DATABASE_URL "https://esp-cm-default-rtdb.firebaseio.com"
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;
String uid;
String databasePath;
String timePath = "/timestamp";
String tempPath = "/Temperatura";
String humPath = "/Humedad";
String suePath = "/TempSuelo";
String lluPath = "/Lluvia";
String luzPath = "/Luz";
String altiPath = "/Altitud";
String presPath = "/Presion";
String tempAPath = "/TempAgua";
String luz_direcPath = "/LuzDirecta";
String caudalPath = "/Caudal";
String anePath = "/Anemometro";
String parentPath;
int timestamp;
FirebaseJson json;
unsigned long sendDataPrevMillis = 0;
unsigned long timerDelay = 30000;  // 30000 = 30seg  // 60000 = 1 min
int comprueba = 0;
int timezone = 18000;  //-18000 --> Se pone positivo porque luego se suma y es incorrecto
int countWi = 0;
//Google Sheets
String GOOGLE_SCRIPT_ID = "AKfycbyD_AnwXW6JycjCNBVm265IaQznudTUHK5tJg55LnnMueZmxzuVGXs566DnSJOsCmCZ";  // change Gscript ID
//Módulo GSM
void init_gsm();
void gprs_connect();
boolean gprs_disconnect();
boolean is_gprs_connected();
void post_to_sheets();
boolean waitResponse(String expected_answer = "OK", unsigned int timeout = 2000);
const String APN = "internet.cnt.net.ec";
const String USER = "";
const String PASS = "";
#define DELAY_MS 500

OneButton button(button_brd, true);

DHT dht(SENSORT_H, DHT22);

File sd_file;

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
//icono de temperatura del agua
const unsigned char tempagua[] PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x30, 0x30, 0x00, 0x00, 0x30, 0x3b, 0xc7, 0x87, 0x70,
  0x1f, 0xff, 0xff, 0xe0, 0x1f, 0xff, 0xff, 0xe0, 0x1c, 0x10, 0x20, 0xe0, 0x18, 0x00, 0x00, 0x60,
  0x18, 0x1c, 0xe0, 0x60, 0x1c, 0x1c, 0xe0, 0xe0, 0x0c, 0x0c, 0xe0, 0xe0, 0x0c, 0x00, 0x00, 0xc0,
  0x0c, 0x1c, 0xe0, 0xc0, 0x0c, 0x1c, 0xe0, 0xc0, 0x0c, 0x0c, 0xc0, 0xc0, 0x0e, 0x00, 0x01, 0xc0,
  0x0e, 0x00, 0x01, 0xc0, 0x07, 0xff, 0xff, 0xc0, 0x07, 0xff, 0xff, 0x80, 0x00, 0x00, 0x00, 0x00
};
//icono caudal de agua
const unsigned char iconCaudal[] PROGMEM = {
  0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00, 0x07, 0x80, 0x00, 0x00, 0x03, 0xc0,
  0x00, 0x00, 0xff, 0xc0, 0x00, 0x01, 0xff, 0xc0, 0x00, 0x01, 0xc3, 0x80, 0x00, 0x03, 0x87, 0x00,
  0x00, 0x03, 0x06, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x01, 0xc3, 0x00, 0x00,
  0x07, 0xf3, 0x00, 0x00, 0x0f, 0xf3, 0x00, 0x00, 0x0f, 0xff, 0x00, 0x00, 0x0f, 0xfe, 0x00, 0x00,
  0x0f, 0xf8, 0x00, 0x00, 0x0f, 0xf0, 0x00, 0x00, 0x07, 0xf0, 0x00, 0x00, 0x03, 0xe0, 0x00, 0x00
};
//icono del anemómetro
const unsigned char ventilador[] PROGMEM = {
  0x00, 0x07, 0xe0, 0x00, 0x00, 0x0f, 0xf8, 0x00, 0x00, 0x1f, 0xf8, 0x00, 0x00, 0x1f, 0xf8, 0x00,
  0x00, 0x1f, 0xf0, 0x00, 0x00, 0x1f, 0xc0, 0x00, 0x3c, 0x1f, 0x80, 0x00, 0x7e, 0x0f, 0x87, 0xe0,
  0xff, 0x9f, 0xff, 0xf8, 0xff, 0xfc, 0xff, 0xfc, 0xff, 0xfc, 0xff, 0xfc, 0x7f, 0xff, 0xe7, 0xfc,
  0x1f, 0xc7, 0xc1, 0xf8, 0x00, 0x07, 0xe0, 0xf8, 0x00, 0x0f, 0xe0, 0x00, 0x00, 0x3f, 0xe0, 0x00,
  0x00, 0x7f, 0xe0, 0x00, 0x00, 0x7f, 0xe0, 0x00, 0x00, 0x7f, 0xc0, 0x00, 0x00, 0x1f, 0x80, 0x00
};
//iconos header
const unsigned char sdicon[] PROGMEM = {
  0x3f, 0x80, 0x6a, 0x80, 0x7f, 0x80, 0xff, 0x80, 0x89, 0x80, 0xba, 0x80, 0xea, 0x80, 0x89, 0x80,
  0xff, 0x80
};
const unsigned char wificon[] PROGMEM = {
  0x00, 0x00, 0xff, 0x80, 0x00, 0x00, 0x7f, 0x00, 0x00, 0x00, 0x3e, 0x00, 0x00, 0x00, 0x1c, 0x00,
  0x08, 0x00
};
const unsigned char gsmicon[] PROGMEM = {
  0x01, 0x80, 0x01, 0x80, 0x01, 0x80, 0x0d, 0x80, 0x0d, 0x80, 0x2d, 0x80, 0x6d, 0x80, 0xed, 0x80,
  0xed, 0x80
};
const unsigned char bateriaicon[] PROGMEM = {
  0xff, 0xf0, 0x80, 0x10, 0xb6, 0xd8, 0xb6, 0xd8, 0xb6, 0xd8, 0xb6, 0xd8, 0xb6, 0xd8, 0x80, 0x10,
  0xff, 0xf0
};
const unsigned char bateriaicon2[] PROGMEM = {
  0xff, 0xf0, 0x80, 0x10, 0xb6, 0x18, 0xb6, 0x18, 0xb6, 0x18, 0xb6, 0x18, 0xb6, 0x18, 0x80, 0x10,
  0xff, 0xf0
};
const unsigned char bateriaicon3[] PROGMEM = {
  0xff, 0xf0, 0x80, 0x10, 0xb0, 0x18, 0xb0, 0x18, 0xb0, 0x18, 0xb0, 0x18, 0xb0, 0x18, 0x80, 0x10,
  0xff, 0xf0
};
const unsigned char bateriaicon4[] PROGMEM = {
  0xff, 0xf0, 0x80, 0x10, 0x80, 0x18, 0x80, 0x18, 0x80, 0x18, 0x80, 0x18, 0x80, 0x18, 0x80, 0x10,
  0xff, 0xf0
};


void setup() {
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();

  Serial.begin(9600);
  Serial2.begin(9600);

  Wire.begin();
  boolean status = bmp280.begin(0x76);

  if (SD.begin()) {
    Serial.println("Se inició la tarjeta SD");
    sd_iniciada = true;
  } else {
    Serial.println("No se pudo iniciar la tarjeta SD");
    sd_iniciada = false;
    //return; --> Hace que el programa no funcione correctamente
  }

  /*if (!status) {
    Serial.println("no conectado bpm");
  }*/

  dht.begin();
  pinMode(CAUDAL, INPUT);
  pinMode(SENSOR_ANE, INPUT);
  attachInterrupt(digitalPinToInterrupt(CAUDAL), ContarPulsos, RISING);
  attachInterrupt(digitalPinToInterrupt(SENSOR_ANE), ContarGiros, RISING);

  DS18B20.begin();

  button.attachClick(click);
  button.attachDoubleClick(doubleclick);
  button.attachLongPressStop(longPressStop);
  button.attachLongPressStart(longPressStart);

  init_gsm();

  configTime(timezone, 0, "south-america.pool.ntp.org");
}

void loop() {

  button.tick();

  if (millis() >= (lastmillis + maxtime) && pic != 116) {
    pic = 0;
  }

  Serial.println(pic);

  //Capa 1
  if (pic == 0) {
    //Para salir de la pantalla
    refresh();
    while (valPot != 9) {
      header();
      display.setTextSize(1);
      display.setCursor(0, 57);
      display.print(F("Calibrar"));
      iniciaVariables();
      mostrarTodo();
      refresh();
    }
    pic = 1;
  }
  menuInicial();
  //Capa 2
  if (pic == 11 || pic == 12 || pic == 13) {
    header();
    menuCapa2(pic, 29);
  }
  if (pic == 21 || pic == 22 || pic == 23) {
    header();
    opcionesA_C(tipoModalidad);
    display.setCursor(0, 47);
    display.print(">");
    refresh();
  }
  //acepta capa 2
  if (pic == 111) {
    pic = 116;  //Para salir
    while (pic != 111) {
      if (comprueba != 1) {
        initWiFi();
      }
      almacenamiento();
    }
  } else if (pic == 112) {
    pic = 117;
    while (pic != 111) {
      if (comprueba == 0) {
        //ejecucionSetWifi(); Se comenta porque son las credenciales de Firestore
        initWiFi();
      } else {
      }
      almacenamiento();
    }
  } else if (pic == 113) {
    pic = 118;
    while (pic != 111) {
      refresh();
      display.setCursor(0, 15);
      display.setTextSize(1);
      display.print("Iniciando almacenamiento GSM");
      delay(2000);
      almacenamiento();
    }
  }
  if (pic == 121 || pic == 122 || pic == 123) {
    pic = 1;
    refresh();
  }
  if (pic == 14) {
    info();
    refresh();
  }
}

void ejecucionSetWifi() {
  initWiFi();
  config.api_key = API_KEY;
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;
  config.database_url = DATABASE_URL;
  Firebase.reconnectWiFi(true);
  fbdo.setResponseSize(4096);
  config.token_status_callback = tokenStatusCallback;
  config.max_token_generation_retry = 5;
  Firebase.begin(&config, &auth);
  Serial.println("Getting User UID");
  while ((auth.token.uid) == "") {
    Serial.print('.');
    delay(1000);
  }
  uid = auth.token.uid.c_str();
  Serial.print("User UID: ");
  Serial.println(uid);
  databasePath = "/UsersData/" + uid + "/readings";
}

void ContarPulsos() {
  NumPulsos++;
}

void ContarGiros() {
  entrada = digitalRead(SENSOR_ANE);

  if (entrada == 1 && !paso) {
    contadore++;
    paso = 1;
  }
  if (entrada == 0) {
    paso = 0;
  }
}

int ObtenerFrecuencia() {
  NumPulsos = 0;
  interrupts();
  delay(medidaIntervalo);
  noInterrupts();

  return (float)NumPulsos * 1000 / medidaIntervalo;
}

void mostrarDatos(int valorSensor, String nombre, int max, int min) {
  display.setCursor(0, 11);
  if (nombre == "Temperatura") {
    conexDTH22();
    display.drawBitmap(15, 11, temp, 30, 20, WHITE);
    display.setTextSize(2);
    display.setCursor(37, 35);
    display.print((char)247);
    display.print("C");
    display.setTextSize(3);
  } else if (nombre == "Humedad") {
    conexDTH22();
    display.drawBitmap(15, 11, hum, 30, 20, WHITE);
    if (HUMEDAD >= 100) {
      display.setCursor(55, 35);
    } else {
      display.setCursor(37, 35);
    }
    display.setTextSize(2);
    display.print("%");
    display.setTextSize(3);
  } else if (nombre == "Humedad suelo") {
    conexHumeSuelo();
    display.drawBitmap(10, 11, humSuelo, 30, 20, WHITE);
    if (SUELO == 100) {
      display.setCursor(55, 35);
    } else {
      display.setCursor(37, 35);
    }
    display.setTextSize(2);
    display.print("%");
    display.setTextSize(3);
  } else if (nombre == "Pluviosidad") {
    if (valorSensor == 0) {
      display.drawBitmap(15, 11, lluvia, 30, 20, WHITE);
    } else {
      display.drawBitmap(15, 11, nube, 30, 20, WHITE);
    }
    if (LLUVIANALOG == 100) {
      display.setCursor(55, 35);
    } else {
      display.setCursor(37, 35);
    }
    display.setTextSize(2);
    display.print("mm");
    display.setTextSize(3);
  } else if (nombre == "Luz") {
    conexLuz();
    if (LUZ < 112) {
      display.drawBitmap(10, 11, noche, 30, 20, WHITE);
    } else if (LUZ >= 350) {
      display.drawBitmap(15, 11, sol, 30, 20, WHITE);
    } else {
      display.drawBitmap(15, 11, nublado, 30, 20, WHITE);
    }
    display.setCursor(53, 35);
    display.setTextSize(1);
    display.print("ohm");
    if (LUZ > 999) {
      display.setTextSize(2);
    } else {
      display.setTextSize(3);
    }
  } else if (nombre == "Altitud") {
    conexBPM();
    display.drawBitmap(10, 11, altitud, 30, 20, WHITE);

    display.setCursor(55, 35);
    display.setTextSize(2);
    display.print("m");

    if (ALTITUD >= 999) {
      display.setTextSize(1);
    } else {
      display.setTextSize(3);
    }
  } else if (nombre == "Presion") {
    conexBPM();
    display.drawBitmap(10, 11, presion, 30, 20, WHITE);

    display.setCursor(55, 35);
    display.setTextSize(1);
    display.print("mb");

    if (PRESION >= 999) {
      display.setTextSize(1);
    } else {
      display.setTextSize(3);
    }
  } else if (nombre == "Temp Agua") {
    conexTempAgua();
    display.drawBitmap(15, 11, tempagua, 30, 20, WHITE);
    display.setCursor(37, 35);
    display.setTextSize(2);
    display.print((char)247);
    display.print("C");

    if (TEMP_AGUA >= 99) {
      display.setTextSize(1);
    } else {
      display.setTextSize(3);
    }
  } else if (nombre == "Caudal") {
    display.drawBitmap(15, 11, iconCaudal, 30, 20, WHITE);

    display.setCursor(37, 35);
    display.setTextSize(1);
    display.print("L/min");

    if (caudal_L_m >= 99) {
      display.setTextSize(1);
    } else {
      display.setTextSize(3);
    }
  } else if (nombre == "Anemometro") {
    display.drawBitmap(15, 11, ventilador, 30, 20, WHITE);

    display.setCursor(37, 35);
    display.setTextSize(1);
    display.print("m");
    display.setTextSize(3);
  } else if (nombre == "Luz Directa") {
    display.drawBitmap(15, 11, sol, 30, 20, WHITE);

    display.setCursor(37, 35);
    display.setTextSize(1);
    display.print("V");
    display.setTextSize(3);
  }

  display.setCursor(0, 35);
  display.print(valorSensor, 0);

  display.setTextSize(1);
  display.setCursor(50, 12);
  display.print(nombre);

  display.setTextSize(1);
  display.setCursor(75, 26);
  display.print("max:");
  display.print(max, 1);
  display.setCursor(75, 41);
  display.print("min:");
  display.print(min, 1);

  display.display();
}

void fechaActual() {
  time_t now = time(nullptr);
  struct tm* p_tm = localtime(&now);

  /*display.setCursor(0, 57);
  display.print(p_tm->tm_hour);
  display.print(":");
  if (p_tm->tm_min < 10)
    display.print("0");
  display.print(p_tm->tm_min);

  display.setCursor(64, 57);
  display.print(p_tm->tm_mday);
  display.print("/");
  display.print(p_tm->tm_mon + 1);
  display.print("/");
  display.print(p_tm->tm_year + 1900);*/
}

void mostrarTodo() {
  display.setTextSize(1);

  //fechaActual();

  display.setCursor(0, 14);
  display.print("T:");
  display.print(TEMPERATURA, 0);

  display.setCursor(0, 24);
  display.print("H:");
  display.print(HUMEDAD, 0);

  display.setCursor(0, 34);
  display.print("HS:");
  display.print(SUELO, 0);

  display.setCursor(0, 44);
  display.print("LL:");
  display.print(LLUVIANALOG, 0);

  display.setCursor(35, 14);
  display.print("LU:");
  display.print(LUZ, 0);

  display.setCursor(35, 24);
  display.print("AL:");
  display.print(ALTITUD, 0);

  display.setCursor(35, 34);
  display.print("P:");
  display.print(PRESION, 0);

  display.setCursor(35, 44);
  display.print("TA:");
  display.print(TEMP_AGUA, 0);

  display.setCursor(84, 14);
  display.print("Ca:");
  display.print(caudal_L_m, 0);

  display.setCursor(84, 24);
  display.print("T-B:");
  display.print(TEMP_BPM, 0);

  display.setCursor(84, 34);
  display.print("PA:");
  display.print(LUZ_DIRECTA, 0);

  display.setCursor(84, 44);
  display.print("AN:");
  display.print(ANEMOMETRO, 0);

  display.display();
}

void conexDTH22() {
  if (isnan(TEMPERATURA) || isnan(HUMEDAD)) {
    display.setTextSize(1);
    display.setCursor(0, 57);
    display.print(F("Problema con DHT22"));
    display.display();
  }
}

void conexHumeSuelo() {
  if (SUELO > 90) {
    display.setTextSize(1);
    display.setCursor(0, 57);
    display.print(F("Error sensor de suelo"));
    display.display();
  }
}

void conexLuz() {
  if (LUZ == 0 || LUZ >= 2000) {
    display.setTextSize(1);
    display.setCursor(0, 57);
    display.print(F("Error sensor de luz"));
    display.display();
  }
}

void conexBPM() {
  if (isnan(ALTITUD)) {
    display.setTextSize(1);
    display.setCursor(0, 57);
    display.print(F("Error sensor presion BPM"));
    display.display();
  }
}

void conexTempAgua() {
  if (TEMP_AGUA == -127) {
    display.setTextSize(1);
    display.setCursor(0, 57);
    display.print(F("Error sensor temp agua"));
    display.display();
  }
}

void enviarSD() {
  display.setCursor(20, 0);
  sd_file = SD.open(nombreArchivo, FILE_APPEND);
  if (sd_file) {
    enviarDatos();
    display.setTextSize(1);
    display.write(24);
  } else {
    display.setTextSize(1);
    display.print("X");
    Serial.println("Problema al enviar registros");
  }
  display.display();
  /*********************************************/
}

void enviarDatos() {
  sd_file.print(
    String(timestamp) + "," + String(TEMPERATURA) + "," + String(HUMEDAD) + "," + String(SUELO) + "," + String(LLUVIANALOG) + "," + String(LUZ) + "," + String(ALTITUD) + "," + String(PRESION) + "," + String(TEMP_AGUA) + "," + String(LUZ_DIRECTA) + "," + String(caudal_L_m) + "," + String(ANEMOMETRO) + "\n");
  sd_file.close();
  Serial.println(contador);
  contador++;
}

void headerArchivo() {
  sd_file = SD.open(nombreArchivo, FILE_WRITE);
  if (sd_file) {
    sd_file.print("Tiempo");
    sd_file.print(",");
    sd_file.print("Temperatura");
    sd_file.print(",");
    sd_file.print("Humedad");
    sd_file.print(",");
    sd_file.print("TempSuelo");
    sd_file.print(",");
    sd_file.print("Lluvia");
    sd_file.print(",");
    sd_file.print("Luz");
    sd_file.print(",");
    sd_file.print("Altitud");
    sd_file.print(",");
    sd_file.print("Presion");
    sd_file.print(",");
    sd_file.print("TempAgua");
    sd_file.print(",");
    sd_file.print("LuzDirecta");
    sd_file.print(",");
    sd_file.println("Caudal");
    sd_file.print(",");
    sd_file.println("Anemometro");

    sd_file.close();

    cabeceraCreada = 1;
    Serial.print("Cabecera creada");
  } else {
    cabeceraCreada = 0;
  }
}

void header() {
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.print("D");
  display.write(252);
  display.drawLine(0, 9, 128, 9, WHITE);

  if (sd_iniciada) {
    display.drawBitmap(20, 0, sdicon, 9, 9, WHITE);
  } else {
    display.setCursor(13, 0);
    display.print("x");
    display.drawBitmap(20, 0, sdicon, 9, 9, WHITE);
  }

  display.setCursor(32, 0);
  display.print("M");
  if (tipoModalidad == "GSM") {
    display.print("3");
  } else if (tipoModalidad == "Wi-Fi") {
    display.print("2");
  } else if (tipoModalidad == "Local") {
    display.print("1");
  } else {
    display.print("0");
  }

  display.setCursor(45, 0);
  display.write(178);
  display.print(valPot);
  display.write(178);

  display.drawBitmap(68, 0, gsmicon, 9, 9, WHITE);

  display.drawBitmap(82, 0, wificon, 9, 9, WHITE);

  display.drawBitmap(95, 0, bateriaicon, 13, 9, WHITE);
  display.setCursor(110, 0);
  display.print("100");
}

void iniciaVariables() {
  LUZ_DIRECTA = map(analogRead(SENSOR1_LUZ), 0, 4095, 0, 30);
  TEMPERATURA = dht.readTemperature();
  HUMEDAD = dht.readHumidity();
  SUELO = map(analogRead(SENSOR_HS), 0, 4095, 100, 0);
  LLUVIANALOG = map(analogRead(lluviaAnalog), 0, 4095, 100, 0);
  LUZ = analogRead(SENSOR_LUZ);
  ALTITUD = bmp280.readAltitude(1011.18);
  PRESION = (bmp280.readPressure() / 100);
  TEMP_BPM = bmp280.readTemperature();
  DS18B20.requestTemperatures();
  TEMP_AGUA = DS18B20.getTempCByIndex(0);
  ANEMOMETRO = contadore;
  //tempF = tempC * 9 / 5 + 32;
  float frecuencia = ObtenerFrecuencia();       //obtenemos la Frecuencia de los pulsos en Hz
  caudal_L_m = frecuencia / factor_conversion;  //calculamos el caudal en L/m

  valPot = map(analogRead(36), 0, 4095, 0, 13);
}

void minmaxVal() {
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
  if (TEMP_AGUA > maxtemp_agua) {
    maxtemp_agua = TEMP_AGUA;
  }
  if (TEMP_AGUA < mintemp_agua) {
    mintemp_agua = TEMP_AGUA;
  }
  if (caudal_L_m > maxcaudal) {
    maxcaudal = caudal_L_m;
  }
  if (caudal_L_m < mincaudal) {
    mincaudal = caudal_L_m;
  }
  if (ANEMOMETRO > maxane) {
    maxane = ANEMOMETRO;
  }
  if (ANEMOMETRO < minane) {
    minane = ANEMOMETRO;
  }
  if (LUZ_DIRECTA > maxdirecta) {
    maxdirecta = LUZ_DIRECTA;
  }
  if (LUZ_DIRECTA < mindirecta) {
    mindirecta = LUZ_DIRECTA;
  }
}

void click() {
  lastmillis = millis();
  if (pic >= 0 && pic < 10) {
    if (pic >= maxPics_L1) {
      pic = 1;
    } else if (pic < maxPics_L1) {
      pic++;
    }
  }

  if (pic >= 10 && pic < 100) {
    if (pic > maxPics_L2 * 10) {
      pic = 11;
    } else if (pic < maxPics_L2 * 10) {
      pic = pic + 10;
    }
  }
}

void doubleclick() {
  lastmillis = millis();
  if (pic == 11 || pic == 21 || pic == 111) pic = 1;
  if (pic == 12 || pic == 22 || pic == 112) pic = 2;
  if (pic == 13 || pic == 23 || pic == 113) pic = 3;
  if (pic == 14 || pic == 24 || pic == 114) pic = 4;

  if (pic == 116) pic = 1;
}

void longPressStart() {
  lastmillis = millis();
  if (pic >= 1 && pic < 5) {
    pic = pic + 10;
  } else if (pic > 10 && pic < 25) {
    pic = pic + 100;
  }
}

void longPressStop() {
  ;
}

void refresh() {
  display.display();
  delay(00);
  display.clearDisplay();
}

void menuInicial() {
  if (pic == 1) {
    header();
    opciones();
    display.setCursor(0, 20);
    display.print(">");
    refresh();
  }
  if (pic == 2) {
    header();
    opciones();
    display.setCursor(0, 29);
    display.print(">");
    refresh();
  }
  if (pic == 3) {
    header();
    opciones();
    display.setCursor(0, 38);
    display.print(">");
    refresh();
  }
  if (pic == 4) {
    header();
    opciones();
    display.setCursor(0, 56);
    display.print(">");
    refresh();
  }
}

void opciones() {
  display.setCursor(0, 11);
  display.print("Men");
  display.write(151);
  display.print(" de Opciones");
  display.setCursor(0, 20);
  display.print(" LOCAL");
  display.setCursor(0, 29);
  display.print(" Wi-Fi");
  display.setCursor(0, 38);
  display.print(" GSM");
  display.setCursor(0, 56);
  display.print(" INFO");
}

void opcionesA_C(String tipoModalidad) {
  display.setCursor(0, 11);
  display.print("Almacenamiento " + tipoModalidad);
  display.setCursor(0, 29);
  display.print(" EMPEZAR");
  display.setCursor(0, 47);
  display.print(" CANCELAR");
}

void menuCapa2(int pico, int y) {
  if (pico == 11) {
    tipoModalidad = "Local";
    opcionesA_C(tipoModalidad);
  }
  if (pico == 12) {
    tipoModalidad = "Wi-Fi";
    opcionesA_C(tipoModalidad);
  }
  if (pico == 13) {
    tipoModalidad = "GSM";
    opcionesA_C(tipoModalidad);
  }
  display.setCursor(0, y);
  display.print(">");
  refresh();
}

void info() {
  display.setCursor(0, 15);
  display.print("Realizado por:");
  display.setCursor(0, 24);
  display.print("-> Dayan Puetate y");
  display.setCursor(0, 33);
  display.print("-> Ernesto Bahamonde");
}

void almacenamiento() {

  refresh();
  iniciaVariables();

  minmaxVal();

  header();

  if (pic == 116) {
    while (cabeceraCreada != 1) {
      headerArchivo();
    }
    if (millis() - sendDataPrevMillis > timerDelay || sendDataPrevMillis == 0) {
      timestamp = getTime();
      timestamp = timestamp - timezone;
      sendDataPrevMillis = millis();
      enviarSD();
    }
  } else if (pic == 117) {
    //enviarWifi();
    enviarSheets();
  } else if (pic == 118) {
    if (millis() - sendDataPrevMillis > timerDelay || sendDataPrevMillis == 0) {
      timestamp = getTime();
      timestamp = timestamp - timezone;
      sendDataPrevMillis = millis();
      if (!is_gprs_connected()) {
        gprs_connect();
      }

      post_to_sheets();
    }
  }

  if (valPot == 0) {
    mostrarTodo();
  }
  if (valPot == 1) {
    mostrarDatos(TEMPERATURA, "Temperatura", maxtemp, mintemp);
  }
  if (valPot == 2) {
    mostrarDatos(HUMEDAD, "Humedad", maxhum, minhum);
  }
  if (valPot == 3) {
    mostrarDatos(SUELO, "Humedad suelo", maxhs, minhs);
  }
  if (valPot == 4) {
    mostrarDatos(LLUVIANALOG, "Pluviosidad", maxllu, minllu);
  }
  if (valPot == 5) {
    mostrarDatos(LUZ, "Luz", maxluz, minluz);
  }
  if (valPot == 6) {
    mostrarDatos(ALTITUD, "Altitud", maxalti, minalti);
  }
  if (valPot == 7) {
    mostrarDatos(PRESION, "Presion", maxpres, minpres);
  }
  if (valPot == 8) {
    mostrarDatos(TEMP_AGUA, "Temp Agua", maxtemp_agua, mintemp_agua);
  }
  if (valPot == 9) {
    mostrarDatos(LUZ_DIRECTA, "Luz Directa", maxdirecta, mindirecta);
  }
  if (valPot == 10) {
    mostrarDatos(caudal_L_m, "Caudal", maxcaudal, mincaudal);
  }
  if (valPot == 11) {
    mostrarDatos(ANEMOMETRO, "Anemometro", maxane, minane);
  }
  if (valPot == 12) {
    pic = 111;  //--> Hasta aquí sale con 111 del while --> colocar un botón para poder salir
  }
  if (valPot == 13) {
    pic = 111;  //--> Hasta aquí sale con 111 del while --> colocar un botón para poder salir
  }

  Serial.print(" ");
  Serial.print("Tiempo: ");
  Serial.print(timestamp);
  Serial.print("\tTemperatura: ");
  Serial.print(TEMPERATURA);
  Serial.print("\tHumedad: ");
  Serial.print(HUMEDAD);
  Serial.print("\tHumedad Suelo: ");
  Serial.print(SUELO);
  Serial.print("\tLluvia A: ");
  Serial.print(LLUVIANALOG);
  Serial.print("\tLuz: ");
  Serial.print(LUZ);
  Serial.print("\tAltitud: ");
  Serial.print(ALTITUD);
  Serial.print("\tPresion: ");
  Serial.print(PRESION);
  Serial.print("\tTemp Agua: ");
  Serial.print(TEMP_AGUA);
  Serial.print("LuzDirecta: ");
  Serial.print(LUZ_DIRECTA);
  Serial.print("\tCaudal: ");
  Serial.print(caudal_L_m, 3);
  Serial.print(" L/min");
  Serial.print("\tAnemometro: ");
  Serial.print(ANEMOMETRO);
  Serial.print("\tTEMP_BPM: ");
  Serial.print(TEMP_BPM);
  Serial.print("\tPotenciometro: ");
  Serial.print(valPot);
  Serial.println();
}

void initWiFi() {
  refresh();
  display.setCursor(0, 15);
  Serial.print("Conectando Wi-Fi..");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  display.println("\nConectando a Wi-Fi");
  display.display();
  while (/*WiFi.status() != WL_CONNECTED ||*/ countWi != 40) {
    Serial.print('.');
    countWi++;
    Serial.print(countWi);
    delay(500);
    display.print(".");
    display.display();
  }
  refresh();
  Serial.println(WiFi.localIP());
  Serial.println();
  display.setCursor(0, 24);
  display.print(WiFi.localIP());
  comprueba = 1;
}

unsigned long getTime() {
  time_t now;
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    //Serial.println("Failed to obtain time");
    return (0);
  }
  time(&now);
  return now;
}

void enviarWifi() {
  timestamp = getTime();
  timestamp = timestamp - timezone;

  if (Firebase.ready() && (millis() - sendDataPrevMillis > timerDelay || sendDataPrevMillis == 0) && timestamp != 0) {
    sendDataPrevMillis = millis();

    Serial.print("time: ");
    Serial.println(timestamp);

    parentPath = /*databasePath + "/" + */ String(timestamp);

    json.set(tempPath.c_str(), String(TEMPERATURA));
    json.set(humPath.c_str(), String(HUMEDAD));
    json.set(suePath.c_str(), String(SUELO));
    json.set(lluPath.c_str(), String(LLUVIANALOG));
    json.set(luzPath.c_str(), String(LUZ));
    json.set(altiPath.c_str(), String(ALTITUD));
    json.set(presPath.c_str(), String(PRESION));
    json.set(tempAPath.c_str(), String(TEMP_AGUA));
    json.set(luz_direcPath.c_str(), String(LUZ_DIRECTA));
    json.set(caudalPath.c_str(), String(caudal_L_m));
    json.set(anePath.c_str(), String(ANEMOMETRO));
    Serial.printf("Set json... %s\n", Firebase.RTDB.setJSON(&fbdo, parentPath.c_str(), &json) ? "ok" : fbdo.errorReason().c_str());
  }
}

void enviarSheets() {
  timestamp = getTime();
  timestamp = timestamp - timezone;
  if ((millis() - sendDataPrevMillis > timerDelay || sendDataPrevMillis == 0) && timestamp != 0) {
    sendDataPrevMillis = millis();
    String urlFinal = "https://script.google.com/macros/s/" + GOOGLE_SCRIPT_ID + "/exec?" + "Tiempo=" + String(timestamp) + "&Temperatura=" + String(TEMPERATURA) + "&Humedad=" + String(HUMEDAD) + "&TempSuelo=" + String(SUELO) + "&Lluvia=" + String(LLUVIANALOG) + "&Luz=" + String(LUZ) + "&Altitud=" + String(ALTITUD) + "&Presion=" + String(PRESION) + "&TempAgua=" + String(TEMP_AGUA) + "&LuzDirecta=" + String(LUZ_DIRECTA) + "&Caudal=" + String(caudal_L_m) + "&Anemometro=" + String(ANEMOMETRO);
    HTTPClient http;
    http.begin(urlFinal.c_str());
    http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
    int httpCode = http.GET();
    String payload;
    if (httpCode > 0) {
      payload = http.getString();
    }
    http.end();
  }
}

void post_to_sheets() {

  Serial2.println("AT+SAPBR=3,1,\"Contype\",\"GPRS\"");
  delay(DELAY_MS);
  waitResponse();
  Serial2.println("AT+CSTT=\"internet\",\"guest\",\"guest\"");
  delay(DELAY_MS);
  waitResponse();
  Serial2.println("AT+SAPBR=1,1");
  delay(DELAY_MS);
  waitResponse();
  Serial2.println("AT+HTTPINIT");
  delay(DELAY_MS);
  waitResponse();
  Serial2.println("AT+HTTPPARA=\"CID\",1");
  delay(DELAY_MS);
  waitResponse();
  Serial2.println("AT+HTTPPARA=\"URL\",\"https://script.google.com/macros/s/AKfycbyD_AnwXW6JycjCNBVm265IaQznudTUHK5tJg55LnnMueZmxzuVGXs566DnSJOsCmCZ/exec?Tiempo=" + String(timestamp) + "&Temperatura=" + String(TEMPERATURA) + "&Humedad=" + String(HUMEDAD) + "&TempSuelo=" + String(SUELO) + "&Lluvia=" + String(LLUVIANALOG) + "&Luz=" + String(LUZ) + "&Altitud=" + String(ALTITUD) + "&Presion=" + String(PRESION) + "&TempAgua=" + String(TEMP_AGUA) + "&LuzDirecta=" + String(LUZ_DIRECTA) + "&Caudal=" + String(caudal_L_m) + "&Anemometro=" + String(ANEMOMETRO) + "\"");
  waitResponse();
  Serial.println(" ");
  Serial2.println("AT+HTTPSSL=1");
  delay(DELAY_MS);
  waitResponse();
  Serial2.println("AT+HTTPACTION=0");
  delay(DELAY_MS);
  waitResponse();
  Serial2.println("AT+HTTPREAD");
  delay(DELAY_MS);
  waitResponse();
  Serial2.println("AT+HTTPTERM");
  delay(DELAY_MS);
}
void init_gsm() {
  //Testing AT Command
  Serial2.println("AT");
  waitResponse();
  delay(DELAY_MS);
  //Checks if the SIM is ready
  Serial2.println("AT+CPIN?");
  waitResponse("+CPIN: READY");
  delay(DELAY_MS);
  //Turning ON full functionality
  Serial2.println("AT+CFUN=1");
  waitResponse();
  delay(DELAY_MS);
  //Turn ON verbose error codes
  Serial2.println("AT+CMEE=2");
  waitResponse();
  delay(DELAY_MS);
  //Enable battery checks
  Serial2.println("AT+CBATCHK=1");
  waitResponse();
  delay(DELAY_MS);
  //Register Network (+CREG: 0,1 or +CREG: 0,5 for valid network)
  //+CREG: 0,1 or +CREG: 0,5 for valid network connection
  Serial2.println("AT+CREG?");
  waitResponse("+CREG: 0,");
  delay(DELAY_MS);
  //setting SMS text mode
  Serial2.print("AT+CMGF=1\r");
  waitResponse("OK");
  delay(DELAY_MS);
}
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//Connect to the internet
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
void gprs_connect() {
  //DISABLE GPRS
  Serial2.println("AT+SAPBR=0,1");
  waitResponse("OK", 60000);
  delay(DELAY_MS);
  //Connecting to GPRS: GPRS - bearer profile 1
  Serial2.println("AT+SAPBR=3,1,\"Contype\",\"GPRS\"");
  waitResponse();
  delay(DELAY_MS);
  //sets the APN settings for your sim card network provider.
  Serial2.println("AT+SAPBR=3,1,\"APN\"," + APN);
  waitResponse();
  delay(DELAY_MS);
  //sets the user name settings for your sim card network provider.
  if (USER != "") {
    Serial2.println("AT+SAPBR=3,1,\"USER\"," + USER);
    waitResponse();
    delay(DELAY_MS);
  }
  //sets the password settings for your sim card network provider.
  if (PASS != "") {
    Serial2.println("AT+SAPBR=3,1,\"PASS\"," + PASS);
    waitResponse();
    delay(DELAY_MS);
  }
  //after executing the following command. the LED light of
  //sim800l blinks very fast (twice a second)
  //enable the GPRS: enable bearer 1
  Serial2.println("AT+SAPBR=1,1");
  waitResponse("OK", 30000);
  delay(DELAY_MS);
  //Get IP Address - Query the GPRS bearer context status
  Serial2.println("AT+SAPBR=2,1");
  waitResponse("OK");
  delay(DELAY_MS);
}
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
* Function: gprs_disconnect()
* AT+CGATT = 1 modem is attached to GPRS to a network. 
* AT+CGATT = 0 modem is not attached to GPRS to a network
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
boolean gprs_disconnect() {
  //Disconnect GPRS
  Serial2.println("AT+CGATT=0");
  waitResponse("OK", 60000);
  return true;
}
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
* Function: gprs_disconnect()
* checks if the gprs connected.
* AT+CGATT = 1 modem is attached to GPRS to a network. 
* AT+CGATT = 0 modem is not attached to GPRS to a network
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
boolean is_gprs_connected() {
  Serial2.println("AT+CGATT?");
  if (waitResponse("+CGATT: 1", 6000) == 1) { return false; }
  return true;
}
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//Handling AT COMMANDS
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//boolean waitResponse(String expected_answer="OK", unsigned int timeout=2000) //uncomment if syntax error (arduino)
boolean waitResponse(String expected_answer, unsigned int timeout)  //uncomment if syntax error (esp8266)
{
  uint8_t x = 0, answer = 0;
  String response;
  unsigned long previous;

  //Clean the input buffer
  while (Serial2.available() > 0) Serial2.read();
  previous = millis();
  do {
    //if data in UART INPUT BUFFER, reads it
    if (Serial2.available() != 0) {
      char c = Serial2.read();
      response.concat(c);
      x++;
      //checks if the (response == expected_answer)
      if (response.indexOf(expected_answer) > 0) {
        answer = 1;
      }
    }
  } while ((answer == 0) && ((millis() - previous) < timeout));

  Serial.println(response);
  return answer;
}