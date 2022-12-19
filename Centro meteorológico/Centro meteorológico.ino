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
int lluviaAnalog = 35, lluviaDigital = 34, LLUVIANALOG, LLUVIADIGI, maxllu, minllu = 100;
//Sensor de luminosidad
int SENSORLUZ = 13, LUZ;
int maxluz = 0, minluz = 500;
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
int SENSOR1_LUZ = 33, SENSOR2_LUZ = 25, minluz1_difusa = 5, minluz1_reflejada = 5, maxluz1_difusa = 0, maxluz1_reflejada = 0;
//Sdk
#define CS_PIN 5
String registro;
long segundos = 0.0, minutos = 0.0, horas = 0.0;
bool sd_iniciada;
String nombreArchivo = "/dataESP32_D.txt";
int contador = 0, linea = 0;
byte caracter;
int cabeceraCreada = 0;
int tiemporecoleccion = 17;  //~1min -> 17//~2min->37 |~5min->97 |~10min->191  | 15min ->    PUEDE SER REEMPLAZADO CON TIEMPOS CON LA HORA
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
//#define WIFI_SSID "TP-Link_293C"
//#define WIFI_PASSWORD "13904906"

#define API_KEY "AIzaSyASpyvFug26lUAYfnODjtJLw0Jg8iLZ7og"
#define USER_EMAIL "edbahamonde@espe.edu.ec"
#define USER_PASSWORD "172596dD"
#define DATABASE_URL "https://esp-firebase-demo-78fab-default-rtdb.firebaseio.com"
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
String frecPath = "/FrecuenciaPulsos";
String caudalPath = "/Caudal";
String parentPath;
int timestamp;
FirebaseJson json;
const char* ntpServer = "pool.ntp.org";
unsigned long sendDataPrevMillis = 0;
unsigned long timerDelay = 60000;  // = 1 min
int comprueba = 0;
unsigned long valordetiempo = -18000;

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

