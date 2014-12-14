/*********************
 * 
 * Example code for the Adafruit RGB Character LCD Shield and Library
 * 
 * This code displays text on the shield, and also reads the buttons on the keypad.
 * When a button is pressed, the backlight changes color.
 *
 * WITH ALARM
 **********************/

// include the library code:
#include <Wire.h>
#include <Adafruit_MCP23017.h>
#include <Adafruit_RGBLCDShield.h>
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
int alarmState = LOW;
int alarmOff = HIGH;// high = on low = off
int snooze = 0;
int snoozeTime = 1;
float Hour =0;

// Variables will change:
//int ledState = LOW;             // ledState used to set the lcd
long previousMillis = 0;        // will store the start time the lcd backlight was turned on

// the follow variables is a long because the time, measured in miliseconds,
// will quickly become a bigger number than can be stored in an int.
long interval = 750;//2*1000; // interval at which to turn off the backlight (milliseconds)

#define alarm = off
RTC_Millis rtc;

void setup() {
  // Debugging output
  Serial.begin(115200);
  // set up the LCD's number of columns and rows: 
  lcd.begin(16, 2);
  rtc.begin(DateTime(F(__DATE__), F(__TIME__)));  // Print a message to the LCD. We track how long it takes since
  // this library has been optimized a bit and we're proud of it :)
  int time = millis();
  DateTime now = rtc.now();
  lcd.print(now.month(), DEC);
  lcd.print('/');
  lcd.print(now.day(), DEC);
  lcd.print('/');
  lcd.print(now.year(), DEC);

  lcd.setBacklight(WHITE);
}

uint8_t i=0;
void loop() {
  DateTime now = rtc.now();

  Time();


  lcd.setCursor(0, 0);
  uint8_t buttons = lcd.readButtons();
  if (buttons) {

    lcd.setCursor(0,0);
    if (buttons & BUTTON_UP) {
      lcd.setBacklight(RED);

    }
    if (buttons & BUTTON_DOWN) {
      lcd.setBacklight(YELLOW);
    }
    if (buttons & BUTTON_LEFT) {
      lcd.setBacklight(GREEN);
    }
    if (buttons & BUTTON_RIGHT) {
      lcd.setBacklight(TEAL);
      lcd.clear();
      alarmOff=HIGH;
      snooze = (now.minute());
      lcd.setCursor(0, 0);
      lcd.print("Snooze: ");
      lcd.print(snoozeTime);
      lcd.print(" Min");
    }
    if (buttons & BUTTON_SELECT) {
      alarmOff=LOW;
      lcd.setBacklight(RED);
    }
  }
  if (now.minute() - snooze > snoozeTime){
    alarmOff=LOW;

  }
  unsigned long currentMillis = millis();

  if(currentMillis - previousMillis > interval) {
    // save the last time you blinked the LED 
    previousMillis = currentMillis; 

    if ((now.hour()>=22)&& (now.hour()<=23) && (now.minute()>=01) && alarmOff==LOW)

    // if the LED is off turn it on and vice-versa:
    if (alarmState == LOW){
      alarmState = HIGH;
      lcd.setBacklight(RED);
    }
    else
      if (alarmState = HIGH){
        alarmState = LOW;
        lcd.setBacklight(YELLOW);
      }
  }
  if (alarmOff==HIGH){
    
      lcd.print(now.month(), DEC);
    lcd.print('/');
    lcd.print(now.day(), DEC);
    lcd.print('/');
    lcd.print(now.year(), DEC);
  }
}


void Time(){
  DateTime now = rtc.now();
  // set the cursor to column 0, line 1
  // (note: line 1 is the second row, since counting begins with 0):
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
}





