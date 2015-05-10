#include <Servo.h>

Servo servo;
const int servoNeutral = 90;
const int servoPin = 3;

const int batLedPin = 13;
const int statLedPin = 12;

//Battery usage
//Source: https://autonomoushobbyist.wordpress.com/2011/02/24/arduino-battery-meter/
//voltage divider for the battery level indicator via 2x220ohm resistors
#define Z1 220.0
#define Z2 220.0

unsigned long batLedTimer = 0;
bool batLedState = false;
const float lowBatThreshold = 2.0; //4v

//Authentication
const int UnlockPIN = 1234;
const int LockPIN = 0000;
int userPin = 0;

void setup() 
{
  pinMode(batLedPin, OUTPUT);
  pinMode(statLedPin, OUTPUT);
  digitalWrite(batLedPin, LOW);
  digitalWrite(statLedPin, LOW);  
  Serial.begin(9600);
}


bool batteryLow()
{
  float batteryReading = analogRead(A0);
  float vout = batteryReading * (5.0 / 1023.0);
  //Now that we have our Vout, Z1, and Z2 we are ready to use that to get the input voltage. 
  //Using the voltage divider formula we get:
  float vin = (Z1 + Z2) / (Z2) * vout;
  //Serial.println(vin);
  //delay(1000);
  return vin <= lowBatThreshold;
}


void blinkBatLed()
{
  unsigned long now = millis();
  if(now > batLedTimer)
  {
    batLedState = !batLedState;
    batLedTimer = now + 1000;
  }
  digitalWrite(batLedPin, batLedState);
}


void loop() 
{
  //Handle battery indicator
  if(batteryLow())
  {
    blinkBatLed();
  }
  else
  {
    digitalWrite(batLedPin, HIGH);
  }
  
  //BT command input
  if(Serial.available() > 0)
  {
    digitalWrite(statLedPin, HIGH);
    userPin = Serial.parseInt();
    if(userPin == LockPIN || userPin == UnlockPIN)
    {
      const int rotationDir = userPin == UnlockPIN? 0: 180;
      Serial.write("Access Granted\n");
      servo.attach(servoPin);
      for(int i = 0; i < 27000; i++)
      {
        servo.write(rotationDir);
      }
      
      servo.write(servoNeutral);
      servo.detach();
    }
    else
    {
      Serial.write("Access Denied\n");
    }
  }
  
  digitalWrite(statLedPin, LOW);  
}
