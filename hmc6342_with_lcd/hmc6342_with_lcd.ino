#include <LiquidCrystal_I2C.h>
#include <Wire.h>

const int HMC6352SlaveAddress = 0x42 >> 1;
const int HMC6352ReadAddress = 0x41; //"A" in hex, read command

const int lcdAddress = 0x27; // 0x4F << 1

LiquidCrystal_I2C lcd(lcdAddress, 20, 4); // 20x4 lcd

int headingValue;

void setup(){
  Wire.begin();
  
  lcd.init();
  lcd.backlight();
  lcd.setCursor(4,0); // char 4, line 0
  lcd.print("HMC6352 with");
  lcd.setCursor(4,1); // char 4, line 1
  lcd.print("Sainsmart LCD");
  lcd.setCursor(4,2); // char 0, line 1
  lcd.print("Test Program");
  delay(5000);
}

void loop(){
  static int previousHeadingInt = -1;
  
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
  int headingInt = headingSum / 10; // this should be a number between 0 and 360
  
  if (headingInt != previousHeadingInt)
  {
    lcd.clear();
    lcd.setCursor(0,0);
    
    lcd.print(headingInt);
    lcd.print(" degrees");
    
    previousHeadingInt = headingInt;
    
    delay(100);
  }
}
