#include <Servo.h>

Servo myservo;
int pos;
#define trigPin 9
#define echoPin 8

int measure()
{
  int duration,distance;
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(1000);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance = (duration/2) / 29.1;
  Serial.println(distance);
  return distance;
}
  
void sweep()
{
  myservo.write(0);
  delay(1500);
  int distance = measure();
  
  if(distance>15)
  {
    myservo.write(90);
    delay(1500);
    turn_left();
  }
  else if(distance<=15 && distance >=0)
  {
    myservo.write(180);
    delay(1500);
    int dist=measure();
    if(dist>15)
    {
      myservo.write(90);
      delay(1500);
      turn_right();
    }
    else if(dist<=15 && dist>=0)
    {
      myservo.write(90);
      delay(1500);
      reverse();
    }
  }
      
    
    
}
void reverse()
{
  digitalWrite(2,HIGH);
  digitalWrite(3,LOW);
  digitalWrite(4,HIGH);
  digitalWrite(5,LOW);
  delay(3000);
  digitalWrite(2,LOW);
  digitalWrite(3,LOW);
  digitalWrite(4,LOW);
  digitalWrite(5,LOW);
  sweep();
}
void turn_right()
{
  digitalWrite(2,HIGH);
  digitalWrite(3,LOW);
  digitalWrite(4,LOW);
  digitalWrite(5,HIGH);
  delay(300);
}
void turn_left()
{
  digitalWrite(2,LOW);
  digitalWrite(3,HIGH);
  digitalWrite(4,HIGH);
  digitalWrite(5,LOW);
  delay(300);
}
void setup()
{
  Serial.begin(9600);
  //Define Sensor Pins
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  //Initialize Servo
  myservo.attach(11);
  myservo.write(90);
  //Define Motor Pins
  pinMode(2,OUTPUT);
  pinMode(3,OUTPUT);
  pinMode(4,OUTPUT);
  pinMode(5,OUTPUT);
  // Initialize Motor Forward Direction
  digitalWrite(2,LOW);
  digitalWrite(3,HIGH);
  digitalWrite(4,LOW);
  digitalWrite(5,HIGH);
}
void loop()
{
  int distance=measure();

  //Check if obstacle found
  if(distance>15)
  {
    digitalWrite(2,LOW);
    digitalWrite(3,HIGH);
    digitalWrite(4,LOW);
    digitalWrite(5,HIGH);
  }
  else if(distance<=15 && distance >=0)
  {
    digitalWrite(2,LOW);
    digitalWrite(3,LOW);
    digitalWrite(4,LOW);
    digitalWrite(5,LOW);
    sweep();
  }
  
} 
