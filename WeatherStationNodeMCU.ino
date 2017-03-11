#include <Wire.h>

//Time Stuff begin
#define RTC_DS1307_ADDRESS 0x68

typedef struct TIME {
  uint8_t Second;
  uint8_t Minute;
  uint8_t Hour;
  uint8_t Day;
  uint8_t WorkDay;
  uint8_t Month;
  uint8_t Year;
} TIME;

TIME gTime;

uint8_t bcd2bin (uint8_t bcd)
{
  return (bcd / 16 * 10) + (bcd % 16);
}

void TimeBcdToBin (TIME *Time) {
  Time->Second  = bcd2bin (Time->Second);
  Time->Minute  = bcd2bin (Time->Minute);
  Time->Hour    = bcd2bin (Time->Hour);
  Time->Day     = bcd2bin (Time->Day);
  Time->WorkDay = bcd2bin (Time->WorkDay);
  Time->Month   = bcd2bin (Time->Month);
  Time->Year    = bcd2bin (Time->Year);
}

bool ReadTime (TIME *Time) {

  Wire.beginTransmission (RTC_DS1307_ADDRESS);
  Wire.write(0x0);

  if (Wire.endTransmission () != 0) {
    return false; 
  }

  Wire.requestFrom (RTC_DS1307_ADDRESS, 7);
  Time->Second  = Wire.read ();
  Time->Minute  = Wire.read ();
  Time->Hour    = Wire.read ();
  Time->Day     = Wire.read ();
  Time->WorkDay = Wire.read ();
  Time->Month   = Wire.read ();
  Time->Year    = Wire.read ();

  TimeBcdToBin (Time);

  return true;
}

void PrintTime (TIME *Time) {
  Serial.print (Time->Year);
  Serial.print ("/");
  Serial.print (Time->Month);
  Serial.print ("/");
  Serial.print (Time->Day);
  Serial.print (" ");
  Serial.print (Time->Hour);
  Serial.print (":");
  Serial.print (Time->Minute);
  Serial.print (":");
  Serial.println (Time->Second);
}

//Time stuff end
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Wire.begin();
  while(!Serial) {
    
  }
  Serial.println();
  Serial.println("#Wheather station begining");
}

void loop() {
  // put your main code here, to run repeatedly:
  delay(1000);
  if (ReadTime (&gTime)) {
    PrintTime (&gTime);
  }

}
