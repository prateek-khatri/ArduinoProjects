#include <ArduinoJson.h>
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
ESP8266WiFiMulti WiFiMulti;

//GLOBAL DEFINITIONS
#define SERIAL_RX 2
#define SERIAL_TX 3
// WiFi Connection

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

void updatePayloadOnArduino()
{
  ESPserial.flush();
  //{"tdsUpperThresholds":0,"lightLowerThresholds":0,"tdsLowerThresholds":0,"moistureLowerThresholds":0,"phLowerThresholds":0,"phUpperThresholds":0}
  StaticJsonBuffer<200> jsonBuffer;
  char payloadArray[200]; 
  payload.toCharArray(payloadArray,200);
  JsonObject& root = jsonBuffer.parseObject(payloadArray);
//  if(!root.success())
//  {
//    Serial.println("Json Conversion Failed"); 
//    return;
//  }
  soil_moisture_min = root["moistureLowerThresholds"];
  light_intensity_min = root["lightLowerThresholds"];
  pH_min = root["phUpperThresholds"];
  pH_max = root["phLowerThresholds"];
  ec_min = root["tdsLowerThresholds"];
  ec_max = root["tdsUpperThresholds"];
  String responseThresholds = ""+String(pH_min)+","+String(pH_max)+","+String(ec_min)+","+String(ec_max)+","+String(soil_moisture_min)+","+String(light_intensity_min);
  ESPserial.print(responseThresholds);
  ESPserial.flush();
}

void sendValues(float ph, float ec, int soil, int light)
{
    bool flag = false;
    Serial.println("Updating Values on Server...");
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
        http.begin("172.20.10.1", 8080,"?&fn=getSensorDetails&sensorId=esp001&ph="+String(ph,2)+"&tds="+String(ec,2)+"&light="+String(light)+"&moisture="+String(soil)+"&boot=false");
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
            //payload.replace("\n","");
            //payload.replace(" ","");
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
    updatePayloadOnArduino();
  
}

void sendInitToServer()
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
        http.begin("172.20.10.1", 8080,"?&fn=getSensorDetails&sensorId=esp001&ph=7.2&tds=50&light=15.3&moisture=15.99&boot=true");
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
            //payload.replace("\n","");
            //payload.replace(" ","");
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
   ESPserial.flush();
   while(!(ESPserial.available() > 0))
   {
    delay(1000);
    Serial.print(".");
   }
   Serial.println();
   while(ESPserial.available() > 0)
   {
    char a = ESPserial.read();
    if(a=='1')
    {
      Serial.printf("Received Code: %c\n",a);
      sendInitToServer();
      break;
    }
    delay(500);
   }
   updatePayloadOnArduino();
}

void deltaViolationUpdate(String payload)
{
  ESPserial.flush();
  String sensorVals[4];
  int i=0;
  int val = 0;
  for(i=0;i<payload.length();i++)
  {
    if(payload[i] == ',') val++;
    sensorVals[val] += payload[i];
  }
  pHValue = sensorVals[0].toFloat();
  ecValue = sensorVals[1].toFloat();
  soil_moisture = sensorVals[2].toFloat();
  light_intensity = sensorVals[3].toFloat();

  sendValues(pHValue,ecValue,soil_moisture,light_intensity);
  
}

void thresholdViolationUpdate(String payload)
{
  ESPserial.flush();
  ESPserial.flush();
  String sensorVals[4];
  int i=0;
  int val = 0;
  for(i=0;i<payload.length();i++)
  {
    if(payload[i] == ',') val++;
    sensorVals[val] += payload[i];
  }
  pHValue = sensorVals[0].toFloat();
  ecValue = sensorVals[1].toFloat();
  soil_moisture = sensorVals[2].toFloat();
  light_intensity = sensorVals[3].toFloat();

  sendValues(pHValue,ecValue,soil_moisture,light_intensity);
  
}

void sendActuatorUpdate(bool ph, bool ec, bool light, bool water)
{
   bool flag = false;
    Serial.println("Updating Actuator Status on Server...");
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
        http.begin("172.20.10.1", 8080,"?&fn=updateActuatorData&sensorId=esp001&ph="+String(ph)+"&tds="+String(ec)+"&light="+String(light)+"&moisture="+String(water)+"&boot=false");
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
            //payload.replace("\n","");
            //payload.replace(" ","");
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
    updatePayloadOnArduino();
  
}

void actuatorStatusUpdate(String payload)
{
  ESPserial.flush();
  ESPserial.flush();
  String sensorVals[4];
  int i=0;
  int val = 0;
  for(i=0;i<payload.length();i++)
  {
    if(payload[i] == ',') val++;
    sensorVals[val] += payload[i];
  }
  //0 false
  // 1 true
  phActuator = sensorVals[0].toInt();
  ecActuator = sensorVals[1].toInt();
  lightActuator = sensorVals[2].toInt();
  waterActuator = sensorVals[3].toInt();

  sendActuatorUpdate(phActuator,ecActuator,lightActuator,waterActuator);
  
}
/*
 * Code 0: Normal Delta Update
 * Code 2: Threshold Violation, need to follow with actuator status
 * Code 9: Actuator status string
 */

void waitForSensorUpdates()
{
  while(!(ESPserial.available() >0))
  {
    delay(1000);
  }
  String response = "";
  char a;
  while(ESPserial.available() >0)
  {
    a = ESPserial.read();
    if(a == '0' || a == '2' || a=='9')
    {
      ESPserial.read(); //remove comma
      break;
    }
  }
  payload = "";
  while(ESPserial.available() >0)
  {
   char b = ESPserial.read();
    payload +=b;
  }
  if(a=='0')
  {
    deltaViolationUpdate(payload);
  }
  if(a=='2')
  {
    thresholdViolationUpdate(payload);
  }
  if(a=='9')
  {
    actuatorStatusUpdate(payload);
  }
}

void loop() 
{
    waitForSensorUpdates();
    delay(1000);
}
