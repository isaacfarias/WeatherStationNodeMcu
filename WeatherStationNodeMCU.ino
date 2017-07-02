#include <Wire.h>
#include <Adafruit_BMP085.h>
#include <DHT.h>
#include <Sensors.h>

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <stdio.h>
const char* ssid = "No disponible";
const char* password = "farias38244744";

char timePage[] = "<!DOCTYPE html>\n <html>\n <head>\n </head>\n <body>\n <form action=\"/setData\">\n <h1>\n <br>Hora y fecha<br>\n </h1>\n <script type=\"text/javascript\">\n \n function getTimeString() {\n var d = new Date();\n var aux;\n var val = \'\';\n \n aux = d.getHours();\n if (aux<10) {\n val += \'0\' + aux;\n } else {\n val += aux;\n }\n aux = d.getMinutes();\n if (aux<10) {\n val += \':0\' + aux;\n } else {\n val += \':\' + aux;\n }\n return val;\n }\n \n function getDateString() {\n var d = new Date();\n var aux;\n var val;\n var val = \'\'+d.getFullYear();\n aux = (d.getMonth()+1);\n if (aux < 10) {\n val += \'-0\' + aux;\n } else {\n val += \'-\' + aux;\n }\n aux = d.getDate();\n if (aux < 10) {\n val += \'-0\' + aux;\n } else {\n val += \'-\' + aux;\n }\n return val;\n }\n \n document.write(\'<input type=\"time\" name=\"hora\" value=\"\'+ getTimeString() +\'\">\');\n document.write(\'<input type=\"date\" name=\"fecha\" value=\"\'+ getDateString() +\'\">\');\n setInterval(function(){ \n //code goes here that will be run every 5 seconds.\n document.getElementsByName(\"hora\")[0].value = getTimeString();\n }, 30000);\n </script>\n <input type=\"submit\" value=\"Enviar\">\n </form>\n </body>\n </html>";
String homePage0 = "<!DOCTYPE html> <html > <head> </head> <body> <h1> <br>Datos del sensor:<br> </h1> <h2>";

String homePage2 = "</h2> <form action = \"/setData\"> <button type=\"submit\" >Set RTC</button> </form> <script type=\"text/javascript\"> setTimeout(function(){ window.location.reload(1); }, 5000); </script> </body> </html>";
char data_c[40];

int last_humidity = 0;

ESP8266WebServer server(80);

//DHT stuff begin
#define DHTPIN 14
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

byte decToBcd(byte val){
// Convert normal decimal numbers to binary coded decimal
  return ( (val/10*16) + (val%10) );
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

void setTimeDate(byte hour, byte minute, byte second, byte year, byte month, byte monthDay){
  
  byte weekDay =     1; //1-7

  Wire.beginTransmission(RTC_DS1307_ADDRESS);
  Wire.write(0x0); //stop Oscillator

  Wire.write(decToBcd(second));
  Wire.write(decToBcd(minute));
  Wire.write(decToBcd(hour));
  Wire.write(decToBcd(weekDay));
  Wire.write(decToBcd(monthDay));
  Wire.write(decToBcd(month));
  Wire.write(decToBcd(year));

  if (Wire.endTransmission() != 0) {
    Serial.println("Not able to set time");
  } else {
    Serial.println("Able to set time in RTC");
  }

}

void PrintTime (TIME *Time) {

}

//Time stuff end

Adafruit_BMP085 gBmp;


String data;
void getData () {
  data = "";
  int tempHum = 0;
  tempHum = (int) gDht.readHumidity();
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
  //reading altitude generates an error with humidity read
  data += gBmp.readAltitude();
  data += " ";
  
  if(tempHum > 99) {
    tempHum = last_humidity;
  } else {
    last_humidity = tempHum;
  }
  data += tempHum; 
  data += "%";
  data.toCharArray(data_c,40);
  data_c[39] = '\0';
  Serial.println (data_c);
}

void handleRoot() {
  //server.send(200, "text/plain", "hello from esp8266!");
  ReadTime (&gTime);
  getData ();
  
  //server.send(200, "text/html", data);
  server.send(200, "text/html", homePage0 + ' ' + data + ' ' + homePage2);
  
  //server.send(200, "text/html", homePage2);
 
}

int isDigit(char c) {
    if (c >= '0' && c <= '9') {
      return 1;
    }
    return 0;
}

int parseInt(int *val, char* str) {
    int len = 0;
    int aux = 0;
    int dec = 1;
    int idx = 0;
    int ret = 0;

    while (str[len] != '\0' && isDigit(str[len])) {
        len++;
    }
    if (len == 0) {
        return 0;
    }
    ret = len;
    while (len>0) {
        idx = len-1;
        aux = aux + ((str[idx] - '0') * dec);
        len--;
        dec*=10;
    }
    *val = aux;
    return ret;
}

void setDataCallBack() {
  
  byte hour_b, minute_b, second_b = 30;
  byte year_b, month_b, day_b;
  int idx = 0, aux_int;//using aux_int to avoid alligned memory issues
  char buf[20];

  
  server.send(200, "text/html",timePage);
    for (uint8_t i=0; i<server.args(); i++){
    Serial.println(" " + server.argName(i) + ": " + server.arg(i));
  }
  if (server.args()<2) {
    return;
  }
  server.arg(0).toCharArray(buf,20);
  idx += parseInt((int *)&hour_b,buf)+1;
  idx += parseInt((int *)&minute_b,&(buf[idx]))+1;

  server.arg(1).toCharArray(buf,20);
  idx = 0;
  idx += parseInt(&aux_int,buf)+1;
  year_b = (byte) (aux_int - 2000);
  idx += parseInt(&aux_int,&(buf[idx]))+1;
  month_b = (byte) aux_int;
  idx += parseInt((int *)&aux_int,&(buf[idx]))+1;
  day_b = (byte) aux_int;

  setTimeDate(hour_b, minute_b, second_b, year_b, month_b, day_b);
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
  WiFi.begin(ssid, password);
  Serial.print("#Initializing WIFI");
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.print("\n#IP address: ");
  Serial.println(WiFi.localIP());
  server.on("/", handleRoot);
  server.on("/setData", setDataCallBack);
  server.begin();
  Serial.println("#HTTP server started");
}

void loop() {
  // put your main code here, to run repeatedly:
  server.handleClient();
  delay(1000);

}
