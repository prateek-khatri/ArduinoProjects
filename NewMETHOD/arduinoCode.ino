#include<Arduino.h>
#include<SoftwareSerial.h>

#define SERIAL_RX 13
#define SERIAL_TX 12

//SENSOR PINS
#define EC_SENSOR_PIN_ONE 2
#define EC_SENSOR_PIN_TWO 3
#define SOIL_MOISTURE_SENSOR_PIN A1
#define LIGHT_SENSOR_PIN A2
#define SERIAL_RX 13
#define SERIAL_TX 12

//SENSOR VALUES
int soil_moisture;
int light_intensity;
float pHValue;
float ecValue;
String payload; //send this string to ESP ezpz

SoftwareSerial ESPSerial(SERIAL_RX,SERIAL_TX);

void initValues()
{
  soil_moisture = 0;
  light_intensity = 0;
  pHValue = 0;
  ecValue = 0;
  payload = "";
  
}
//********************SCANNING SENSORS*******************************
void scanEC()
{
  ecValue = 50.23;
}
void scanPH()
{
  pHValue = 7.56;
}
void scanLight()
{
  float a =(float) (analogRead(LIGHT_SENSOR_PIN)-255); 
  if(a<0) a=0;
  a = a*100;
  a = a/(1023-255);
  light_intensity = (int)a;
}
void scanMoisture()
{
  float a =(float) (analogRead(SOIL_MOISTURE_SENSOR_PIN-20)); 
  if(a<0) a=0;
  a = a*100;
  a = a/(1023-20);
  soil_moisture = (int)a;
}
//********************END SCANNING SECTION*******************************

//********************SEND VALUES TO ESP*******************************
void sendValues(float ph,float ec,int moisture, int light)
{
  Serial.println("Sending Values to ESP8266...");
  ESPSerial.print("6,"+String(ph)+","+String(ec)+","+String(moisture)+","+String(light));
  Serial.println("Done!");
  Serial.println();
}

void setup() 
{
  // put your setup code here, to run once:
  Serial.begin(9600);
  ESPSerial.begin(115200);
  Serial.println("System Init - Polling Module");
  Serial.println("Sending Init Code: 1 to ESP");
  ESPSerial.print("1");

  initValues();
  Serial.println("Values Initiatiated, starting scan...");
  Serial.println();
  delay(5000);

}

void loop()
{
  // put your main code here, to run repeatedly:
  Serial.println("Scanning TDS...");
  scanEC();
  Serial.print("TDS Value: ");Serial.println(ecValue);
  Serial.println();
  
  Serial.println("Scanning pH...");
  scanPH();
  Serial.print("pH Value(1-14): "); Serial.println(pHValue);
  Serial.println();
  
  Serial.println("Scanning Light Levels...");
  scanLight();
  Serial.print("Light Intensity(%): ");Serial.println(light_intensity);
  Serial.println();
  
  Serial.println("Scanning Moisture Levels...");
  scanMoisture();
  Serial.print("Moisture Level(%): ");Serial.println(soil_moisture);
  Serial.println();
  sendValues(pHValue,ecValue,soil_moisture,light_intensity);
  delay(10000);
}

