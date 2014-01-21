#include <SoftwareSerial.h>

const int rxPin = 10;
const int txPin = 11;
const int resetPin = 12;
const int motorPowerPin = 13;

const byte motor1 = B10;
const byte motor2 = B11;

const byte startByte = 0x80;
const byte deviceType = 0x00;
const byte changeConfigByte = 0x02;
const byte configByte = B10; // binary 00000010, controlling motors 2 and 3


SoftwareSerial serialMotorController(rxPin, txPin);

void configureMotorController(int baud, boolean dual, byte motorNum)
{
  // Initialize motor controller serial communication
  serialMotorController.begin(baud);

  // Delay for 100ms
  delay(100);
  
  // Configuration is achieved by sending a three-byte packet consisting of the start byte, a 
  // configuration command byte, and the new configuration byte:
  // start byte = 0x80, change configuration = 0x02, new settings, 0x00-0x7F
  serialMotorController.write(startByte);
  serialMotorController.write(changeConfigByte);
  
  byte configByte = 0;
  
  if (!dual) configByte = B01000000;
  
  configByte |= motorNum;
  
  serialMotorController.write(configByte);
}

void setMotorSpeed(int motor, int spd)
{
  // To set the speed and direction of a motor, send a four-byte command with the following 
  // structure to the motor controller:
  // start byte = 0x80, device type = 0x00, motor # and direction, motor speed
  serialMotorController.write(startByte);
  
  // This byte identifies the device type for which the command is 
  // intended, and it should be 0x00 for commands sent to this motor controller. All devices 
  // that are not dual motor controllers ignore all subsequent bytes until another start byte is 
  // sent.
  serialMotorController.write(deviceType);
  
  byte motorByte = 0;
  
  if (spd < 0)
  {
    motorByte = 0; // OR 00000000
    spd = -spd;
  }
  else
  {
    motorByte = 1; // OR 00000001
  }
  
  if (spd > 127)
  {
    spd = 127;
  }
  
  motorByte |= ( motor << 1);

  serialMotorController.write(motorByte);
  
  // The most significant bit must be zero since this is not a start 
  // byte. The remaining seven bits specify the motor speed. The possible range of values 
  // for byte 4 is thus 0x00 to 0x7F (0 to 127 decimal). 0x00 turns the motor off, and 0x7F 
  // turns the motor fully on; intermediate values correspond to intermediate speeds. The 
  // motor will brake when the speed is set to 0 in forward or reverse.
  serialMotorController.write(spd);
  
}

void setup()
{
  // Open serial communications
  Serial.begin(9600);
  
  // Pin to control power to the motors
  pinMode(motorPowerPin, OUTPUT);
  digitalWrite(motorPowerPin, LOW); // Kill power to motors to ensure they don't move during setup
  
  /*
  For some reason, I can't get this to work. The reset pin goes low then high but the motor
  controller doesn't seem to reset correctly. Doing it manually works fine.
  
  I found a solution to this problem in the forum on the Pololu website.
  http://forum.pololu.com/viewtopic.php?f=15&t=3558&p=23102&hilit=low+voltage+controller+reset#p23102
  You need to write the reset pin to low before you write it to high.
  */
  pinMode(resetPin, OUTPUT);
  digitalWrite(resetPin, LOW);
  delay(200);
  digitalWrite(resetPin, HIGH);
  delay(200);
  
  // Configure the motor controller at 9600 baud, dual motors = true, motor1 number = 2 (motor2 = 3)
  configureMotorController(9600, true, 2);
  
  
  // we have to reset the controller after configuring it for it to work
  // delay 3 seconds to visually inspect the motor controller is correctly configured
  delay(3000);
  digitalWrite(resetPin, LOW);
  delay(200);
  digitalWrite(resetPin, HIGH);
  delay(200);
  
  // Turn the motor power back on
  digitalWrite(motorPowerPin, HIGH);
}

void loop()
{
  // motorspeed can range from -127 to 127
  
  setMotorSpeed(motor1, 50);
  delay(500);
  setMotorSpeed(motor1, 0);
  setMotorSpeed(motor1, -50);
  delay(500);
  setMotorSpeed(motor1, 0);
  
  setMotorSpeed(motor2, 50);
  delay(500);
  setMotorSpeed(motor2, 0);
  setMotorSpeed(motor2, -50);
  delay(500);
  setMotorSpeed(motor2, 0);
}

