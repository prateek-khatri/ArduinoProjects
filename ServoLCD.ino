/* RS: Pin 7
   EN: Pin 6
   D4: Pin 5
   D5: Pin 4
   D6: Pin 3
   D7: Pin 2
   */
//Include LCD Library
#include <LiquidCrystal.h>
#include <Servo.h>

// Initialize LCD
LiquidCrystal lcd(7,6,5,4,3,2);
Servo myservo;

void setup()
{
  //Begin LCD Interface
  lcd.begin(16,2);
  myservo.attach(11);
  lcd.print("                ");
  lcd.setCursor(0,1);
  lcd.print("                ");
  lcd.setCursor(0,0);

}
void loop()
{
  char a[10];
  lcd.setCursor(0,1);
  lcd.print("                ");
  lcd.setCursor(0,0);
  for(int i=0;i<=180;i=i+20)
  {
    myservo.write(i);
    lcd.write("Servo Angle: ");
    lcd.setCursor(0,1);
    itoa(i,a,10);
   // lcd.print("                ");
    lcd.print(a);
    lcd.setCursor(0,0);
    delay(1000);
  }
}
