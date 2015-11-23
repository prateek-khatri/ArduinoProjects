int jobSize = 100;
void setup()
{
  Serial.begin(9600);
  pinMode(13,OUTPUT);
  digitalWrite(13,LOW);
  while(!(Serial.available() >0));
  char algo = Serial.read();
  if(algo == '1') Serial.println(jobSize);
  while(!(Serial.available() >0));
  if(Serial.read() == 'g')
  {
    return;
  }
  
}

void waitForStart()
{
  Serial.println("Waiting for Start");
  while(!(Serial.available() > 0));
  if(Serial.read() == 'g')
  return;
}

void signalPolling()
{
  if(Serial.available() > 0)
  {
    if(Serial.read() == 's')
    {
      Serial.println("Stopped");
      waitForStart();
    }
  }
  else return;
  
}

void loop()
{
  
    digitalWrite(13,HIGH);
    signalPolling();
    delay(500);
    signalPolling();
    digitalWrite(13,LOW);
    signalPolling();
    delay(500);
    signalPolling();
  
 
 
}
