#include <SoftwareSerial.h>


SoftwareSerial ss(9, 8);

void setup()
{
  ss.begin(4800);
  Serial.begin(4800);
}

void loop()
{
  if (ss.available())
  {
    Serial.write(ss.read());
  }
}
