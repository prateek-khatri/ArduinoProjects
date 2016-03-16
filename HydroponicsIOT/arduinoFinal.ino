//********************
//PIN DEFINITIONS
//********************
//RELAY PINS
#define RELAY_PH_ACID_PUMP 4
#define RELAY_PH_BASE_PUMP 5
#define RELAY_EC_PUMP_ONE 6
#define RELAY_EC_PUMP_TWO 7
#define RELAY_WATER_PUMP 8
#define RELAY_LIGHT_SWITCH 9

//SENSOR PINS
#define EC_SENSOR_PIN_ONE 2
#define EC_SENSOR_PIN_TWO 3
#define SOIL_MOISTURE_SENSOR_PIN A3
#define LIGHT_SENSOR_PIN A2
#define SERIAL_RX 13
#define SERIAL_TX 12

//********************
//GLOBAL DEFINITIONS
//********************
//SENSOR VALUES
int soil_moisture;
int light_intensity;
float pHValue;
float ecValue;
String requestStringThresholds[6];

//THRESHOLDS
int soil_moisture_min;
int light_intensity_min;
float pH_min;
float pH_max;
float ec_min;
float ec_max;

//ACTUATOR STATE
bool actuatorState;

//DELTA VALUES (To be set manually depending on data frequency required)
#define SOIL_MOISTURE_DELTA 5 //(PERCENT)
#define LIGHT_INTENSITY_DELTA 5 //(PERCENT)
#define PH_DELTA 0.25 //pH scale
#define EC_DELTA 0.1 //ec scale


#include "Wire.h"
#define pHtoI2C 0x48
#define T 273.15     
#include <SoftwareSerial.h>
SoftwareSerial ESPserial(SERIAL_RX, SERIAL_TX); // RX | TX

void showInitSerialMessage()
{
  Serial.println("Serial Ports Init, System Running");
  Serial.println("");
}

void requestThresholds()
{
  //communicate with ESP - > wait for timeout
  ESPserial.println("1"); // If ESP receives 1 then boot
  Serial.println("Waiting for Thresholds...");
  delay(10000);
  if(!ESPserial.available())
  {
    Serial.println("Waiting 1 min for response...");
    delay(60000);//Boot time delay
  }
  int i=0;
  while( ESPserial.available() )   
  {
      char a = ESPserial.read();
      if(a==',')
      {
        i++;
      }
      requestStringThresholds[i] = requestStringThresholds[i]+a;  
      if(!ESPserial.available())
      {
        Serial.println();
        Serial.flush();
        ESPserial.flush();
      }
  }

}
void setNewThresholds(String req[])
{
  //Parse String here
  /*
   * 0 - ph min
   * 1 - ph max
   * 2 - ec min
   * 3 - ec max
   * 4 - soil moisture level
   * 5 - light moisture level
   */
   pH_min = req[0].toFloat();
   pH_max = req[1].toFloat();
   ec_min = req[2].toFloat();
   ec_max = req[3].toFloat();
   soil_moisture_min = req[4].toInt();
   light_intensity_min = req[5].toInt();
  
  
  
}
void setPinModes()
{
  actuatorState = false;
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




bool matchDeltas()
{
  bool eCState = (abs(ecValue - scanEcValue()) > EC_DELTA);
  bool pHState = (abs(pHValue - scanpHValue()) > PH_DELTA);
  bool soilState = (abs(soil_moisture - scanSoilMoisture()) > SOIL_MOISTURE_DELTA);
  bool lightState = (abs(light_intensity - scanLightIntensity()) > LIGHT_INTENSITY_DELTA);
  if(eCState || pHState || soilState || lightState)
  {
    return true;
  }
  return false;
  
}

bool matchThresholds()
{
  return false;
}

void sendUpdate()
{
  
}

void activateActuators()
{
  
}

int scanSoilMoisture()
{
  float a =(float) analogRead(SOIL_MOISTURE_SENSOR_PIN); 
  a = a*100;
  a = a/1023;
  return (int)a;
}
int scanLightIntensity()
{
  float a =(float) analogRead(LIGHT_SENSOR_PIN); 
  a = a*100;
  a = a/1023;
  return (int)a;
}

float scanpHValue()
{
  return 0.0;
}
float scanEcValue()
{
  return 0.0;
}

void setup() 
{
   //INIT SERIAL PORTS
   ESPserial.begin(115200);
   ESPserial.flush();
   Serial.begin(9600);
   Serial.flush();
   showInitSerialMessage();
   //SEND THRESHOLD REQUESTS -: THIS FUNCTION WILL WAIT FOR A REPLY OR KEEP SENDING REQUEST
   
   int j;
   for(j=0;j<6;j++)
   {
    requestStringThresholds[j] = "";
   }
   
   while(requestStringThresholds[0].equals(""))
   {
    requestThresholds();
   }
   //SET THE THRESHOLDS TO WORK WITH
   setNewThresholds(requestStringThresholds);
   //SET PIN MODES
   setPinModes();
    
}
void loop() 
{
  if(matchDeltas() == true)
  {
    sendUpdate();
  }
  if(matchThresholds() == true)
  {
    activateActuators();
  }
  
}
