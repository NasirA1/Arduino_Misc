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

//batteries level measurement
//index 0: 9v battery to power the mcu
//index 1: 4x1.5v AA batteries to power the servo
const int batLedPin[2] = {11, 12};
const int batReadingPin[2] = {A0, A1};
unsigned long batLedTimer[2] = {0, 0};
bool batLedState[2] = {false, false};
const float lowBatThreshold[2] = {7.0, 4.0};
//voltage div resistors
const float R1[2] = {20000, 10000};
const float R2[2] = {10000, 10000};

//client connection state
bool clientConnected = false;
//commands
//s: status
//c: control[PIN]


float getBatteryVoltage(const int index)
{
  int reading = analogRead(batReadingPin[index]);
  float vOut = (float)reading * (3.3 / 1023.0);
  float vIn = (R1[index] + R2[index])/(R2[index]) * vOut;  
  return vIn;
}


/***************************************************
 *  Low battery indicator
 *  Returns true when battery is low 
 ***************************************************/
bool batteryLow(const int index)
{
  float vin = getBatteryVoltage(index);
//debug code  
//  if(index == 0)
//    Serial.print(vin);
//  else {
//    Serial.print(",");
//    Serial.println(vin);
//  }
  return vin < lowBatThreshold[index];  
}


/***************************************************
 *  Blinks low battery indicator LED
 ***************************************************/
void blinkBatLed(const int index)
{
  unsigned long now = millis();
  if(now > batLedTimer[index])
  {
    batLedState[index] = !batLedState[index];
    batLedTimer[index] = now + 999;
    digitalWrite(batLedPin[index], HIGH); //quick flash for 1 ms
    delay(1);
  }
  digitalWrite(batLedPin[index], LOW);
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
  if(!clientConnected) {
    Serial.print("AT+SLEEP");
    delay(500);
  }
  LowPower.powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF);  

  //Wakeup here...  
  // Detach pinchange interrupts & reconfigure serial port. 
  PCintPort::detachInterrupt(SERIAL_RX_PIN);
  PCintPort::detachInterrupt(buttonPin);
  Serial.begin(SERIAL_BAUD);
}


/***************************************************
 *  Wakeup handler        
 ***************************************************/
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
  pinMode(batLedPin[0], OUTPUT);
  pinMode(batLedPin[1], OUTPUT);
  pinMode(buttonPin, INPUT);
  digitalWrite(batLedPin[0], LOW);
  digitalWrite(batLedPin[1], LOW);
}


/***************************************************
 *  Main application loop
 ***************************************************/
void loop() 
{
  //Handle battery indicators
  if(batteryLow(0)){
    blinkBatLed(0);
  }
  if(batteryLow(1)){
    blinkBatLed(1);
  }
  
  int buttonReading = digitalRead(buttonPin);
    
  //Handle input and proccess..
  if(Serial.available() > 0 || buttonReading == HIGH)
  {
    if(buttonReading == HIGH){
      input = PIN;
    }
    else { 
      input = Serial.readString();
      clientConnected = true;
    }
        
    if(input == PIN) //control command (open/close)
    {
      int rotationDir;
      if(locked)
      {
        rotationDir = 0;
        if(clientConnected)
          Serial.print("Unlocking");
      }
      else
      {
        rotationDir = 180;
        if(clientConnected)
          Serial.print("Locking");    
      }
      
      servo.attach(servoPin);
      for(int i = 0; i < 13000; i++)
      {
        servo.write(rotationDir);
      }
      
      servo.write(servoNeutral);
      servo.detach();
      locked = !locked;
      
      if(buttonReading == HIGH && !clientConnected){
        sleepMode();
      }
    }
    else if(input == "s") //status command
    {
      if(locked) {
       if(clientConnected) 
          Serial.print("1");
      } else {
        if(clientConnected)
          Serial.print("0");
      }
    }
    else if(input == "OK+CONN")
    {
      clientConnected = true;
    }
    else if(input == "OK+LOST") //client disconnected
    {
      clientConnected = false;
      sleepMode();
    }
  }
}

