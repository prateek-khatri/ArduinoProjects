#include <AcceleroMMA7361.h>
#include <SD.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define rtc 0x68

int scale=0;
String c_scale;
String hh_scale;
int a=0;
int b=0;
int c=0;
int e=0;
const int chipSelect = 10;
AcceleroMMA7361 accelero;
int x=0;
int y=0;
int z=0;
int sample_count=0;
int led_pin=2;
int mx=0;
int my=0;
int mz=0;
int minx=1023;
int miny=1023;
int minz=1023;
String time_stamp;
String AMPM="";
int ss,mm,hh,d,DD,MM,YY,mode;
boolean time_format,meridiem;
String dataString;
LiquidCrystal_I2C lcd(0x27,16,2);  // LCD Address is 0x27, if converter jumpers connected then 0x20
int pot_val=0;
void setup()
{
 // Open serial communications and wait for port to open:
  Wire.begin(); //Initiate Wire control
  Serial.begin(9600);
  
  accelero.begin(7, 6, 5, 4, A2, A1, A0);
  accelero.setSensitivity(HIGH);                   //sets the sensitivity to +/-6G
  accelero.calibrate();
  lcd.init();
  lcd.noBacklight();
  lcd.clear();


  //Serial.print("Initializing SD card...");
  // make sure that the default chip select pin is set to
  // output, even if you don't use it:
  pinMode(10, OUTPUT);
  
  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    
    // don't do anything more:
    return;
  }
 
  
  File dataFile = SD.open("data.csv", FILE_WRITE);
  if (dataFile) {
    dataFile.println("Richter Scale,Date,Time");
    dataFile.close();
    
  }  
  pinMode(led_pin,OUTPUT);
  digitalWrite(led_pin,LOW);

}
void loop()
{
  
  dataString= "";
  time_stamp= "";
  scale=0.0;

  x=y=z=0;
  x=accelero.getXRaw();
  y=accelero.getYRaw();
  z=accelero.getZRaw();
  sample_count++;
  
  if(sample_count<=50)
  {
    if(x>mx)
      mx=x;
    if(y>my)
      my=y;
    if(z>mz)
      mz=z;
    if(x<minx)
      minx=x;
    if(y<miny)
      miny=y;
    if(z<minz)
      minz=z;
  }
  if(sample_count==50)
  {  
   pot_val=analogRead(A3);
   pot_val = map(pot_val, 0, 1023, 10, 25); 
    mx=mx+pot_val;
    my=my+pot_val;
    mz=mz+pot_val;
    minx=minx-pot_val;
    miny=miny-pot_val;
    minz=minz-pot_val;
    lcd.backlight();
    delay(10);
    lcd.noBacklight();
  }
  //Start of Earthquake Detection
  if(sample_count>=51)
  {
    
      int temp=0;
      if(x<=mx && x>=minx)
      {
        x=0;
        
      }
      if(y<=my && y>=miny)
      {
        y=0;  
        
      }
      if(z<=mz && z>=minz)
      {
        z=0;
        
      }
      if(x==0 && y==0 && z==0)
      {
        
        dataString="0,0,0,"+time_stamp;
      }
        
      if(x>mx)
      {
        temp=x-mx;
        x=temp;
        
        
      }
      else if(x<minx && x!=0)
      {
       temp=minx-x;
        x=temp;
        
      }
        
      if(y>my)
      {
        temp=y-my;
        y=temp;
        
      }
      else if(y<miny && y!=0)
      {
        temp=miny-y;
        y=temp;
        
      }
      
      if(z>mz && z!=0)
      {
        temp=z-mz;
        z=temp;
        
      }
      else if(z<minz && z!=0)
      {
        temp=minz-z;
        z=temp;
        
      }
        
  }
  if(sample_count>=51)
  {
    
    if(x==0 && y==0 && z==0)
    {
        digitalWrite(led_pin,LOW);
              
    }
    else if(x==0 && y==0 && z!=0)
    {
      digitalWrite(led_pin,HIGH);
      get_time();
      log_data();
    }
    else if(x==0 && y!=0 && z==0)
    {
      digitalWrite(led_pin,HIGH);
      get_time();
      log_data();
    }
    else if(x==0 && y!=0 && z!=0)
    {
      digitalWrite(led_pin,HIGH);
      get_time();
      log_data();
    }
    else if(x!=0 && y==0 && z==0)
    {
      digitalWrite(led_pin,HIGH);
      get_time();
      log_data();
    }
    else if(x!=0 && y==0 && z!=0)
    {
      digitalWrite(led_pin,HIGH);
      get_time();
      log_data();
    }
    else if(x!=0&& y!=0 && z==0)
    {
      digitalWrite(led_pin,HIGH);
      get_time();
      log_data();
    }
    else if(x!=0 && y!=0 && z!=0)
    {
      digitalWrite(led_pin,HIGH);
      get_time();
      log_data();
    }
    
    
  }
 // delay(30);
}
void calc_scale()
{
      a=scale/10;
      b=scale%10;
      if(a>9)
      {
        a=9;
        b=9;
      }
      c_scale=String(a)+"."+String(b);
      if(a>c || (a==c && b>e))
      {
        c=a;
        e=b;
      }
      if(c>9)
      {
        c=9;
        e=9;
      }
      hh_scale=String(c)+"."+String(e);
}

