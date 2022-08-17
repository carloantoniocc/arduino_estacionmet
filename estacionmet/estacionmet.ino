#include "DHT.h"        // including the library of DHT11 temperature and humidity sensor
#define DHTTYPE DHT22   // DHT 11
#include <Wire.h>      // libreria de comunicacion por I2C
#include <LCD.h>      // libreria para funciones de LCD
#include <LiquidCrystal_I2C.h>    // libreria para LCD por I2C
#include <Adafruit_Sensor.h>
#include <DHT_U.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

// librerias
// https://github.com/adafruit/DHT-sensor-library
// https://github.com/adafruit/Adafruit_Sensor
// https://github.com/fmalpartida/New-LiquidCrystal


#define dht_dpin 0
DHT dht(dht_dpin, DHTTYPE); 

uint32_t delayMS;

// Icono de Humededad
byte humchar1[8]={B00000, B00001, B00011, B00011, 
                  B00111, B01111, B01111, B11111,};
byte humchar2[8]={B11111, B11111, B11111, B01111, 
                  B00011, B00000, B00000, B00000,};
byte humchar3[8]={B00000, B10000, B11000, B11000, 
                  B11100, B11110, B11110, B11111,};
byte humchar4[8]={B11111, B11111, B11111, B11110,
                  B11100, B00000, B00000, B00000,};

//Icono de Termometro
byte tempchar1[8]={B00000, B00001, B00010, B00100, //Row 0, Col 2
                  B00100, B00100, B00100, B00111,};
byte tempchar2[8]={B00111, B00111, B00111, B01111, //Row 1, Col 2
                  B11111, B11111, B01111, B00011,};
byte tempchar3[8]={B00000, B10000, B01011, B00100, //ROW 0, Col 3
                  B00111, B00100, B00111, B11100,};
byte tempchar4[8]={B11111, B11100, B11100, B11110, //Row 1, Col 3
                  B11111, B11111, B11110, B11000,};

//Icono de House
byte housechar1[8]={B00000, B00001, B00011, B00011, //Row 0, Col 0
                  B00111, B01111, B01111, B11111,};
byte housechar2[8]={B11111, B11111, B11100, B11100, //Row 1, Col 0
                  B11100, B11100, B11100, B11100,};
byte housechar3[8]={B00000, B10010, B11010, B11010, //ROW 0, Col 1
                  B11110, B11110, B11110, B11111,};
byte housechar4[8]={B11111, B11111, B11111, B10001, //Row 1, Col 1
                    B10001, B10001, B11111, B11111,};

// creamos el objeto lcd
LiquidCrystal_I2C lcd (0x27, 2, 1, 0, 4, 5, 6, 7); // DIR, E, RW, RS, D4, D5, D6, D7

//#define SERVER_IP "10.0.1.7:9080" // PC address with emulation on host
#define SERVER_IP "192.168.137.1"

#ifndef STASSID
#define STASSID "dell"
#define STAPSK  "zero1234"
#endif

void setup(void)
{ 

  lcd.setBacklightPin(3,POSITIVE);  // puerto P3 de PCF8574 como positivo
  lcd.setBacklight(HIGH);   // habilita iluminacion posterior de LCD
  lcd.begin(16, 2);     // 16 columnas por 2 lineas para LCD 1602A
  lcd.clear();      // limpia pantalla

  firmware_version();

  Serial.begin(9600);
  Serial.println();
  dht.begin();

  WiFi.begin(STASSID, STAPSK);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Conectado! IP address: ");
  Serial.println(WiFi.localIP());

}


