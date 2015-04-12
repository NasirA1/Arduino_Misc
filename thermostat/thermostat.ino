#include <LiquidCrystal.h>

//Thermostat
//Author:  Nasir Ahmad
//Version: 2015.04.12


// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

const int ContrastPin = A0;
const int TempPin = A1;
const int LedPin = 13;

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
    digitalWrite(LedPin, LOW);
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
    digitalWrite(LedPin, HIGH);
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
  pinMode(ContrastPin, OUTPUT);
  pinMode(TempPin, INPUT);
  pinMode(LedPin, OUTPUT);
  
  // number of columns and rows on the LCD
  lcd.begin(16, 2);  
  lcd.setCursor(0, 0);
  lcd.clear();
  digitalWrite(LedPin, HIGH);
  bufferReset();
}


void loop() 
{
  int reading = analogRead(TempPin);
  float tmp = getTemperature(reading);
  bufferWrite(tmp);
  lcd.setCursor(0, 0);
  float avg = totalTmp / (float)BUFFER_SIZE;
  lcd.print(avg);
  lcd.print("C");
  lcd.setCursor(0, 1);
  
  if(avg >= 20.0 && avg <= 25.0)
    lcd.print("Lovely Weather! ");
  else
    lcd.print("Shit Weather! :(");    
    
  delay(500);
}

