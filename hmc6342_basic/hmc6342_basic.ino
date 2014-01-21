//All this does is read the heading from HMC6352 and spit it out via serial
// It also modulates the brightness of an LED based on the heading. The LED should be brightest when
// pointing north, and darkest when pointing south.
// For 0-180 degrees, we do 0-50% brightness
// For 180-360 degrees, we do 50-0% brightness.

#include <Wire.h>

// "The Wire library uses 7 bit addresses throughout. 
//If you have a datasheet or sample code that uses 8 bit address, 
//you'll want to drop the low bit (i.e. shift the value one bit to the right), 
//yielding an address between 0 and 127."

const int HMC6352SlaveAddress = 0x42 >> 1; // I know 0x42 is less than 127, but this is still required
const int HMC6352ReadAddress = 0x41; //"A" in hex, A command is: 

int headingValue;

const int led = 9; // Use pin 9 for the LED
int brightness = 0;

void setup(){
  Serial.begin(9600);
  Wire.begin();
  pinMode(led, OUTPUT); // Set pin 9 to output
}

void loop(){
  //"Get Data. Compensate and Calculate New Heading"
  Wire.beginTransmission(HMC6352SlaveAddress);
  Wire.write(HMC6352ReadAddress);              // The "Get Data" command
  Wire.endTransmission();

  //time delays required by HMC6352 upon receipt of the command
  //Get Data. Compensate and Calculate New Heading : 6ms
  delay(6);

  Wire.requestFrom(HMC6352SlaveAddress, 2); //get the two data bytes, MSB and LSB

  //"The heading output data will be the value in tenths of degrees
  //from zero to 3599 and provided in binary format over the two bytes."
  byte MSB = Wire.read();
  byte LSB = Wire.read();

  float headingSum = (MSB << 8) + LSB; //(MSB / LSB sum)
  float headingInt = headingSum / 10; // this should be a number between 0 and 360
  
  // subtract 180 from the headingInt to get a range between -180 and 180
  // Then take the absolute value, and multiply by the ratio
  int adjustedHeadingInt = headingInt - 180;
  int dutyCycle = abs(map(adjustedHeadingInt, -180, 180, -127, 127));

  analogWrite(led, dutyCycle);
  
  Serial.print(headingInt);
  Serial.print(" degrees : ");
  Serial.print(adjustedHeadingInt);
  Serial.print(" degrees : ");
  Serial.println(dutyCycle);
 
  delay(100);
}
