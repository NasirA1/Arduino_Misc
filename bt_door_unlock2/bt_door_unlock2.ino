#include <Servo.h>
#include <LowPower.h>
#include <PinChangeInt.h>


#define SERIAL_RX_PIN 0
#define SERIAL_BAUD 9600

//servo motor
Servo servo;
const int servoNeutral = 90;
const int servoPin = 3;

//button
const int buttonPin = 2;

//Lock parameters
const String PIN = "----";
String input = "";
bool locked = true;

//battery
const int batLedPin = 11;
unsigned long batLedTimer = 0;
bool batLedState = false;
const float lowBatThreshold = 1.5; //3v

//commands
//s: status
//c: control[PIN]


void WakeHandler()
{
  // Nothing to do; just wakes the device. 
}


/***************************************************
 *  Initialisation           
 ***************************************************/
void setup()
{
  Serial.begin(SERIAL_BAUD);
  pinMode(batLedPin, OUTPUT);
  pinMode(buttonPin, INPUT);
  digitalWrite(batLedPin, LOW);
}


/***************************************************
 *  Low battery indicator
 *  Returns true when Arduino battery is low 
 ***************************************************/
bool batteryLow()
{
  //TODO
  return true;
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
 *  Go to sleep to preserve power
 ***************************************************/
void sleepMode()
{
  // Enable the pin change interrupt on the receive pin 
  // so that serial activity will wake the device.
  pinMode(SERIAL_RX_PIN, INPUT_PULLUP);
  PCintPort::attachInterrupt(SERIAL_RX_PIN, &WakeHandler, LOW);
  PCintPort::attachInterrupt(buttonPin, &WakeHandler, LOW);
  Serial.print("AT+SLEEP");
  delay(500);
  LowPower.powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF);  

  //Wakeup here...  
  // Detach pinchange interrupts & reconfigure serial port. 
  PCintPort::detachInterrupt(SERIAL_RX_PIN);
  PCintPort::detachInterrupt(buttonPin);
  Serial.begin(SERIAL_BAUD);
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
  
  int buttonReading = digitalRead(buttonPin);
    
  //Handle input and proccess..
  if(Serial.available() > 0 || buttonReading == HIGH)
  {
    input = buttonReading == HIGH? PIN: Serial.readString();
        
    if(input == PIN) //control command (open/close)
    {
      int rotationDir;
      if(locked)
      {
        rotationDir = 0;
        Serial.print("Unlocking");
      }
      else
      {
        rotationDir = 180;
        Serial.print("Locking");    
      }
      
      servo.attach(servoPin);
      for(int i = 0; i < 27000; i++)
      {
        servo.write(rotationDir);
      }
      
      servo.write(servoNeutral);
      servo.detach();
      locked = !locked;
      
      if(buttonReading == HIGH)
      {
        sleepMode();
      }
    }
    else if(input == "s") //status command
    {
      if(locked) Serial.print("1");
      else Serial.print("0");
    }
    else if(input == "OK+LOST") //client disconnected
    {
      sleepMode();
    }
  }
}

