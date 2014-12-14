// Date and time functions using a DS1307 RTC connected via I2C and Wire lib

#include <Wire.h>
#include "RTClib.h"
#include <SoftwareSerial.h>

SoftwareSerial mySerial(3,2);//defaut
RTC_DS1307 rtc;

void setup () {
  Serial.begin(115200);
  mySerial.begin(9600);
  rtc.begin();

  if (! rtc.isrunning()) {
    Serial.println("RTC is NOT running!");
    // following line sets the RTC to the date & time this sketch was compiled
    //rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
  }
}

void loop () {

  myTime();
  myDate();
  mySerial.print("temp 80f");
  delay(1000);

}


void myTime(){
  DateTime now = rtc.now();
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
    mySerial.print(" AM");
  }
  else{
    mySerial.print(" PM");
  }
  mySerial.print("");
}
void myDate(){
  DateTime now = rtc.now();

  mySerial.print(now.month(), DEC);
  mySerial.print('/');
  mySerial.print(now.day(), DEC);
  mySerial.print('/');
  mySerial.print(now.year(), DEC);

}



