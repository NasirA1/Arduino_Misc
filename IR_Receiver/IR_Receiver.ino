/*
IR Sensor is: VS-1838B
  Wiring:
     |-----|
     |  \/ |
     |  /\ |
     |--|--|
     |  |  |
     |  |  |
     |  |  |
     S  G  5V
        N
        D
    
S: Signal - connected to pin3 of Arduino
GND: Ground
5V:  Vcc 5v+
*/


const int irPin = 3;
const int ledPin = 13;


void setup() 
{
  Serial.begin(9600);
  pinMode(irPin, INPUT);
  pinMode(ledPin, OUTPUT);
}



void loop() 
{
  //receive data and flash LED
  //when IR light is detected (sensor goes LOW)
  int reading = digitalRead(irPin);
  Serial.print(reading);
  
  if(reading == LOW)
  {
    digitalWrite(ledPin, HIGH);
  }
  else
  {
    digitalWrite(ledPin, LOW);
  }
}

