#include <ArduinoJson.h>
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
ESP8266WiFiMulti WiFiMulti;

//GLOBAL DEFINITIONS
#define SERIAL_RX 2
#define SERIAL_TX 3
//RELAY PINS
#define RELAY_PH_ACID_PUMP 4
#define RELAY_PH_BASE_PUMP 5
#define RELAY_EC_PUMP_ONE 6
#define RELAY_EC_PUMP_TWO 7
#define RELAY_WATER_PUMP 8
#define RELAY_LIGHT_SWITCH 9

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


//Actuator Status
bool phActuator;
bool ecActuator;
bool lightActuator;
bool waterActuator;

//Server Request Strings
String payload;

#include <SoftwareSerial.h>
SoftwareSerial ESPSerial(SERIAL_RX, SERIAL_TX); // RX | TX
//**********************INIT FUNCTIONS**************************
void initValues()
{
  soil_moisture =0;
  light_intensity = 0;
  pHValue = 0;
  ecValue =0;
  phActuator = false;
  ecActuator = false;
  lightActuator = false;
  waterActuator = false; 
}
void initPins()
{
  pinMode(RELAY_PH_ACID_PUMP,OUTPUT);
  pinMode(RELAY_PH_BASE_PUMP,OUTPUT);
  pinMode(RELAY_EC_PUMP_ONE,OUTPUT);
  pinMode(RELAY_EC_PUMP_TWO,OUTPUT);
  pinMode(RELAY_WATER_PUMP,OUTPUT);
  pinMode(RELAY_LIGHT_SWITCH,OUTPUT);
  digitalWrite(RELAY_PH_ACID_PUMP,LOW);
  digitalWrite(RELAY_PH_BASE_PUMP,LOW);
  digitalWrite(RELAY_EC_PUMP_ONE,LOW);
  digitalWrite(RELAY_EC_PUMP_TWO,LOW);
  digitalWrite(RELAY_WATER_PUMP,LOW);
  digitalWrite(RELAY_LIGHT_SWITCH,LOW);
}
void initPeripherals()
{
  Serial.begin(115200);
  ESPSerial.begin(115200);
  Serial.println("System Init...Ports Initialized");
  Serial.println("Initializing Values...");
  initValues();
  Serial.println("Initializing Pins for Relay");
  initPins();
}

//**********************INIT FUNCTIONS END**************************

void setup() 
{
  initPeripherals();
  
}

void loop() 
{
  

}
