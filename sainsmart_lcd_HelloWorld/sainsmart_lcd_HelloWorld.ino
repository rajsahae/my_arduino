#include <LiquidCrystal_I2C.h>
#include <Wire.h>

/*
Finally got this working by reading this post:
http://bradsduino.blogspot.ch/2012/10/sainsmart-iici2ctwi-serial-2004-20x4.html
Downloaded the library linked to there (it's really all about the library)
At some point, I should diff the different libraries I have and see why this one worked
whereas the other one's didn't
*/

LiquidCrystal_I2C lcd(0x27,20,4);  // set the LCD address to 0x4F for a 20 chars and 4 line display

void setup()
{
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0,0);
  lcd.print("Hello, World!");
}

void loop()
{
}
