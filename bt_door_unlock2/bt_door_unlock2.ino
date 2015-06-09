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
const int lowBatThreshold = 330; //3.3v

bool clientConnected = false;

//commands
//s: status
//c: control[PIN]



const long InternalReferenceVoltage = 1080;  // Adjust this value to your board's specific internal BG voltage
 
// Code courtesy of "Coding Badly" and "Retrolefty" from the Arduino forum
// results are Vcc * 100
// So for example, 5V would be 500.
int getBandgap () 
{
  // REFS0 : Selects AVcc external reference
  // MUX3 MUX2 MUX1 : Selects 1.1V (VBG)  
   ADMUX = bit (REFS0) | bit (MUX3) | bit (MUX2) | bit (MUX1);
   ADCSRA |= bit( ADSC );  // start conversion
   while (ADCSRA & bit (ADSC)){ }  // wait for conversion to complete
   int results = (((InternalReferenceVoltage * 1024) / ADC) + 5) / 10; 
   return results;
} // end of getBandgap


/***************************************************
 *  Low battery indicator
 *  Returns true when Arduino battery is low 
 ***************************************************/
bool batteryLow()
{
  return getBandgap() < lowBatThreshold;
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
  pinMode(batLedPin, OUTPUT);
  pinMode(buttonPin, INPUT);
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
      for(int i = 0; i < 27000; i++)
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

