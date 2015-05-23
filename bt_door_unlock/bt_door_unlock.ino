#include <Servo.h>
#include <avr/sleep.h>
#include <avr/power.h>

Servo servo;
const int servoNeutral = 90;

const int servoPin = 3;
const int batLedPin = 11;
const int statLedPin = 10;
const int batteryReadingPin = A0;


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


void sleepNow()
{
    /* Now is the time to set the sleep mode. In the Atmega8 datasheet
     * http://www.atmel.com/dyn/resources/prod_documents/doc2486.pdf on page 35
     * there is a list of sleep modes which explains which clocks and 
     * wake up sources are available in which sleep modus.
     *
     * In the avr/sleep.h file, the call names of these sleep modus are to be found:
     *
     * The 5 different modes are:
     *     SLEEP_MODE_IDLE         -the least power savings 
     *     SLEEP_MODE_ADC
     *     SLEEP_MODE_PWR_SAVE
     *     SLEEP_MODE_STANDBY
     *     SLEEP_MODE_PWR_DOWN     -the most power savings
     *
     *  the power reduction management <avr/power.h>  is described in 
     *  http://www.nongnu.org/avr-libc/user-manual/group__avr__power.html
     */  
     
  set_sleep_mode(SLEEP_MODE_IDLE);   // sleep mode is set here
 
  sleep_enable();          // enables the sleep bit in the mcucr register
                             // so sleep is possible. just a safety pin 
  
  power_adc_disable();
  power_spi_disable();
  power_timer0_disable();
  power_timer1_disable();
  power_timer2_disable();
  power_twi_disable();
  //power_all_disable();
  
  
  sleep_mode();            // here the device is actually put to sleep!!
 
                             // THE PROGRAM CONTINUES FROM HERE AFTER WAKING UP
  sleep_disable();         // first thing after waking from sleep:
                            // disable sleep...
 
  power_all_enable();   
}


void setup() 
{
  Serial.begin(9600);
  pinMode(batteryReadingPin, INPUT);
  pinMode(batLedPin, OUTPUT);
  pinMode(statLedPin, OUTPUT);
  
  digitalWrite(batLedPin, LOW);
  digitalWrite(statLedPin, LOW);  
}


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


void loop() 
{
  //Handle battery indicator
  if(batteryLow())
  {
    blinkBatLed();
  }
  
  //Handle input and proccess..
  if(idle && Serial.available() > 0)
  {
    idle = false;
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
  idle = true;
  sleepNow();  
}

