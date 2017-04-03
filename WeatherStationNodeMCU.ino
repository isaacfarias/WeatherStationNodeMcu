#include <Wire.h>
#include <Adafruit_BMP085.h>
#include <DHT.h>

//DHT stuff begin
#define DHTPIN 2
#define DHTTYPE DHT11

DHT gDht(DHTPIN, DHTTYPE);
//DHT stuff end

//Time Stuff begin
#define RTC_DS1307_ADDRESS 0x68
#define DEBUG 1
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

}

//Time stuff end

Adafruit_BMP085 gBmp;



void getData () {
  String data = "";
  int tempHum = 0;
  data += gTime.Year;
  data += "/";
  data += gTime.Month;
  data += "/";
  data += gTime.Day;
  data += " ";
  data += gTime.Hour;
  data += ":";
  data += gTime.Minute;
  data += ":";
  data += gTime.Second;
  data += " ";
  data += gBmp.readTemperature ();
  data += " ";
  data += gBmp.readPressure ();
  data += " ";
  data += gBmp.readAltitude();
  data += " ";
  tempHum = (int) gDht.readHumidity();
  if(tempHum > 99) {
    tempHum = 0;
  }
  data += tempHum; 
  data += "%";
  Serial.println (data);
}

uint16_t Devices;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Wire.begin();
  while(!Serial) {
    
  }
  Serial.println();
  Serial.println("# Wheather station begining");
  Serial.print("# BMP18 sensor: ");
  Serial.println(gBmp.begin());
  gDht.begin();
}

void loop() {
  // put your main code here, to run repeatedly:
  delay(1000);
  ReadTime (&gTime);
  getData ();
}
