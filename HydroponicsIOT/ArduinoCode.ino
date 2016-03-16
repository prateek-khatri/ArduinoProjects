// Basic serial communication with ESP8266
// Uses serial monitor for communication with ESP8266
//
//  Pins
//  Arduino pin 2 (RX) to ESP8266 TX
//  Arduino pin 3 to voltage divider then to ESP8266 RX
//  Connect GND from the Arduiono to GND on the ESP8266
//  Pull ESP8266 CH_PD HIGH
//
// When a command is entered in to the serial monitor on the computer 
// the Arduino will relay it to the ESP8266
//
 
#include <SoftwareSerial.h>
SoftwareSerial ESPserial(8, 9); // RX | TX
 
void setup() 
{
    Serial.begin(115200);     // communication with the host computer
    //while (!Serial)   { ; }
 
    // Start the software serial for communication with the ESP8266
    ESPserial.begin(115200);  
 
    Serial.println("Init Serial Port");
    ESPserial.flush();
    
}
 
void loop() 
{
  delay(500);
  int a = analogRead(A0);
  ESPserial.print("Value of Sensor: ");
  ESPserial.println(a);
  ESPserial.flush();
   
}
