/*********************
 * 
 * Example code for the Adafruit RGB Character LCD Shield and Library
 * 
 * This code displays text on the shield, and also reads the buttons on the keypad.
 * When a button is pressed, the backlight changes color.
 * 
 **********************/

// include the library code:
#include <Wire.h>
#include <Adafruit_MCP23017.h>
#include <Adafruit_RGBLCDShield.h>
#include "RTClib.h"
#include <SoftwareSerial.h>

// The shield uses the I2C SCL and SDA pins. On classic Arduinos
// this is Analog 4 and 5 so you can't use those for analogRead() anymore
// However, you can connect other I2C sensors to the I2C bus and share
// the I2C bus.

#define rxPin 2    // Serial input (connects to Emic 2 SOUT)
#define txPin 4    // Serial output (connects to Emic 2 SIN)//pin 3 conflicts with CC3000 WiFI else use pin 2
// These #defines make it easy to set the backlight color
#define OFF 0x0
#define RED 0x1
#define YELLOW 0x3
#define GREEN 0x2
#define TEAL 0x6
#define BLUE 0x4
#define VIOLET 0x5
#define WHITE 0x7
long previousMillis = 0;        // will store last time LED was updated
long interval = 6500;   
long randNumber; //Random number used for random voice picker
int sensorPin = 0; //the analog pin the TMP36's Vout (sense) pin is connected to

