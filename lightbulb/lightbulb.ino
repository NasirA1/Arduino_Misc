#include <avr/interrupt.h>
#include <avr/power.h>
#include <avr/sleep.h>
#include <avr/io.h>

const int buttonPin = 2;
const int lightBulbPin = 13;

int lightBulbState = LOW;
int buttonState = LOW;
int lastButtonState = LOW;

long lastDebounceTime = 0;  // the last time the output pin was toggled
long debounceDelay = 50;    // the debounce time; increase if the output flickers


void setup() 
{
  pinMode(buttonPin, INPUT);
  pinMode(lightBulbPin, OUTPUT);

  digitalWrite(lightBulbPin, lightBulbState);
  Serial.begin(9600);
}


void loop() {
  int reading = digitalRead(buttonPin);

  // If the switch changed, due to noise or pressing:
  if (reading != lastButtonState) {
    // reset the debouncing timer
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    // whatever the reading is at, it's been there for longer
    // than the debounce delay, so take it as the actual current state:
    if (reading != buttonState) {
      buttonState = reading;
      if (buttonState == HIGH) {
        lightBulbState = !lightBulbState;
        if(lightBulbState) Serial.println("LED on.");
        else Serial.println("LED off.");
      }
    }
  }

  digitalWrite(lightBulbPin, lightBulbState);
  // save the reading.  Next time through the loop,
  // it'll be the lastButtonState:
  lastButtonState = reading;
}


