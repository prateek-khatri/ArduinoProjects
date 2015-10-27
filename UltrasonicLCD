#include <LiquidCrystal.h>

#define trigPin 9
#define echoPin 8

LiquidCrystal lcd(7, 6, 5, 4, 3, 2);

void setup() {
lcd.begin(16, 2);
lcd.print("Distance (cm):");
pinMode(trigPin, OUTPUT);
pinMode(echoPin, INPUT);
}

void loop() {
lcd.setCursor(0, 1);
int duration, distance;
digitalWrite(trigPin, HIGH);
delayMicroseconds(1000);
digitalWrite(trigPin, LOW);
duration = pulseIn(echoPin, HIGH);
distance = (duration/2) / 29.1;
//lcd.print("                ");
if(distance <=99 && distance >=10)
{
  lcd.print("                ");
  lcd.setCursor(0,1);
  lcd.print(distance);
}
else if(distance <=9 && distance >=0)
{
  lcd.print("                ");
  lcd.setCursor(0,1);
  lcd.print(distance);
}
else
{
  lcd.print("                ");
  lcd.setCursor(0,1);
  lcd.print(distance);
}
delay(500);
}
