// Date and time functions using a DS1307 RTC connected via I2C and Wire lib

#include <Wire.h>
#include "RTClib.h"

RTC_DS1307 rtc;

void setup () {
  Serial.begin(115200);
#ifdef AVR
  Wire.begin();
#else
  Wire1.begin(); // Shield I2C pins connect to alt I2C bus on Arduino Due
#endif
  rtc.begin();
  rtc.adjust(DateTime(__DATE__, __TIME__));



}

void loop () {
  Serial.println();
  DateTime now = rtc.now();
  Serial.print("Date: ");
  Serial.print(now.month(), DEC);
  Serial.print('/');
  Serial.print(now.day(), DEC);
  Serial.print('/');
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
  Serial.println();
  /*
  Serial.print(' ');
   Serial.print(now.hour(), DEC);
   Serial.print(':');
   Serial.print(now.minute(), DEC);
   Serial.print(':');
   Serial.print(now.second(), DEC);
   Serial.println();
   
   Serial.print(" since midnight 1/1/1970 = ");
   Serial.print(now.unixtime());
   Serial.print("s = ");
   Serial.print(now.unixtime() / 86400L);
   Serial.println("d");
   
   // calculate a date which is 7 days and 30 seconds into the future
   DateTime future (now.unixtime() + 7 * 86400L + 30);
   
   Serial.print(" now + 7d + 30s: ");
   Serial.print(future.year(), DEC);
   Serial.print('/');
   Serial.print(future.month(), DEC);
   Serial.print('/');
   Serial.print(future.day(), DEC);
   Serial.print(' ');
   Serial.print(future.hour(), DEC);
   Serial.print(':');
   Serial.print(future.minute(), DEC);
   Serial.print(':');
   Serial.print(future.second(), DEC);*/
  Serial.println();

  delay(3000);
}