void setup() {
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();

  Serial.begin(9600);

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
  pinMode(lluviaDigital, INPUT);
  pinMode(CAUDAL, INPUT);
  attachInterrupt(digitalPinToInterrupt(CAUDAL), ContarPulsos, RISING);

  DS18B20.begin();

  button.attachClick(click);
  button.attachDoubleClick(doubleclick);
  button.attachLongPressStop(longPressStop);
  button.attachLongPressStart(longPressStart);

  configTime(valordetiempo, 0, ntpServer);
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
    while (valPot != 10) {
      header();
      display.setTextSize(1);
      display.setCursor(0, 57);
      display.print(F("CALIBRACI"));
      display.write(149);
      display.print(F("N"));
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
  if (pic == 111 || pic == 112 || pic == 113) {  //ELIMINAR AL TENER TODOS LOS CASOS
    if (pic == 111) {
      pic = 116;  //Para salir
      while (pic != 111) {
        if (comprueba == 0) {
          initWiFi();
        } else {
        }
        iniciaVariables();
        almacenamientoLocal();
      }
    } else if (pic == 112) {
      pic = 117;
      while (pic != 111) {
        if (comprueba == 0) {
          ejecucionSetWifi();
        } else {
        }
        iniciaVariables();
        almacenamientoLocal();
      }
    } else if (pic == 113) {
      pic = 116;
      while (pic != 113) {
        refresh();
        display.setCursor(0, 15);
        display.setTextSize(1);
        display.print("Iniciando almacenamiento GSM");
        delay(2000);
        pic = 113;
      }
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

void mostrarTodo() {
  display.setTextSize(1);

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
  display.print("Fr:");
  display.print(frecuencia, 0);

  display.setCursor(84, 24);
  display.print("Ca:");
  display.print(caudal_L_m, 0);

  display.setCursor(84, 34);
  display.print("T-B:");
  display.print(TEMP_BPM, 0);

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
    String(timestamp) + "," + String(TEMPERATURA) + "," + String(HUMEDAD) + "," + String(SUELO) + "," + String(LLUVIANALOG) + "," + String(LUZ) + "," + String(ALTITUD) + "," + String(PRESION) + "," + String(TEMP_AGUA) + "," + String(frecuencia) + "," + String(caudal_L_m) + "\n");
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
    sd_file.print("FrecuenciaPulsos");
    sd_file.print(",");
    sd_file.println("Caudal");

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
  TEMPERATURA = dht.readTemperature();
  HUMEDAD = dht.readHumidity();
  SUELO = map(analogRead(SENSOR_HS), 0, 4095, 100, 0);
  LLUVIANALOG = map(analogRead(lluviaAnalog), 0, 4095, 100, 0);
  LLUVIADIGI = digitalRead(lluviaDigital);
  //LUZ = map(analogRead(SENSORLUZ), 100, 4000, 0, 100);
  LUZ = analogRead(SENSORLUZ);
  ALTITUD = bmp280.readAltitude(1011.18);
  PRESION = (bmp280.readPressure() / 100);
  TEMP_BPM = bmp280.readTemperature();

  DS18B20.requestTemperatures();
  TEMP_AGUA = DS18B20.getTempCByIndex(0);
  //tempF = tempC * 9 / 5 + 32;
  float frecuencia = ObtenerFrecuencia();       //obtenemos la Frecuencia de los pulsos en Hz
  caudal_L_m = frecuencia / factor_conversion;  //calculamos el caudal en L/m

  valPot = map(analogRead(36), 0, 4095, 0, 10);
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

void almacenamientoLocal() {

  refresh();

  minmaxVal();

  header();

  if (pic == 116) {
    if (cabeceraCreada != 1) {
      headerArchivo();
    }
    if (millis() - sendDataPrevMillis > timerDelay || sendDataPrevMillis == 0 && timestamp != 0) {
      sendDataPrevMillis = millis();
      enviarSD();
    }
  } else if (pic == 117) {
    enviarWifi();
  }

  if (valPot == 0) {
    mostrarTodo();
  }
  if (valPot == 1) {
    mostrarDatos(HUMEDAD, "Humedad", maxhum, minhum);
  }
  if (valPot == 2) {
    mostrarDatos(SUELO, "Humedad suelo", maxhs, minhs);
  }
  if (valPot == 3) {
    mostrarDatos(LLUVIADIGI, "Pluviosidad", maxllu, minllu);
  }
  if (valPot == 4) {
    mostrarDatos(LUZ, "Luz", 300, 100);
  }
  if (valPot == 5) {
    mostrarDatos(ALTITUD, "Altitud", maxalti, minalti);
  }
  if (valPot == 6) {
    mostrarDatos(PRESION, "Presion", maxpres, minpres);
  }
  if (valPot == 7) {
    mostrarDatos(TEMP_AGUA, "Temp Agua", maxtemp_agua, mintemp_agua);
  }
  if (valPot == 8) {
    mostrarDatos(TEMPERATURA, "Temperatura", maxtemp, mintemp);
  }
  if (valPot == 9) {
    mostrarDatos(caudal_L_m, "Caudal", maxcaudal, mincaudal);
  }
  if (valPot == 10) {
    pic = 111;  //--> Hasta aquí sale con 111 del while --> colocar un botón para poder salir
  }
  timestamp = getTime();
  Serial.print(" ");
  Serial.print("Tiempo: ");
  Serial.print(timestamp);
  Serial.print("\tTemperatura: ");
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
  Serial.print("\tTEMP_BPM: ");
  Serial.print(TEMP_BPM);
  Serial.print("\tTemp Agua: ");
  Serial.print(TEMP_AGUA);
  Serial.print("FrecuenciaPulsos: ");
  Serial.print(frecuencia);
  Serial.print(" Hz\tCaudal: ");
  Serial.print(caudal_L_m, 3);
  Serial.print(" L/min");
  Serial.print("\tPotenciometro: ");
  Serial.print(valPot);
  Serial.println();
}

void initWiFi() {
  refresh();
  display.setCursor(0, 15);
  display.print("Conectando Wi-Fi..");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Conectando Wi-Fi..");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
  }
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

  if (Firebase.ready() && (millis() - sendDataPrevMillis > timerDelay || sendDataPrevMillis == 0) && timestamp != 0) {
    sendDataPrevMillis = millis();

    Serial.print("time: ");
    Serial.println(timestamp);

    parentPath = databasePath + "/" + String(timestamp);

    json.set(tempPath.c_str(), String(TEMPERATURA));
    json.set(humPath.c_str(), String(HUMEDAD));
    json.set(suePath.c_str(), String(SUELO));
    json.set(lluPath.c_str(), String(LLUVIANALOG));
    json.set(luzPath.c_str(), String(LUZ));
    json.set(altiPath.c_str(), String(ALTITUD));
    json.set(presPath.c_str(), String(PRESION));
    json.set(tempAPath.c_str(), String(TEMP_AGUA));
    json.set(frecPath.c_str(), String(frecuencia));
    json.set(caudalPath.c_str(), String(caudal_L_m));
    Serial.printf("Set json... %s\n", Firebase.RTDB.setJSON(&fbdo, parentPath.c_str(), &json) ? "ok" : fbdo.errorReason().c_str());
  }
}
