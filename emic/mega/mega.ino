

// Date and time functions using a DS1307 RTC connected via I2C and Wire lib
#include <SoftwareSerial.h>
#include <Wire.h>
#include "RTClib.h"
#include <Adafruit_BMP085.h>
#include <DHT.h>
#define DHTPIN A1     // what pin we're connected to
#define DHTTYPE DHT22   // DHT 22  (AM2302)
#define rxPin 50    // Serial input (connects to Emic 2 SOUT)
#define txPin 51    // Serial output (connects to Emic 2 SIN)
DHT dht(DHTPIN, DHTTYPE);
SoftwareSerial emicSerial =  SoftwareSerial(rxPin, txPin);
long randNumber; //Random number used for random voice picker
//int sensorPin = 0; //the analog pin the TMP36's Vout (sense) pin is connected to

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

Adafruit_BMP085 bmp;
RTC_DS1307 rtc;

void setup () {
  pinMode(rxPin, INPUT);
  pinMode(txPin, OUTPUT);
  Serial.begin(9600);
  Serial.println("Arduino MEGA 2560 / EMIC test");
  emicSerial.begin(9600);
  dht.begin();
  bmp.begin();
  rtc.begin();
  Serial.println("rtc sensor Started");
  //rtc.adjust(DateTime(__DATE__, __TIME__));
  Serial.println("End of setup function");
}

void loop () {
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  float f = dht.readTemperature(true);
  float hi = dht.computeHeatIndex(f, h);
  Serial.println();
  DateTime now = rtc.now();
  if ((now.minute()==15) && (now.second() < 10 )){
    date();
    Time();
  }   
  if ((now.minute()==30) && (now.second() < 10 )){
    date();
    Time();
  } 
  if ((now.minute()==45) && (now.second() < 10 )){
    date();
    Time(); 
  }
  if ((now.minute()==59) && (now.second() > 55 )){
    date();
    Time();
  }
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
  
  //Serial.print("Temperature: \t"); 
  //Serial.print(t,1);
  //Serial.println(" *C ");
  Serial.print("with the currentTemperature at "); 
  Serial.print(f,1);
  Serial.println(" degrees fahrenheit ");
  if (hi>t){
    Serial.print("and will feel ");
    Serial.print(hi-f,1);
    Serial.print(" degrees hotter than the posted temperature");
    Serial.print(" with a measured Heat Index of, ");
    Serial.print(hi,1);
    Serial.println(" fahrenheit");

  }
  Serial.print("The Humidity is at "); 
  Serial.print(h,1);
  Serial.print(" % ");
  Serial.print(" The Barometric Pressure is ");
  Serial.print(bmp.readPressure()* 0.0002953,1);
  Serial.println(" inHg");
  /* 
   Serial.print("Altitude = \t\t");
   Serial.print(bmp.readAltitude(102025)*3.28084);
   Serial.println(" feet");
   
   Serial.print("Pressure at sealevel =\t");
   Serial.print(bmp.readSealevelPressure()* 0.0002953);
   Serial.println(" inHg");
   Serial.println();
   */
  delay(3000);
}

void date(){
  DateTime now = rtc.now();
  emicSerial.print('\n');//tell emic your done sending text
  while (emicSerial.read() != ':');   // When the Emic 2 has initialized and is ready, it will send a single ':' character, so wait here until we receive it
  delay(10);                          // Short delay
  emicSerial.flush(); 
  emicSerial.print(Harry);//Voice 0 - 8
  emicSerial.print(fast);
  emicSerial.print('S'); //tell emic your ready to send text
  emicSerial.print("Todays Date IS, ");
  emicSerial.print(now.dayOfWeek()[weekDays]);
  emicSerial.print(',');
  emicSerial.print(now.month()[Month]);
  emicSerial.print(',');
  emicSerial.print(now.day()[Day]);
  emicSerial.print('\n');//tell emic your done sending text
  while (emicSerial.read() != ':');   // When the Emic 2 has initialized and is ready, it will send a single ':' character, so wait here until we receive it
  delay(10);                          // Short delay
  emicSerial.flush();


}

void Time(){
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  float f = dht.readTemperature(true);
  float hi = dht.computeHeatIndex(f, h);
  DateTime now = rtc.now();
  emicSerial.print('\n');//tell emic your done sending text
  while (emicSerial.read() != ':');   // When the Emic 2 has initialized and is ready, it will send a single ':' character, so wait here until we receive it
  delay(10);                          // Short delay
  emicSerial.flush();                 // Flush the receive buffer
  emicSerial.print(loudest);
  emicSerial.print(fast); //Speed 0 - 200 = fastest
  emicSerial.print(Betty);//Voice 0 - 8
  emicSerial.print('S'); //tell emic your ready to send text
  emicSerial.print(" The Current Time is.");
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

  //Serial.print("Temperature: \t"); 
  //Serial.print(t,1);
  //Serial.println(" *C ");
  emicSerial.print("with the current Temperature at, "); 
  emicSerial.print(f,1);
  emicSerial.print("degrees fare in height, ");

  if (hi>t){
    emicSerial.print("and will feel, ");
    emicSerial.print(hi-f,1);
    emicSerial.print(" degrees hotter than the posted temperature, ");
    emicSerial.print(" with a measured Heat Index of, ");
    emicSerial.print(hi,1);
    emicSerial.print(" degrees fare in height, ");
  }

  emicSerial.print("The Humidity is at, "); 
  emicSerial.print(h,1);
  emicSerial.print(" % ");
  emicSerial.print(" and the Barometric Pressure is, ");
  emicSerial.print(bmp.readPressure()* 0.0002953,1);
  emicSerial.print(" Inches of mercury");
  emicSerial.print('\n');//tell emic your done sending text
  while (emicSerial.read() != ':');   // When the Emic 2 has initialized and is ready, it will send a single ':' character, so wait here until we receive it
  delay(10);                          // Short delay
  emicSerial.flush();
  delay(5000);
}



void alarm(){
  emicSerial.print('\n');//tell emic your done sending text
  while (emicSerial.read() != ':');   // When the Emic 2 has initialized and is ready, it will send a single ':' character, so wait here until we receive it
  delay(10);                          // Short delay
  emicSerial.flush();                 // Flush the receive buffer
  // Speaking Volume (-48 - 18) 18 = highest
  emicSerial.print(loudest);
  emicSerial.print(fastest); //Speed 0 - 200 = fastest
  // Uncoment below to pick and hold a single voice, just type the name from the boiler plate you want to use (case sensitive)
  emicSerial.print(Paul);//Voice 0 - 8 
  emicSerial.print('S'); //tell emic your ready to send text
  emicSerial.print("It's time to WAKE UP.");
  emicSerial.print('\n');//tell emic your done sending text
  while (emicSerial.read() != ':');   // When the Emic 2 has initialized and is ready, it will send a single ':' character, so wait here until we receive it
  delay(10);                          // Short delay
  emicSerial.flush();
  delay(3000);

}





