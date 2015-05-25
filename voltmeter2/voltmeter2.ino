//const int batLedPin = 12;
//
////Battery usage
////Source: https://autonomoushobbyist.wordpress.com/2011/02/24/arduino-battery-meter/
//// Establish Resitance
//#define Z1 220.0
//#define Z2 220.0
//
////battery status
//
//
//void setup() 
//{
//  pinMode(batLedPin, OUTPUT);
//  digitalWrite(batLedPin, LOW);
//  Serial.begin(9600);
//}
//
//
//void batteryUsage()
//{
//  float batteryReading = analogRead(A0);
//  float vout = batteryReading * (5.0 / 1023.0);
//  //Now that we have our Vout, Z1, and Z2 we are ready to use that to get the input voltage. Using the voltage divider formula we get:
//  float vin = (Z1 + Z2) / (Z2) * vout;
//  
//  Serial.print("batteryReading:");
//  Serial.print(batteryReading);
//  Serial.print(", vin:");
//  Serial.print(vin);
//  Serial.print(", vout:");
//  Serial.println(vout);
//  
//  if(vin < 5.0)
//  {
//    digitalWrite(batLedPin, HIGH);
//  }
//  else
//  {
//    digitalWrite(batLedPin, LOW);
//  }
//}
//
//
//void loop() 
//{
//  batteryUsage();
//  delay(1000);
//}


 
/* Read voltage divider
 * Reads the voltage divider to calculate a battery voltage
 * This software has no warranty, real or implied and is free to distribute and modify 
 */

int batMonPin = A0;    // input pin for the divider
int val = 0;       // variable for the A/D value
float pinVoltage = 0; // variable to hold the calculated voltage
float batteryVoltage = 0;
float ratio = 2.316;  // Change this to match the MEASURED ration of the circuit

void setup() {
  
  Serial.begin(9600);      // open the serial port at 9600 baud
}

void loop() {  
  val = analogRead(batMonPin);    // read the voltage on the divider  
  
  pinVoltage = val * 0.00488;       //  Calculate the voltage on the A/D pin
                                    //  A reading of 1 for the A/D = 0.0048mV
                                    //  if we multiply the A/D reading by 0.00488 then 
                                    //  we get the voltage on the pin                                    
  
  batteryVoltage = pinVoltage * ratio;    //  Use the ratio calculated for the voltage divider
                                          //  to calculate the battery voltage
  Serial.print("Voltage: ");
  Serial.println(batteryVoltage);
  
  
  delay(1000);                  //  Slow it down
}

