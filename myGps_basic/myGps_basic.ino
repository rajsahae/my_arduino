#include <SoftwareSerial.h>
#include <TinyGPS.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>

TinyGPS gps;
SoftwareSerial ss(9, 8);

const int HMC6352SlaveAddress = 0x42 >> 1;
const int HMC6352ReadAddress = 0x41; //"A" in hex, read command

const int lcdAddress = 0x27; // 0x4F << 1

LiquidCrystal_I2C lcd(lcdAddress, 20, 4); // 20x4 lcd

int readFromCompass();
void gpsdump(TinyGPS &gps);
bool feedgps();
void printPloat(double f, int digits = 2);
void myOut(String str);

void setup()
{
  ss.begin(4800);
  Serial.begin(115200);
  
  Serial.println("myGps basic test");
  Wire.begin();
  
  lcd.init();
  lcd.backlight();
  lcd.setCursor(2,0); // char 2, line 0
  lcd.print("GPS with HMC6352");
  lcd.setCursor(2,1); // char 2, line 1
  lcd.print("and Sainsmart LCD");
  lcd.setCursor(4,2); // char 0, line 1
  lcd.print("Test Program");
  delay(5000);
  
  lcd.clear();
  lcd.setCursor(4, 0);
  lcd.print("degrees");
  
  lcd.setCursor(0,1);
  lcd.print("Dist to waypoint:");
}


void loop(){
	
  static int previousHeadingInt = -1;
  int headingInt = readFromCompass();
  
  if (headingInt != previousHeadingInt)
  {
    lcd.setCursor(0,0);
    lcd.print(headingInt);
    previousHeadingInt = headingInt;
  }
  
  feedgps();
  float dist = distance();
  Serial.print("Distance to waypoint: ");
  Serial.println(dist);
  if (dist > 0)
  {
	lcd.setCursor(0,2);
	lcd.print(dist);
  }
  
  delay(100);
}

int readFromCompass()
{
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

  // (MSB + LSB sum) / 10 - this should be a number between 0 and 360
  return ((MSB << 8) + LSB) / 10;
}

bool feedgps()
{
	while (ss.available())
	{
		if (gps.encode(ss.read()))
			return true;
	}
	
	return false;
}

float distance()
{
	float flat, flon;
	unsigned long age;
	
	// Get our coordinates from GPS
	gps.f_get_position(&flat, &flon, &age);
	Serial.print("Lat: ");
	Serial.print(flat);
	Serial.print(" Lon: ");
	Serial.println(flon);
	
	if (flat == TinyGPS::GPS_INVALID_F_ANGLE) return -1.0;
	if (flon == TinyGPS::GPS_INVALID_F_ANGLE) return -2.0;
	if (gps.satellites() == TinyGPS::GPS_INVALID_SATELLITES) return -3.0;
	if (gps.hdop() == TinyGPS::GPS_INVALID_HDOP) return -4.0;
	
	// First waypoint, middle of lawn in front of 1189 Millbrae Ave
	float x2lat = 37.59083;
	float x2lon = -122.402407;
	
	// Prep for calculation
	float dist_calc = 0;
	float dist_calc2 = 0;
	float diflat = 0;
	float diflon = 0;
	
	// Calculate distance from current point to waypoint
	diflat = radians(x2lat - flat);
	flat = radians(flat);
	x2lat = radians(x2lat);
	
	diflon = radians(x2lon - flon);
	
	dist_calc = pow(sin(diflat/2.0), 2);
	dist_calc2 = pow(cos(flat), 2) * cos(x2lat) * pow(sin(diflon/2.0), 2);
	
	dist_calc += dist_calc2;
	dist_calc = (2 * atan2(sqrt(dist_calc), sqrt(1.0-dist_calc)));
	
	dist_calc *= 6371000.0; // convert to meters
	
	return dist_calc;
}

void myOut(String str)
{

}