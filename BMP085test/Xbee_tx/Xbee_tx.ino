
// Date and time functions using a DS1307 RTC connected via I2C and Wire lib

#include <SoftwareSerial.h>
#include "DHT.h"
#include <Wire.h>
#include "RTClib.h"
#include <Adafruit_BMP085.h>

#define DHTPIN A0     // what pin we're connected to
#define DHTTYPE DHT22   // DHT 22  (AM2302)

Adafruit_BMP085 bmp;

SoftwareSerial mySerial(3,2);
char* Month[] = {
  "Leave empty! ", "January", "Febuary", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"};
// Sunday is 0 from the now.dayOfWeek() call of the RTC
char* weekDays[] = {
  "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Satuday"};
RTC_DS1307 rtc;
DHT dht(DHTPIN, DHTTYPE);
void setup () {
  dht.begin();
  Serial.begin(9600);
  mySerial.begin(9600);
  if (!bmp.begin()) {
    Serial.println("Could not find a valid BMP085 sensor, check wiring!");
    while (1) {
    }
  }
#ifdef AVR
  Wire.begin();
#else
  Wire1.begin(); // Shield I2C pins connect to alt I2C bus on Arduino Due
#endif
  rtc.begin();

  if (! rtc.isrunning()) {
    Serial.println("RTC is NOT running!");
    // following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
  }
}

void loop () {
  float h = dht.readHumidity();
  // Read temperature as Celsius
  float t = dht.readTemperature();
  // Read temperature as Fahrenheit
  float f = dht.readTemperature(true);

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  // Compute heat index
  // Must send in temp in Fahrenheit!
  float hi = dht.computeHeatIndex(f, h);

  Serial.print("Humidity: "); 
  Serial.println(h);
  Serial.print("Temperature: "); 
  Serial.print(f);
  Serial.println(" *F");
  Serial.print("Heat index: ");
  Serial.print(hi);
  Serial.println(" *F");
  Serial.println();

  Serial.print("bmp Temperature = ");
  Serial.print(bmp.readTemperature());
  Serial.println(" *C");

  Serial.print("Pressure = ");
  Serial.print(bmp.readPressure());
  Serial.println(" Pa");

  Serial.println();
  delay(500);
  DateTime now = rtc.now();
  Serial.print(now.dayOfWeek()[weekDays]);
  Serial.print(", ");
  Serial.print(now.month()[Month]);
  Serial.print(", ");
  Serial.print(now.day(), DEC);
  Serial.print(", ");
  Serial.println(now.year(), DEC); 
  Serial.print("Time: ");
  if(now.hour() < 10){                // Zero padding if value less than 10 ie."09" instead of "9"
    Serial.print(" ");
    Serial.print((now.hour()> 12) ? now.hour() - 12 : ((now.hour() == 0) ? 12 : now.hour()), DEC); // Conversion to AM/PM  
  }
  else{
    Serial.print((now.hour() > 12) ? now.hour() - 12 : ((now.hour() == 0) ? 12 : now.hour()), DEC); // Conversion to AM/PM
  }
  Serial.print(':');
  if(now.minute() < 10){         // Zero padding if value less than 10 ie."09" instead of "9"
    Serial.print("0");
    Serial.print(now.minute(), DEC);
  }
  else{
    Serial.print(now.minute(), DEC);
  }
  Serial.print(':');
  if(now.second() < 10){         // Zero padding if value less than 10 ie."09" instead of "9"
    Serial.print("0");
    Serial.print(now.second(), DEC);
  }
  else{
    Serial.print(now.second(), DEC);
  }
  if(now.hour() < 12){                  // Adding the AM/PM sufffix
    Serial.println(" AM");
  }
  else{
    Serial.println(" PM");
  }
  Serial.println("_________________________");
  Serial.println();
  mySerial.println();
  mySerial.print(now.dayOfWeek()[weekDays]);
  mySerial.print(" ");
  mySerial.print(now.month()[Month]);
  mySerial.print(" ");
  mySerial.print(now.day(), DEC);
  mySerial.print(", ");
  mySerial.println(now.year(), DEC); 
  mySerial.print("Time:         ");
  if(now.hour() < 10){                // Zero padding if value less than 10 ie."09" instead of "9"
    mySerial.print(" ");
    mySerial.print((now.hour()> 12) ? now.hour() - 12 : ((now.hour() == 0) ? 12 : now.hour()), DEC); // Conversion to AM/PM  
  }
  else{
    mySerial.print((now.hour() > 12) ? now.hour() - 12 : ((now.hour() == 0) ? 12 : now.hour()), DEC); // Conversion to AM/PM
  }
  mySerial.print(':');
  if(now.minute() < 10){         // Zero padding if value less than 10 ie."09" instead of "9"
    mySerial.print("0");
    mySerial.print(now.minute(), DEC);
  }
  else{
    mySerial.print(now.minute(), DEC);
  }
  mySerial.print(':');
  if(now.second() < 10){         // Zero padding if value less than 10 ie."09" instead of "9"
    mySerial.print("0");
    mySerial.print(now.second(), DEC);
  }
  else{
    mySerial.print(now.second(), DEC);
  }
  if(now.hour() < 12){                  // Adding the AM/PM sufffix
    mySerial.println(" AM");
  }
  else{
    mySerial.println(" PM");
  }

  mySerial.print("Temperature: "); 
  mySerial.print(f,1);
  mySerial.println(" *F");
  mySerial.print("Humidity:    "); 
  mySerial.print(h,1);
  mySerial.println(" %");
  mySerial.print("Pressure:    ");
  mySerial.print(bmp.readPressure()* 0.0002953,1);
  mySerial.println(" inHg");

  //mySerial.print("Heat index: ");
  // mySerial.print(hi);
  // mySerial.println(" *F");

  //mySerial.print("bmp Temperature = ");
  //mySerial.print(bmp.readTemperature());
  //mySerial.println(" *F");
  //mySerial.print("Pressure = ");
  //mySerial.print(bmp.readPressure()* 0.0002953,1);
  //mySerial.println(" inHg");
  mySerial.println("_________________________");
  
  delay(1000);
}








