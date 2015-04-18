#include <LiquidCrystal.h>

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);



const int LedPin = 13;
const int SensorPin = A0;
const int TmpPin = A1;

const int RLedPin = 10;
const int BLedPin = 9;
const int GLedPin = 6;

const int BUFFER_SIZE = 10;
float buffer[BUFFER_SIZE];
int bufferIndex;
float totalTmp;



void bufferReset()
{
  bufferIndex = 0;
  totalTmp = 0.0;
  
  for(int i = 0; i < BUFFER_SIZE; ++i)
  {
    buffer[i] = 0.0;
  }
}


void bufferWrite(const float value)
{
  bufferIndex++;
  
  if(bufferIndex < BUFFER_SIZE)
  {
    buffer[bufferIndex] = value;
    totalTmp += value;
  }
  else
  {
    for(int i = 1; i < (BUFFER_SIZE - 1); ++i)
    {
      buffer[i - 1] = buffer[i];
    }
    bufferIndex = BUFFER_SIZE - 1;
    buffer[bufferIndex] = value;
    
    totalTmp = 0.0;    
    for(int i = 0; i < BUFFER_SIZE; ++i)
    {
      totalTmp += buffer[i];
    }
  }
}


//converting from 10 mv per degree wit 500 mV offset,   //to degrees ((voltage - 500mV) times 100)
float getTemperature(const int reading)
{
  float voltage = (reading / 1024.0) * 5.0;   
  float temperatureC = (voltage - 0.5) * 100 ;
  return temperatureC;
}


void setup() 
{
  pinMode(SensorPin, INPUT);
  pinMode(TmpPin, INPUT);
  pinMode(LedPin, OUTPUT);
  pinMode(RLedPin, OUTPUT);
  pinMode(GLedPin, OUTPUT);
  pinMode(BLedPin, OUTPUT);  
  bufferReset();
  
  // number of columns and rows on the LCD
  lcd.begin(16, 2);  
  lcd.setCursor(0, 0);
  lcd.clear();
  lcd.print("LOADING...");
  digitalWrite(LedPin, HIGH);
  for(int i = 0; i < BUFFER_SIZE; ++i)
  {
    int reading = analogRead(TmpPin);
    float tmp = getTemperature(reading);
    bufferWrite(tmp);
    delay(500);
  }  
  digitalWrite(LedPin, LOW);
  
  digitalWrite(RLedPin, HIGH);
  digitalWrite(GLedPin, HIGH);
  digitalWrite(BLedPin, HIGH);
}


void loop() 
{
  int light = analogRead(SensorPin);
  int reading = analogRead(TmpPin);
  float tmp = getTemperature(reading);
  bufferWrite(tmp);
  float avg = totalTmp / (float)BUFFER_SIZE;
  
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("TMP: ");
  lcd.print(avg);
  lcd.print("C");
  lcd.setCursor(0, 1);
  lcd.print("LIGHT: ");
  lcd.print(light);
  delay(500);
}

