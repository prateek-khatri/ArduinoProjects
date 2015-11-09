/*
D7 - Pin 2
D6 - Pin 3
D5 - Pin 4
D4 - Pin 5
Enable - Pin 6
R/S - Pin  7
*/

#include <LiquidCrystal.h>

LiquidCrystal lcd(7,6,5,4,3,2);
int i=0;
int j=16;

void setup()
{
  
  // LCD Interface
  lcd.begin(16,2);
  
  //Print Something
  lcd.setCursor(0,0);
  lcd.print("TESSERACT");
  lcd.setCursor(0,1);
  lcd.print("GDGWI - HDWKA?");
  
}

void loop()
{
 
    delay(50);
    lcd.print("                ");
    lcd.setCursor(i,0);
   
    lcd.print("TESSERACT");
    if(i!=0)
    {
    lcd.setCursor(i-1,0);
    lcd.print(" ");
    }
    i++;
    delay(200);
    lcd.setCursor(0,1);
    lcd.print("                ");
  
  
    
    delay(50);
    lcd.print("                ");
    lcd.setCursor(j,1);
   
    lcd.print("GDGWI - HDWKA?");
    if(j!=19)
    {
    lcd.setCursor(j+17,1);
    lcd.print(" ");
    }
    j--;
    delay(200);
    lcd.setCursor(0,0);
    lcd.print("                ");
    
    if(i==16 && j==0)
    {
      j=16;i=0;
    }
    
  }
  
