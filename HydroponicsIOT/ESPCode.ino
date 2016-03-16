#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>


#include <SoftwareSerial.h>
SoftwareSerial ESPserial(2, 3); // RX | TX
 
void setup() 
{
    Serial.begin(115200);     // communication with the host computer
    //while (!Serial)   { ; }
 
    // Start the software serial for communication with the ESP8266
    ESPserial.begin(115200);  
 
    Serial.println("");
    Serial.println("Remember to to set Both NL & CR in the serial monitor.");
    Serial.println("Ready");
    Serial.println("");    
}
 
void loop() 
{

    delay(400);
    // listen for communication from the ESP8266 and then write it to the serial monitor
    while( ESPserial.available() )   
    {
      char a = ESPserial.read();
      Serial.print(a);  
      if(!ESPserial.available())
      {
        Serial.println();
        Serial.flush();
        ESPserial.flush();
      }
    }
 
}
