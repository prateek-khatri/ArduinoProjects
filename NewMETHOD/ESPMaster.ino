#include <ArduinoJson.h>
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>

ESP8266WiFiMulti WiFiMulti;

//GLOBAL DEFINITIONS
#define SERIAL_RX 2
#define SERIAL_TX 6
//RELAY PINS
#define RELAY_PH_ACID_PUMP 0
#define RELAY_PH_BASE_PUMP 2
#define RELAY_EC_PUMP_ONE 4
#define RELAY_EC_PUMP_TWO 12
#define RELAY_WATER_PUMP 13
#define RELAY_LIGHT_SWITCH 15

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

//DELTAS
float ph_delta = 0.25;
float ec_delta = 10;
int light_delta = 3;
int moisture_delta = 5;

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
  Serial.println("Values/PINS Initialized!");
}

//**********************INIT FUNCTIONS END************************************

//**********************BOOT TIME REQUEST THRESHOLDS**************************
boolean updateThresholds() //THIS FUNCTION CAN BE CALLED WHEN PAYLOAD STRING HAS JSON OBJECT WITH THRESHOLDS
{
  Serial.println("Verifying Threshold Data...");
  StaticJsonBuffer<200> jsonBuffer;
  char payloadArray[200]; 
  payload.toCharArray(payloadArray,200);
  JsonObject& root = jsonBuffer.parseObject(payloadArray);
  int valid = root["valid"];
  if(valid == 0)
  {
    Serial.println("Invalid Thresholds...wait for Server...");
    delay(2000);
    return false;
  }
  soil_moisture_min = root["moistureLowerThresholds"];
  light_intensity_min = root["lightLowerThresholds"];
  pH_min = root["phUpperThresholds"];
  pH_max = root["phLowerThresholds"];
  ec_min = root["tdsLowerThresholds"];
  ec_max = root["tdsUpperThresholds"];
  String responseThresholds = ""+String(pH_min)+","+String(pH_max)+","+String(ec_min)+","+String(ec_max)+","+String(soil_moisture_min)+","+String(light_intensity_min);
  Serial.println("Thresholds Received :");
  Serial.println(responseThresholds);
  return true;
}
boolean requestInitThresholds()
{
    bool flag = false;
   
    Serial.println("System Boot...requestiong Server Data...");
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
            flag = true;
           }
         }
       
       else
       {
         Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
       }
      
       http.end();
       delay(2000); 
     }
    }
   flag = updateThresholds();
   return flag;
  

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
  String inputVals[4];
  for(int j=0;j<4;j++) inputVals[j] ="";
  int i=0;
  int index = 0;
  for(i=0;i<vals.length();i++)
  {
    if(vals[i] == ',')
    {
      index++;
      i++;
    }
    inputVals[index] += vals[i];
  }
  
  pHValue = inputVals[0].toFloat();
  ecValue = inputVals[1].toFloat();
  soil_moisture = inputVals[2].toInt();
  light_intensity = inputVals[3].toInt();
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


//**********************SENDING UPDATES TO SERVER*************************
boolean sendUpdate()
{
   bool flag = false;
   
    Serial.println("Sending Values to Server...");
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
        http.begin("172.20.10.1", 8080,"?&fn=getSensorDetails&sensorId=esp001&ph="+String(pHValue)+"&tds="+String(ecValue)+"&light="+String(light_intensity)+"&moisture="+String(soil_moisture)+"&boot=false");
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
     delay(2000); 
    }
   flag = updateThresholds();
   return flag;
  
}
boolean sendActuatorUpdate(bool ph,bool ec,bool light,bool moisture)
{
   bool flag = false;
   
    Serial.println("Sending Actuator Update to server");
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
        http.begin("172.20.10.1", 8080,"?&fn=updateActuatorData&sensorId=esp001&ph="+String(ph)+"&tds="+String(ec)+"&light="+String(light)+"&moisture="+String(moisture)+"&boot=false");
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
     delay(2000); 
    }
   flag = updateThresholds();
   return flag;
  
}

//**********************SENDING UPDATES TO SERVER END*********************

//**********************RELAY CONTROLS************************************
void activate_ph_pumps()
{
  if(pHValue < pH_min)
  {
    //activate base
    Serial.println("Starting Base Pump for PH!");
    digitalWrite(RELAY_PH_BASE_PUMP,HIGH);
    digitalWrite(RELAY_PH_ACID_PUMP,LOW);
  }
  else if(pHValue > pH_max)
  {
    //activate acid
    Serial.println("Starting Acid Pump for PH!");
    digitalWrite(RELAY_PH_BASE_PUMP,LOW);
    digitalWrite(RELAY_PH_ACID_PUMP,HIGH);
  }
  phActuator = true;
}
void deactivate_ph_pumps()
{
  Serial.println("Deactivate PH Pumps!");
  digitalWrite(RELAY_PH_BASE_PUMP,LOW);
  digitalWrite(RELAY_PH_ACID_PUMP,LOW);
  phActuator = false;
}