void loop() {
    float h = dht.readHumidity();
    float t = dht.readTemperature();         
    unsigned long currentMillis = millis();
    
    delay(delayMS);
    // Get temperature event and print its value.

   
    lcd.clear();
    delay(1000);
    lcd.createChar(1,tempchar1);
    lcd.createChar(2,tempchar2);
    lcd.createChar(3,tempchar3);
    lcd.createChar(4,tempchar4);
    lcd.setCursor(0,0);
    lcd.write(1);
    lcd.setCursor(0,1);
    lcd.write(2);
    lcd.setCursor(1,0);
    lcd.write(3);
    lcd.setCursor(1,1);
    lcd.write(4);
    lcd.setCursor(3,0);
    lcd.print("Temperatura");
    lcd.setCursor(3,1);
    lcd.print(t);
    lcd.setCursor(9,1);
    lcd.print((char)223);
    lcd.setCursor(10,1);
    lcd.print("C");
    delay(3000);

 
    lcd.clear();
    delay(1000);
    lcd.createChar(1,humchar1);
    lcd.createChar(2,humchar2);
    lcd.createChar(3,humchar3);
    lcd.createChar(4,humchar4);
    lcd.setCursor(0,0);
    lcd.write(1);
    lcd.setCursor(0,1);
    lcd.write(2);
    lcd.setCursor(1,0);
    lcd.write(3);
    lcd.setCursor(1,1);
    lcd.write(4);
    lcd.setCursor(3,0);
    lcd.print("Humedad");
    lcd.setCursor(3,1);
    lcd.print(h);
    delay(3000);

    envia_post(t,h);
    
}

void envia_post(float t, float h)
{

    Serial.print("Temperatura: ");
    Serial.println(t);
    Serial.print("Humedad: ");
    Serial.println(h);

  if ((WiFi.status() == WL_CONNECTED)) {

    WiFiClient client;
    HTTPClient http;

    Serial.print("[HTTP] begin...\n");
    // configure traged server and url
    http.begin(client, "http://" SERVER_IP "/apimet/public/api/v1/registro"); //HTTP
    http.addHeader("Content-Type", "application/json");


    Serial.print("[HTTP] POST...\n");
    // start connection and send HTTP header and body
    // int httpCode = http.POST("{\"dispositivo_id\":\"1\"}");
    // int httpCode = http.POST("{\"dispositivo_id\":\"1\",\"temperatura\":\"10\"}");
    //{\"hello\":\"world\",\"temperatura\":\"10\"}
    // int httpCode = http.POST("{\"dispositivo_id\":\"1\",\"temperatura\":\"10\",\"humedad\":\"11\",\"presion_atmosferica\":\"12\",\"velocidad_viento\":\"13\"}");
    int httpCode = http.POST("{\"dispositivo_id\":\"1\",\"temperatura\":\"" + String(t, 2) + "\",\"humedad\":\"" + String(h, 2) + "\",\"presion_atmosferica\":\"12\",\"velocidad_viento\":\"13\"}");
    // {\"dispositivo_id\":\"1\",\"temperatura\":\"10\",\"humedad\":\"11\",\"presion_atmosferica\":\"12\",\"velocidad_viento\":\"13\"}

    // httpCode will be negative on error
    if (httpCode > 0) {
      // HTTP header has been send and Server response header has been handled
      Serial.printf("[HTTP] POST... code: %d\n", httpCode);

      // file found at server
      if (httpCode == HTTP_CODE_OK) {
        const String& payload = http.getString();
        Serial.println("received payload:\n<<");
        Serial.println(payload);
        Serial.println(">>");
      }
    } else {
      Serial.printf("[HTTP] POST... failed, error: %s\n", http.errorToString(httpCode).c_str());
    }

    http.end();
  }
   
    
}




void firmware_version()
{
  
  lcd.clear();
  delay(1000);
  lcd.createChar(1,housechar1);
  lcd.createChar(2,housechar2);
  lcd.createChar(3,housechar3);
  lcd.createChar(4,housechar4);
  lcd.setCursor(0,0);
  lcd.write(1);
  lcd.setCursor(0,1);
  lcd.write(2);
  lcd.setCursor(1,0);
  lcd.write(3);
  lcd.setCursor(1,1);
  lcd.write(4);
  lcd.setCursor(3,0);
  lcd.print("T-LAB TRONIC");
  lcd.setCursor(5,1);
  lcd.print("Ver 1.1");
  delay(5000);
   
}