void scale_mag()
{
  if(x>=y && x>=z)
  {
    
    if(x<=10)
    { 
      
      scale=map(x,0,10,20,30);
      calc_scale();
      
    }
    else if(x<=30)
    {
      scale=map(x,11,30,30,40);
      calc_scale();
    }
    else if(x<=60)
    {
      scale=map(x,31,60,40,50);
      calc_scale();
    }
    else if(x<=100)
    {
      scale=map(x,61,100,50,60);
      calc_scale();
    }
    else if(x<=150)
    {
      scale=map(x,101,150,60,70);
      calc_scale();
    }
    else if(x<=200)
    {
      scale=map(x,151,200,70,80);
      calc_scale();
    }
    else if(x<=250)
    {
      scale=map(x,201,250,80,90);
      calc_scale();
    }
    else 
    {
      scale=map(x,251,300,90,100);
      calc_scale();
    }
  }
  
  else if(y>=x && y>=z)
  {
    
    if(y<=10)
    {
      scale=map(y,0,10,20,30);
      calc_scale();
    }
    else if(y<=30)
    {
      scale=map(y,11,30,30,40);
      calc_scale();
    }
    else if(y<=60)
    {
      scale=map(y,31,60,40,50);
      calc_scale();
    }
    else if(y<=100)
    {
      scale=map(y,61,100,50,60);
      calc_scale();
    }
    else if(y<=150)
    {
      scale=map(y,101,150,60,70);
      calc_scale();
    }
    else if(y<=200)
    {
      scale=map(y,151,200,70,80);
      calc_scale();
    }
    else if(y<=250)
    {
      scale=map(y,201,250,80,90);
      calc_scale();
    }
    else 
    {
      scale=map(y,251,500,90,100);
      calc_scale();
    }
  }
  
  else if(z>=y && z>=x)
  {
    
    if(z<=10)
    {
      scale=map(z,0,10,20,30);
      calc_scale();
    }
    else if(z<=30)
    {
      scale=map(z,11,30,30,40);
      calc_scale();
    }
    else if(z<=60)
    {
      scale=map(z,31,60,40,50);
      calc_scale();
    }
    else if(z<=100)
    {
      scale=map(z,61,100,50,60);
      calc_scale();
    }
    else if(z<=150)
    {
      scale=map(z,101,150,60,70);
      calc_scale();
    }
    else if(z<=200)
    {
      scale=map(z,151,200,70,80);
      calc_scale();
    }
    else if(z<=250)
    {
      scale=map(x,201,250,80,90);
      calc_scale();
    }
    else 
    {
      scale=map(z,251,300,90,100);
      calc_scale();
    }
  }
}
void log_data()
{
  scale_mag();
      
  lcd.setCursor(0,0);
  lcd.clear();
  lcd.backlight();
  lcd.setCursor(0,0);
  lcd.print("C: ");
  lcd.setCursor(3,0);
  lcd.print(c_scale);
  //Serial.println(c_scale);
  lcd.setCursor(7,0);
  lcd.print("H: ");
  lcd.setCursor(10,0);
  lcd.print(hh_scale);
  //Serial.println(hh_scale);
  lcd.setCursor(0,1);
  lcd.print(time_stamp);
  
  dataString=c_scale+","+time_stamp;
  File dataFile = SD.open("data.csv", FILE_WRITE);
    if (dataFile) 
    {
      
      dataFile.println(dataString);
      dataFile.close();
    }  
    else 
    {
      Serial.println("error opening datalog.txt");
    }
    
}

void get_time()
{
  Wire.beginTransmission(rtc);
  Wire.write((byte)0);
  Wire.endTransmission();

  Wire.requestFrom(rtc,7);
  ss=bcd_to_dec(Wire.read());
  mm=bcd_to_dec(Wire.read());
  hh=Wire.read();
  time_format=hh&(1<<6);   
  meridiem=hh&(1<<5);          // calculate am or pm
  hh=bcd_to_dec(hh&0x1F);
  d=bcd_to_dec(Wire.read());
  DD=bcd_to_dec(Wire.read());
  MM=bcd_to_dec(Wire.read());
  YY=bcd_to_dec(Wire.read());
  if(time_format)
  {
    if(meridiem)
      {
        AMPM="PM";
      }
      else
      {
        AMPM="AM";
      }
  } 
 // Serial.print(hh);Serial.print(" ");Serial.print(mm);Serial.print(" ");Serial.println(ss);  
  time_stamp=String(DD)+"/"+String(MM)+"/"+String(YY)+","+String(hh)+":"+String(mm)+":"+String(ss)+AMPM;
  Wire.endTransmission();
  
}
int bcd_to_dec(int bcd)
{
  return bcd/16*10 + (bcd%16);
}
