/*********************

Example code for the Adafruit RGB Character LCD Shield and Library

This code displays text on the shield, and also reads the buttons on the keypad.
When a button is pressed, the backlight changes color.

**********************/

// include the library code:
#include <Wire.h>
#include <Adafruit_MCP23017.h>
#include <Adafruit_RGBLCDShield.h>
#include <Adafruit_BMP085.h>
Adafruit_BMP085 bmp;
#include "RTClib.h"

// The shield uses the I2C SCL and SDA pins. On classic Arduinos
// this is Analog 4 and 5 so you can't use those for analogRead() anymore
// However, you can connect other I2C sensors to the I2C bus and share
// the I2C bus.
Adafruit_RGBLCDShield lcd = Adafruit_RGBLCDShield();

// These #defines make it easy to set the backlight color
#define OFF 0x0
#define RED 0x1
#define YELLOW 0x3
#define GREEN 0x2
#define TEAL 0x6
#define BLUE 0x4
#define VIOLET 0x5
#define WHITE 0x7
RTC_Millis rtc;
char* Month[] = {
  "null", "Jan,", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
// emic will say the day
// Sunday is 0 from the now.dayOfWeek() call of the RTC
char* weekDays[] = {
  "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
//emic will say the numerical version of the calander day in human standard slang.
// there is no "0" day so I just put the word null in, the sketch will read the RTC "Library"(RTC SOFT) and say the correct numerical calender day
char* Day[] = {
  "null", "First", "Second", "Third","Forth", "Fifth", "Sixth", "Seventh","Eighth", "Nineth", "Tenth", "Eleventh", "Twelfth","Thirteenth","Fourteenth","fifthteenth","Sixteenth","Seventeenth","EightTeenth","Nine Teenth","Twenty eth","Twenty first","Twenty Second","Twenty third","Twenty Forth","Twenty Fifth","Twenty Sixth","Twenty Seventh","Twenty Eighth","Twenty Ninth"," Thirty eth","Thirty First"}; 

void setup() {
  rtc.begin(DateTime(F(__DATE__), F(__TIME__)));
  // Debugging output
  Serial.begin(9600);
    if (!bmp.begin()) {
	Serial.println("Could not find a valid BMP085 sensor, check wiring!");
	while (1) {}
  }
  // set up the LCD's number of columns and rows: 
  lcd.begin(16, 2);

  // Print a message to the LCD. We track how long it takes since
  // this library has been optimized a bit and we're proud of it :)
  
}

uint8_t i=0;
void loop() {
   lcd.setCursor(0, 0);
  DateTime now = rtc.now();
  lcd.print(now.dayOfWeek()[weekDays]);
  lcd.print(" ");
  lcd.print(now.month()[Month]);
  lcd.print(" ");
  lcd.print(now.day());
  lcd.print(" ");
  lcd.print(bmp.readTemperature(),1);
  lcd.setCursor(0, 1);
  if(now.hour() < 10){                // Zero padding if value less than 10 ie."09" instead of "9"
    lcd.print(" ");
    lcd.print((now.hour()> 12) ? now.hour() - 12 : ((now.hour() == 0) ? 12 : now.hour()), DEC); // Conversion to AM/PM  
  }
  else{
    lcd.print((now.hour() > 12) ? now.hour() - 12 : ((now.hour() == 0) ? 12 : now.hour()), DEC); // Conversion to AM/PM
  }
  lcd.print(':');
  if(now.minute() < 10){         // Zero padding if value less than 10 ie."09" instead of "9"
    lcd.print("0");
    lcd.print(now.minute(), DEC);
  }
  else{
    lcd.print(now.minute(), DEC);
  }
  lcd.print(':');
  if(now.second() < 10){         // Zero padding if value less than 10 ie."09" instead of "9"
    lcd.print("0");
    lcd.print(now.second(), DEC);
  }
  else{
    lcd.print(now.second(), DEC);
  }
  if(now.hour() < 12){                  // Adding the AM/PM sufffix
    lcd.print(" AM");
  }
  else{
    lcd.print(" PM");
  }
  lcd.print(" ");
  lcd.print(bmp.readPressure()* 0.0002953,1);
  
  
  
  
  
  
    //lcd.print(" *F");
     uint8_t buttons = lcd.readButtons();

  if (buttons) {
    lcd.clear();
    lcd.setCursor(0,0);
    if (buttons & BUTTON_UP) {
      lcd.print("UP ");
      lcd.setBacklight(RED);
    }
    if (buttons & BUTTON_DOWN) {
      // set the cursor to column 0, line 1
  // (note: line 1 is the second row, since counting begins with 0):
  lcd.clear();
  lcd.setCursor(0,1);
    lcd.print("Baro =   ");
    lcd.print(bmp.readPressure()* 0.0002953,1);
    //lcd.print(" inHg");
  lcd.setCursor(0, 0);
  lcd.print("Temp =   ");
    lcd.print(bmp.readTemperature(),1);
      lcd.setBacklight(GREEN);
      delay(5000);
      lcd.clear();
    }
    if (buttons & BUTTON_LEFT) {
      lcd.print("LEFT ");
      lcd.setBacklight(GREEN);
    }
    if (buttons & BUTTON_RIGHT) {
      lcd.print("RIGHT ");
      lcd.setBacklight(OFF);
    }
    if (buttons & BUTTON_SELECT) {
      lcd.print("SELECT ");
      lcd.setBacklight(VIOLET);
    }
  } 
    
}
