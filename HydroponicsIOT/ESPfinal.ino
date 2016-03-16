#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
ESP8266WiFiMulti WiFiMulti;

//GLOBAL DEFINITIONS
#define SERIAL_RX 2
#define SERIAL_TX 3

//SENSOR VALUES
int soil_moisture;
int light_intensity;
float pHValue;
float ecValue;

//THRESHOLDS
int soil_moisture_min;
int light_intensity_min;
float pH_min;
float pH_max;
float ec_min;
float ec_max;

#include <SoftwareSerial.h>
SoftwareSerial ESPserial(2, 3); // RX | TX

void setup() {

  Serial.begin(115200);
  ESPserial.begin(115200);
  Serial.println("Init Serial Ports done...");


}

void loop() {
    
}
