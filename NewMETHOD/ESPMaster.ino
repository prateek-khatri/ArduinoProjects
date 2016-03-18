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
//**********************INIT FUNCTIONS*************************************
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

//**********************INIT FUNCTIONS END************************************

//**********************BOOT TIME REQUEST THRESHOLDS**************************
void updateThresholds() //THIS FUNCTION CAN BE CALLED WHEN PAYLOAD STRING HAS JSON OBJECT WITH THRESHOLDS
{
  StaticJsonBuffer<200> jsonBuffer;
  char payloadArray[200]; 
  payload.toCharArray(payloadArray,200);
  JsonObject& root = jsonBuffer.parseObject(payloadArray);
  soil_moisture_min = root["moistureLowerThresholds"];
  light_intensity_min = root["lightLowerThresholds"];
  pH_min = root["phUpperThresholds"];
  pH_max = root["phLowerThresholds"];
  ec_min = root["tdsLowerThresholds"];
  ec_max = root["tdsUpperThresholds"];
  String responseThresholds = ""+String(pH_min)+","+String(pH_max)+","+String(ec_min)+","+String(ec_max)+","+String(soil_moisture_min)+","+String(light_intensity_min);
  Serial.println("Thresholds Received :");
  Serial.println(responseThresholds);
}
void requestInitThresholds()
{
    bool flag = false;
    Serial.println("Fethching Thresholds from server");
    Serial.println("Waiting for WiFi connection...");
    while(flag == false)
    {
      Serial.println("Trying to connect...");
      if((WiFiMulti.run() == WL_CONNECTED)) 
      {
        Serial.println("WiFi Connection Established!");
        HTTPClient http;
        Serial.print("[HTTP] begin...\n");

        //configure server request
        http.begin("172.20.10.1", 8080,"?&fn=getSensorDetails&sensorId=esp001&ph=0&tds=0&light=0&moisture=0&boot=true");
        Serial.println("[HTTP] GET...");

        //send HTTP header
        int httpCode = http.GET();

        //httpCode will be negative on error
        if(httpCode >0)
        {
          Serial.print("[HTTP] GET... code ");Serial.println(httpCode);

          if(httpCode == HTTP_CODE_OK)
          {
            payload = http.getString();
            Serial.println("Received From Server:");
            Serial.println(payload);
                       
            if(payload.equals(""))
            {
              flag=false;
            }
            else
            {
              flag = true;
              break;
            }
          }
        }
        else
        {
          Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
        }
        http.end();
        
      }
      delay(10000);
    }

    updateThresholds();
  
}
//**********************BOOT TIME REQUEST THRESHOLDS END**************************

//**********************SENSOR READING ACQUISITION********************************
void printSensorValues()
{
  Serial.print("pH Value: ");Serial.println(pHValue);
  Serial.print("ec Value: ");Serial.println(ecValue);
  Serial.print("Moisture Value: ");Serial.println(soil_moisture);
  Serial.print("Light Value: ");Serial.println(light_intensity);
}
void parseSensorValues(String vals)
{
  pHValue = getValue(vals,",",0).toFloat();
  ecValue = getValue(vals,",",1).toFloat();
  soil_moisture = getValue(vals,",",2).toInt();
  light_intensity = getValue(vals,",",3).toInt();
  printSensorValues();
}
void readSensorValues()
{
  ESPSerial.flush();
  Serial.println("Waiting for Values from Arduino...");
  while(!(ESPSerial.available() >0))
  {
    delay(1000);
    Serial.print(".");
  }
  char a;
  String sensorRead = "";
  while(ESPSerial.available() > 0)
  {
    a = ESPSerial.read();
    if(a == '#')
    {
      ESPSerial.read(); //read comma
      break;
    }
  }
  while(ESPSerial.available() > 0)
  {
    a = ESPSerial.read();
    sensorRead +=a;
  }
  Serial.println();
  Serial.println("Values Received -> ");
  parseSensorValues(sensorRead);
}
//**********************SENSOR READING END********************************
void setup() 
{
  initPeripherals();
  requestInitThresholds();
}

void loop() 
{
 readSensorValues(); 

}
