void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  //INPUT PINS
  pinMode(2,INPUT);
  digitalWrite(2,HIGH);
  pinMode(3,INPUT);
  digitalWrite(3,HIGH);
  pinMode(4,INPUT);
  digitalWrite(4,HIGH);
  pinMode(5,INPUT);
  digitalWrite(5,HIGH);
  pinMode(6,INPUT);
  digitalWrite(6,HIGH);
  pinMode(7,INPUT);
  digitalWrite(7,HIGH);
  

  //outputPINS
  pinMode(A0,OUTPUT);
  pinMode(A1,OUTPUT);
  pinMode(A2,OUTPUT);
  pinMode(A3,OUTPUT);
  pinMode(A4,OUTPUT);
  pinMode(A5,OUTPUT);
  Serial.begin(9600);

}

void loop() {
  // put your main code here, to run repeatedly:
  int val = digitalRead(2);
  delay(100);
  digitalWrite(A0,val);
  val = digitalRead(3);
  delay(100);
  digitalWrite(A1,val);
  val = digitalRead(4);
  delay(100);
  digitalWrite(A2,val);
  val = digitalRead(5);
  delay(100);
  digitalWrite(A3,val);
  val = digitalRead(6);
  delay(100);
  digitalWrite(A4,val);
  val = digitalRead(7);
  delay(100);
  digitalWrite(A5,val);
  delay(100);

  Serial.println("Values:");
  Serial.println(digitalRead(2));
  Serial.println(digitalRead(3));
  Serial.println(digitalRead(4));
  Serial.println(digitalRead(5));
  Serial.println(digitalRead(6));
  Serial.println(digitalRead(7));
  delay(500);
}