// emic will say the month
char* Month[] = {
  "null", "January,", "Febuary", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"};
// emic will say the day
// Sunday is 0 from the now.dayOfWeek() call of the RTC
char* weekDays[] = {
  "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
//emic will say the numerical version of the calander day in human standard slang.
// there is no "0" day so I just put the word null in, the sketch will read the RTC "Library"(RTC SOFT) and say the correct numerical calender day
char* Day[] = {
  "null", "First", "Second", "Third","Forth", "Fifth", "Sixth", "Seventh","Eighth", "Nineth", "Tenth", "Eleventh", "Twelfth","Thirteenth","Fourteenth","fifthteenth","Sixteenth","Seventeenth","EightTeenth","Nine Teenth","Twenty eth","Twenty first","Twenty Second","Twenty third","Twenty Forth","Twenty Fifth","Twenty Sixth","Twenty Seventh","Twenty Eighth","Twenty Ninth"," Thirty eth","Thirty First"}; 

/*
// below is for voice reference
 0: Perfect Paul (Paulo) 
 1: Huge Harry (Francisco) 
 2: Beautiful Betty 
 3: Uppity Ursula 
 4: Doctor Dennis (Enrique) 
 5: Kit the Kid 
 6: Frail Frank 
 7: Rough Rita 
 8: Whispering Wendy (Beatriz)
 */
//Speaking volume
char* loudest = ("V18\n");
char* loud = ("V10\n");
char* normal = ("V0\n");
char* quiet = ("V-10\n");
//Speaking speed
char* fastest = ("W200\n");
char* fast = ("W175\n");
char* medium = ("W150\n");
char* slow = ("W100\n");

//Speaking voice
char* Paul = ("N0\n");//Voice 0 - 8
char* Harry  = ("N1\n");//Voice 0 - 8
char* Betty  = ("N2\n");//Voice 0 - 8
char* Ursula  = ("N3\n");//Voice 0 - 8
char* Dennis  = ("N4\n");//Voice 0 - 8
char* Kit  = ("N5\n");//Voice 0 - 8
char* Frank  = ("N6\n");//Voice 0 - 8
char* Rita  = ("N7\n");//Voice 0 - 8
char* Wendy  = ("N8\n");//Voice 0 - 8

//Random Speaking voice's
char* voice[]={
  "N0\n","N1\n","N2\n","N3\n","N4\n","N5\n","N6\n","N7\n","N8\n"};

Adafruit_RGBLCDShield lcd = Adafruit_RGBLCDShield();
SoftwareSerial emicSerial =  SoftwareSerial(rxPin, txPin);
RTC_DS1307 rtc;
void setup() {
  DateTime now = rtc.now();
  pinMode(rxPin, INPUT);
  pinMode(txPin, OUTPUT);
  // Debugging output
  Serial.begin(9600);
  emicSerial.begin(9600);

  // set up the LCD's number of columns and rows: 
  lcd.begin(16, 2);
  rtc.begin();
  lcd.setBacklight(OFF);

}

uint8_t i=0;
void loop() {


  DateTime now = rtc.now();
  // here is where you'd put code that needs to be running all the time.
  Serial.print(now.hour());
  Serial.print(":");
  Serial.print(now.minute());
  Serial.print(":");
  Serial.println(now.second());
  if ((now.hour()== 12)&&(now.minute() == 0)&&(now.second()>=1)){
    alarm();
  }
  //getting the voltage reading from the temperature sensor
  int reading = analogRead(sensorPin);  
  // converting that reading to voltage, for 3.3v arduino use 3.3
  float voltage = reading * 5.0;
  voltage /= 1024.0; 
  // now print out the temperature
  float temperatureC = (voltage - 0.5) * 100 ;  //converting from 10 mv per degree wit 500 mV offset
  //to degrees ((voltage - 500mV) times 100)
  // now convert to Fahrenheit 
  // and use INT so the emic only tells us the temp with the numbers prior to the dec point for claity
  int temperatureF = (temperatureC * 9.0 / 5.0) + 32.0;
  Serial.print(temperatureF); 
  Serial.println(" degrees F");

  uint8_t buttons = lcd.readButtons();

  if (buttons) {
    lcd.clear();
    lcd.setCursor(0,0);
    if (buttons & BUTTON_UP) {
      lcd.setBacklight(WHITE);
      talkTime();
    }
    if (buttons & BUTTON_DOWN) {
      talkTime();
    }
    if (buttons & BUTTON_LEFT) {
      lcd.setBacklight(RED);
      talkTime();
    }
    if (buttons & BUTTON_RIGHT) {
      lcd.setBacklight(TEAL);
      talkTime();
    }
    if (buttons & BUTTON_SELECT) {
      date();
    }
  }
  delay(500);
}

void talkTime(){
  int reading = analogRead(sensorPin);  

  // converting that reading to voltage, for 3.3v arduino use 3.3
  float voltage = reading * 5.0;
  voltage /= 1024.0; 

  // print out the voltage
  Serial.print(voltage); 
  Serial.println(" volts");

  // now print out the temperature
  float temperatureC = (voltage - 0.5) * 100 ;  //converting from 10 mv per degree wit 500 mV offset
  //to degrees ((voltage - 500mV) times 100)
  Serial.print(temperatureC); 
  Serial.println(" degrees C");
  int temperatureF = (temperatureC * 9.0 / 5.0) + 32.0;
  DateTime now = rtc.now();
  lcd.print(now.month(), DEC);
  lcd.print('/');
  lcd.print(now.day(), DEC);
  lcd.print('/');
  lcd.print(now.year(), DEC);
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
  emicSerial.print('\n');//tell emic your done sending text
  while (emicSerial.read() != ':');   // When the Emic 2 has initialized and is ready, it will send a single ':' character, so wait here until we receive it
  delay(10);                          // Short delay
  emicSerial.flush();                 // Flush the receive buffer
  // Speaking Volume (-48 - 18) 18 = highest
  emicSerial.print(loudest);
  //emicSerial.print(louder);
  // emicSerial.print(normal);
  // emicSerial.print(quiet);

  //emicSerial.print(slow); //Speed 0 - 200 = fastest
  // emicSerial.print(medium); //Speed 0 - 200 = fastest
  //emicSerial.print(fast); //Speed 0 - 200 = fastest
  emicSerial.print(fastest); //Speed 0 - 200 = fastest
  // Uncoment below to pick and hold a single voice, just type the name from the boiler plate you want to use (case sensitive)
  // emicSerial.print(Paul);//Voice 0 - 8 

  // Or, use randNumber to choose the name you want to use. 
  randNumber = random(8);//random voice picker

  emicSerial.print(randNumber[voice]);  //send the Emic the random voice picked
  emicSerial.print('S'); //tell emic your ready to send text
  emicSerial.print(temperatureF); 
  emicSerial.print(" degrees");
  emicSerial.print(" And The Current Time is.");
  if(now.hour() < 10){            // Zero padding if value less than 10 ie."09" instead of "9"
    emicSerial.print((now.hour() > 12) ? now.hour() - 12 : ((now.hour() == 0) ? 12 : now.hour()), DEC); // Conversion to AM/PM
    emicSerial.print(":"); 
  }
  else{
    emicSerial.print((now.hour() > 12) ? now.hour() - 12 : ((now.hour() == 0) ? 12 : now.hour()), DEC); // Conversion to AM/PM
    emicSerial.print(":"); 
  }
  if(now.minute()<=1){          // if minutes are less than 10
    emicSerial.print("oh-Clock."); // Put an the letters "oh" here so that the emic say's three-oh-five PM/AM. like we would say "3:05 PM/AM"  and not "Three zero five AM/PM".
  }
  else if((now.minute()>=1)&&(now.minute()<=9)) {          // if minutes are less than 10
    emicSerial.print("oh,"); // Put an the letters "oh" here so that the emic say's three-oh-five PM/AM. like we would say "3:05 PM/AM"  and not "Three zero five AM/PM".
    emicSerial.print(now.minute());
  }
  else{
    emicSerial.print(now.minute()); //emic will say the minutes as the are read
  }

  if(now.hour() < 12){           // Adding the AM/PM sufffix 
    emicSerial.print(" AM."); // ie; if less than 12, say AM


  }
  else{
    emicSerial.print(" PM.");//If more than 12, then say PM


  }
  emicSerial.print('\n');//tell emic your done sending text
  while (emicSerial.read() != ':');   // When the Emic 2 has initialized and is ready, it will send a single ':' character, so wait here until we receive it
  delay(10);                          // Short delay
  emicSerial.flush();
  delay(5000);
  lcd.clear();
  lcd.setBacklight(OFF);

}
void date(){
  lcd.setCursor(0, 0);
  lcd.setBacklight(RED);
  DateTime now = rtc.now();
  lcd.print(now.dayOfWeek()[weekDays]);
  lcd.setCursor(0, 1);
  lcd.print(now.month()[Month]);
  lcd.print(", ");
  lcd.print(now.day());
  emicSerial.print('\n');//tell emic your done sending text
  while (emicSerial.read() != ':');   // When the Emic 2 has initialized and is ready, it will send a single ':' character, so wait here until we receive it
  delay(10);                          // Short delay
  emicSerial.flush(); 
  emicSerial.print('S'); //tell emic your ready to send text
  emicSerial.print(now.dayOfWeek()[weekDays]);
  emicSerial.print(',');
  emicSerial.print(now.month()[Month]);
  emicSerial.print(',');
  emicSerial.print(now.day()[Day]);
  emicSerial.print('\n');//tell emic your done sending text
  while (emicSerial.read() != ':');   // When the Emic 2 has initialized and is ready, it will send a single ':' character, so wait here until we receive it
  delay(10);                          // Short delay
  emicSerial.flush();
  delay(5000);
  lcd.clear();
  lcd.setBacklight(OFF);
}

void alarm(){
  emicSerial.print('\n');//tell emic your done sending text
  while (emicSerial.read() != ':');   // When the Emic 2 has initialized and is ready, it will send a single ':' character, so wait here until we receive it
  delay(10);                          // Short delay
  emicSerial.flush();                 // Flush the receive buffer
  // Speaking Volume (-48 - 18) 18 = highest
  emicSerial.print(loudest);
  //emicSerial.print(louder);
  // emicSerial.print(normal);
  // emicSerial.print(quiet);

  //emicSerial.print(slow); //Speed 0 - 200 = fastest
  // emicSerial.print(medium); //Speed 0 - 200 = fastest
  //emicSerial.print(fast); //Speed 0 - 200 = fastest
  emicSerial.print(fastest); //Speed 0 - 200 = fastest
  // Uncoment below to pick and hold a single voice, just type the name from the boiler plate you want to use (case sensitive)
  emicSerial.print(Paul);//Voice 0 - 8 

  // Or, use randNumber to choose the name you want to use. 
  //randNumber = random(8);//random voice picker
  //emicSerial.print(randNumber[voice]);  //send the Emic the random voice picked
  emicSerial.print('S'); //tell emic your ready to send text
  emicSerial.print("It's time to WAKE UP.");
  emicSerial.print('\n');//tell emic your done sending text
  while (emicSerial.read() != ':');   // When the Emic 2 has initialized and is ready, it will send a single ':' character, so wait here until we receive it
  delay(10);                          // Short delay
  emicSerial.flush();
  delay(3000);

}











