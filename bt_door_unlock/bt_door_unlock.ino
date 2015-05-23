#include <Servo.h>
#include <avr/sleep.h>
#include <avr/power.h>

Servo servo;
const int servoNeutral = 90;

const int servoPin = 3;
const int batLedPin = 11;
const int statLedPin = 10;
const int batteryReadingPin = A0;
const int wakeUpPin = 2; //interrupt pin


//Battery usage
//Source: https://autonomoushobbyist.wordpress.com/2011/02/24/arduino-battery-meter/
//voltage divider for the battery level indicator via 2x220ohm resistors
#define Z1 220.0
#define Z2 220.0

unsigned long batLedTimer = 0;
bool batLedState = false;
const float lowBatThreshold = 2.5; //5v

//Lock parameters
const int PIN = ----;
int userPin = 0;
bool locked = true;
bool idle = true;

unsigned long awakeTimer;
const unsigned long awakePeriod = 1000;


/***************************************************
 *  Service routine for pin2 interrupt
 ***************************************************/
void pin2Interrupt(void)
{
  /* This will bring us back from sleep. */  
  /* We detach the interrupt to stop it from 
   * continuously firing while the interrupt pin
   * is low.
   */
  detachInterrupt(0);
}


/***************************************************
 *  Enters the arduino into sleep mode.
 ***************************************************/
void enterSleep(void)
{
  
  /* Setup pin2 as an interrupt and attach handler. */
  attachInterrupt(0, pin2Interrupt, LOW);
  delay(100);
  
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  
  sleep_enable();
  
  sleep_mode();
  
  /* The program will continue from here. */
  
  /* First thing to do is disable sleep. */
  sleep_disable(); 
}


/***************************************************
 *  Setup for the Arduino.           
 ***************************************************/
void setup() 
{
  Serial.begin(9600);
  pinMode(batteryReadingPin, INPUT);
  pinMode(batLedPin, OUTPUT);
  pinMode(statLedPin, OUTPUT);
  pinMode(wakeUpPin, INPUT);
  
  digitalWrite(batLedPin, LOW);
  digitalWrite(statLedPin, LOW);
  awakeTimer = millis() + awakePeriod;  
}


/***************************************************
 *  Low battery indicator
 *  Returns true when Arduino battery is low 
 ***************************************************/
bool batteryLow()
{
  float batteryReading = analogRead(batteryReadingPin);
  float vout = batteryReading * (5.0 / 1023.0);
  //Now that we have our Vout, Z1, and Z2 we are ready to use that to get the input voltage. 
  //Using the voltage divider formula we get:
  float vin = (Z1 + Z2) / (Z2) * vout;
  //Serial.println(vin);
  return vin <= lowBatThreshold;
}


/***************************************************
 *  Blinks low battery indicator LED
 ***************************************************/
void blinkBatLed()
{
  unsigned long now = millis();
  if(now > batLedTimer)
  {
    batLedState = !batLedState;
    batLedTimer = now + 999;
    digitalWrite(batLedPin, HIGH); //quick flash for 1 ms
    delay(1);
  }
  digitalWrite(batLedPin, LOW);
}


/***************************************************
 *  Main application loop
 ***************************************************/
void loop() 
{
  //Handle battery indicator
  if(batteryLow())
  {
    blinkBatLed();
  }
  
  //Handle input and proccess..
  if(Serial.available() > 0)
  {
    digitalWrite(statLedPin, HIGH);
    userPin = Serial.parseInt();
    
    if(userPin == PIN)
    {
      int rotationDir;
      if(locked)
      {
        rotationDir = 0;
        Serial.write("Unlocking.\n");
      }
      else
      {
        rotationDir = 180;
        Serial.write("Locking.\n");        
      }
      
      servo.attach(servoPin);
      for(int i = 0; i < 27000; i++)
      {
        servo.write(rotationDir);
      }
      
      servo.write(servoNeutral);
      servo.detach();
      locked = !locked;
    }
  }
  
  digitalWrite(statLedPin, LOW);


  //Go to sleep to preserve power
  if(millis() > awakeTimer)
  {  
    Serial.println("zZz.");
    delay(500);
    awakeTimer = millis() + awakePeriod;
    enterSleep();
  }
}