void activate_ec_pumps()
{
  if(ecValue < ec_min)
  {
    //activate base
    Serial.println("Starting Nutrient Pump for EC!");
    digitalWrite(RELAY_EC_PUMP_ONE,HIGH);
    digitalWrite(RELAY_EC_PUMP_TWO,LOW);
  }
  else if(ecValue > ec_max)
  {
    //activate acid
    Serial.println("Starting Water Pump for EC!");
    digitalWrite(RELAY_EC_PUMP_ONE,LOW);
    digitalWrite(RELAY_EC_PUMP_TWO,HIGH);
  }
  ecActuator = true;
}
void deactivate_ec_pumps()
{
    Serial.println("Deactivate EC Pumps!");
    digitalWrite(RELAY_EC_PUMP_ONE,LOW);
    digitalWrite(RELAY_EC_PUMP_TWO,HIGH);
    ecActuator = false;
}

void activate_light_switch()
{
  Serial.println("Turning Light On!");
  digitalWrite(RELAY_LIGHT_SWITCH,HIGH);
  lightActuator = true;
}
void deactivate_light_switch()
{
  Serial.println("Turning Light Off!");
  digitalWrite(RELAY_LIGHT_SWITCH,LOW);
  lightActuator = false;
}

void activate_water_pump()
{
  Serial.println("Turning Water Pump On!");
  digitalWrite(RELAY_WATER_PUMP,HIGH);
  waterActuator = true;
}
void deactivate_water_pump()
{
  Serial.println("Turning Water Pump off!");
  digitalWrite(RELAY_WATER_PUMP,LOW);
  waterActuator = false;
}
//**********************RELAY CONTROLS END********************************

//**********************MATCHING THRESHOLDS/DELTAS************************
void matchDeltas()
{
  Serial.println("Sending Delta Update...");
  float prev_ph = pHValue;
  float prev_ec = ecValue;
  int prev_light = light_intensity;
  int prev_moisture = soil_moisture;

  readSensorValues();
  if( (abs(prev_ph - pHValue) > ph_delta) || (abs(prev_ec - ecValue) > ec_delta) || (abs(prev_light - light_intensity) > light_delta) || (abs(prev_moisture - soil_moisture)>moisture_delta))
  {
    while(!sendUpdate())
   {
    delay(500);
   }
  }
}

void matchThresholds(bool ph,bool ec,bool light,bool moisture)
{
  //Send Updates Acquires Sensor Values, Sends to server and updates thresholds
  Serial.println("Threshold Violated!");
  while(!sendUpdate())
  {
    delay(500);
  }
  while(ph || ec || light || moisture)
  {
    /*
     * Activate: activate relevant actuator
     * Deactivate: deactivate all actuators relevant
     * both update actuator status
     */
    if(pHValue < pH_min || pHValue > pH_max)
    {
      ph = true;
      activate_ph_pumps();
    }
    else
    {
      ph =  false;
      deactivate_ph_pumps();
    }
  
    if(ecValue < ec_min || ecValue > ec_max)
    {
      ec = true;
      activate_ec_pumps();
    }
    else
    {
      ec = false;
      deactivate_ec_pumps();
    }
  
    if(light_intensity < light_intensity_min)
    {
      light = true;
      activate_light_switch();
    }
    else
    {
      light = false;
      deactivate_light_switch();
    }
  
    if(soil_moisture < soil_moisture_min)
    {
      moisture = true;
      activate_water_pump();
    }
    else
    {
      moisture = false;
      deactivate_water_pump();
    }
    while(!sendActuatorUpdate(ph,ec,light,moisture))
    {
      delay(500);
    }
    matchDeltas();
  }
}
void matchValues()
{
  Serial.println("Checking for Threshold Violations...");
  bool pHViolation = false;
  bool ecViolation = false;
  bool lightViolation = false;
  bool moistureViolation = false;
  if(pHValue < pH_min || pHValue > pH_max)
  {
    pHViolation = true;
  }
  else
  {
    pHViolation = false;
  }
  if(ecValue < ec_min || ecValue > ec_max)
  {
    ecViolation = true;
  }
  else
  {
    ecViolation = false;
  }
  if(light_intensity < light_intensity_min)
  {
    lightViolation = true;
  }
  else
  {
    lightViolation = false;
  }
  if(soil_moisture < soil_moisture_min)
  {
    moistureViolation = true;
  }
  else 
  {
    moistureViolation = false;
  }

  //CALL THRESHOLD MATCHING OR DELTA MATCHING
  if(pHViolation == true || ecViolation == true || lightViolation == true || moistureViolation == true)
  {
    matchThresholds(pHViolation, ecViolation,lightViolation,moistureViolation);
  }
  else
  {
    matchDeltas();
  }
}
//**********************MATCHING THRESHOLDS/DELTAS END********************

void setup() 
{
  initPeripherals();
  while(!requestInitThresholds());
  //don't go forwad until valid thresholds are received.  
}

void loop() 
{
  readSensorValues();
  /*
   * Match Thresholds, if true then
   * 1. send update to server - update thresholds
   * 2. activate respective actuators
   * 3. send actuator status - update thresholds
   * 4. readSensorValues until inside thresholds - send update at deltas
   * 5. deactivate actuators
   * 6. send actuator status - update thresholds
   * 
   * Match Deltas, if true then
   * 1. read sensor values
   * 2. send update
   * 3. update thresholds
   */
   matchValues();
}
