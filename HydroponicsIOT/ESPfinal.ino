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

//Actuator Status
bool phActuator;
bool ecActuator;
bool lightActuator;
bool waterActuator;

//Server Request Strings
String payload;

#include <SoftwareSerial.h>
SoftwareSerial ESPserial(SERIAL_RX, SERIAL_TX); // RX | TX

void sendInitToServer()
{
  bool flag = false;
    Serial.println("Fethching Thresholds from server");
    Serial.println("Waiting for WiFi connection...");
    while(flag == false)
    {
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
            flag = true;
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
}

void setup() 
{
  Serial.begin(115200);
  ESPserial.begin(115200);
  Serial.println("Init Serial Ports done...");
  for(uint8_t t = 4; t > 0; t--) 
  {
       Serial.printf("[SETUP] WAIT %d...\n", t);
       Serial.flush();
       delay(1000);
   }

    
   WiFiMulti.addAP("iPhone 6","parthconnect");
   Serial.println("Waiting for Controller Init...");
   while(!(ESPserial.available() > 0));
   while(ESPserial.available() > 0)
   {
    char a = ESPserial.read();
    if(a=='1')
    {
      sendInitToServer();
      break;
    }
   }
}

void loop() 
{
    
}