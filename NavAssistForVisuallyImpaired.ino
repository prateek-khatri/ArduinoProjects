#include <Servo.h>

//Global Servo Init
Servo motorA;
Servo motorB;
#define servoPinA  9
#define servoPinB  10

//Global Ultrasonic Sensor Init
#define trigA 2
#define echoA 3
#define trigB 5
#define echoB 6

// Constrain Variables
const int readingPerSensor = 5;
const int minRotation = 3; //Minimum Servo Rotation Quantum
const int servoDelay = 9;  //Delay before updating servo

// Value Variables
int readingArr[2][readingPerSensor]; // Data Matrix
int finalReading[2];                 // Smoothed Single Data from each sensor
int lastRead = 0;                    // Pointer for Array
int rotation[2];                     // Servo Rotation 
int minSensor = 200;
int maxSensor = 20000;
int servoPressure[2] = {0,160};
int servoRelease[2] = {70,110};
int servoLoc[2];

void setup()
{
  //Caliberation Pending after glove is made.
}

/**********************************************************
Function Name: takeReading
Purpose: Takes Raw Data from Sensor and scales it to 0-100%
***********************************************************/
int takeReading(int sensor)
{
  long duration;
  int reading;
  
  if(sensor==1)
  {
    digitalWrite(trigA,HIGH);
    delayMicroseconds(1000);
    digitalWrite(trigA,LOW);
    duration = pulseIn(echoA,HIGH);
    duration = constrain(duration, minSensor,maxSensor);
    reading = map(duration, minSensor,maxSensor, 0,100);
    return reading;
  }
  else if(sensor==2)
  {
    digitalWrite(trigB,HIGH);
    delayMicroseconds(1000);
    digitalWrite(trigB,LOW);
    duration = pulseIn(echoB,HIGH);
    duration = constrain(duration,minSensor,maxSensor);
    reading = map(duration, minSensor, maxSensor, 0,100);
    return reading;
  }  
}


/************************************************************
Function Name : calcDist
Purpose: Smooths out using Weighted Average
************************************************************/
int calcDist(int sensorNum)
{
  int readOut= maxSensor;
  
  float weightFactor = 0.5;
  float flickerFactor = 30;
  
  if(lastRead >= readingPerSensor-1)
  {
    int total =0;
    float currentWeight =1;
    float totalPercent = 0;
    boolean flickered = false;
    for(int i=readingPerSensor-1; i>=0;i--)
    {
      flickered = false;
      if(i==readingPerSensor -1)
      {
        if((abs(readingArr[sensorNum][i]) - abs(readingArr[sensorNum][i-1]) > flickerFactor) &&
           (abs(readingArr[sensorNum][i-1]) - abs(readingArr[sensorNum][i-2]) > flickerFactor))
           {
             flickered = true;
           }
      }
      if(flickered == false)
      {
        total +=(readingArr[sensorNum][i] * currentWeight);
        totalPercent += currentWeight;
        currentWeight *= weightFactor;
      }
    }
    readOut = total/totalPercent;
  }
  return readOut;
}

  
/**************************************************************
Function Name: Loop
Purpose: Main function adjusts the servo angle reading the data
**************************************************************/
void loop()
{
  unsigned long delayTime;
  int oldLoc;
  
  for(int i=0;i<2;i++)
  {
    readingArr[i][lastRead]= takeReading(i); // Get reading from sensor
    finalReading[i] = calcDist(i);
    
    oldLoc = servoLoc[i]; //Correct the Serov Angle
    servoLoc[i] = map(finalReading[i], 0,100,servoPressure[i],servoRelease[i]);    
    
    if(lastRead >= readingPerSensor - 1)
    {
      if(abs(servoLoc[i] - oldLoc) >= minRotation)
      {
        if(i==0)
        {
          motorA.attach(servoPinA);
          delay(10);
          motorA.write(servoLoc[i]);
          delayTime = (servoDelay *(abs(servoLoc[i] - oldLoc))+20); //Delay for Motor noise
          if(abs(delayTime)>500)
          {
            delayTime=500;
          }
          delay(delayTime);
          motorA.detach();
        }
        else if(i==1)
        {
          motorB.attach(servoPinB);
          delay(10);
          motorB.write(servoLoc[i]);
          delayTime = (servoDelay*(abs(servoLoc[i] - oldLoc))+20);
          if(abs(delayTime>500))
          {
            delayTime=500;
          }
          delay(delayTime);
          motorB.detach();
        }
      }
      else
      {
        if(i==0)
        {
          motorA.attach(servoPinA);
          delay(10);
          motorA.write(oldLoc);
          delay(50);
          motorA.detach();
          servoLoc[i]=oldLoc;
        }
        else if(i==1)
        {
          motorB.attach(servoPinB);
          delay(10);
          motorB.write(oldLoc);
          delay(50);
          motorB.detach();
          servoLoc[i] = oldLoc;
        }
      }
    }
    delay(20); // Noise Fix
  }
  lastRead = lastRead+1;
  if(lastRead >= readingPerSensor)
  {
    lastRead = readingPerSensor -1;
    
    //Pop Old Reading
    for(int i=0;i<2;i++)
    {
      for(int j=0; j<4; j++)
      {
        readingArr[i][j] = readingArr[i][j+1];
      }
    }
  }
}
//****************************************************
