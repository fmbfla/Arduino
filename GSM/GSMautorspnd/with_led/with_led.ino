/*
___________________________________________________________________
 LINE 114
 txtMessage = input_trimmed.substring(47,60);// WORKING ON THIS!
 __________________________________________________________________
 My version of a "BAIT CAR/TRUCK"
 This version doesn't have the relay shield attached but would be easily attached to a car/truck to integrate the locks, windows, and ignition!
 
 This is a test to use trimmed strings to grab the incoming phone number
 and to Reply to the number Texting "IN" with the gps data and Google map location link!
 Ref http://www.induino.com/2013/11/interfacing-serial-devices-part-3.html
 !********* A word of advice you can use hardware serial and Software serial simultaneously but you cant with SoftWare Serial as I know of!********* 
 
 This set up uses 2 devices sharing the Software Serial library 
 so be cautious on when to "Turn the devices on and off" so as to not lose any data being "sent or received"
 
 Arduino Uno R3
 Seeed Sim900 shield V-2 connected to dig pins 7 & 8 for use with Software Serial
 Adafruit Flora GPS module connected to dig pins 2 & 3 for use with Software Serial
 Using TinyGps Library
 
 9/9/2014
 The Drunkin Programer
 */
#include <SoftwareSerial.h>
#include <TinyGPS.h>
#include <Wire.h>
#include "RTClib.h"


SoftwareSerial GPS(2,3);
SoftwareSerial SIM900(7,8);
TinyGPS gps;
RTC_Millis rtc;

const int led =  13;      // the number of the LED pin
char incoming_char=0;// Will hold the incoming character from the GSM shield

float flat, flon;//holds the GPS DATA
unsigned long age, date, time, chars = 0;
unsigned short sentences = 0, failed = 0;

////////////////
static void smartdelay(unsigned long ms);
static void print_float(float val, float invalid, int len, int prec);
static void print_int(unsigned long val, unsigned long invalid, int len);
static void print_date(TinyGPS &gps);
static void print_str(const char *str, int len);
/////////////////////
String input=""; // Declare an Empty String Variable
String input_trimmed="";// a variable to store the trimmed string
String substring=""; // a variable to store the substring "PHONE NUMBER"
String txtMessage="";// a variable to store the txtMessage "The inComing Text Message"
int length_before_trim = 0, length_after_trim = 0, length_substring=0; // 3 variables for storing the string lengths
int ledState = LOW; 
void setup()  
{
  pinMode(12, OUTPUT);
  pinMode(led, OUTPUT);   
  Serial.begin(19200);
  Serial.println("This is a test to use trimmed strings to grab the incoming phone number");
  Serial.println("that can be used to Auto-Reply to the number Calling or Texting IN!");
  Serial.println();
  SIM900power();  // turn on shield
  delay(5000);  // give time to log on to network.
  SIM900.print("AT+CMGF=1\r");  // set SMS mode to text
  delay(100);

}//////////////////////////////////////// end setup ///////////////////////////////// 

void loop()  // run over and over again
{

  wait4TXT();
  delay(100);

}////////////////////////////////////////end loop ///////////////////////////////// 


void SIM900power()
{

  SIM900.begin(19200);
  Serial.println("POWERING ON!!...Please wait");
  digitalWrite(9, HIGH);
  //digitalWrite(led, HIGH); 
  delay(1000);
  digitalWrite(9, LOW);
  //digitalWrite(led, HIGH); 
  delay(7000);
  //digitalWrite(led, LOW); 
  SIM900.flush();
  delay(1000);
  Serial.println("Ready!");
  Serial.flush();
}
//read the SIM900 and look for the beginning of the Header/CLIP of the text message in my case it's a "+"
void wait4TXT(){

  SIM900.begin(19200);
  delay(100);
  if (SIM900.available() >0){
    incoming_char=SIM900.read();
    if (incoming_char=='+'){//If you find a "+" at the beginning of the text message header/CLIP Send a notification to the serial monitor for debugging
      // and run the "readSIM900" funktion to find the incoming phones number.
      //digitalWrite(led, HIGH);
      delay(10);
      readSIM900();
      delay(10);
      //digitalWrite(led, LOW);
    }
  }
}
void readSIM900(){
  SIM900.begin(19200);
  if(SIM900.available()) // Check if there is incoming data in the Serial Buffer
  {
    input = ""; // reset the input variable
    while(SIM900.available()) // Process incoming data till the buffer becomes empty
    {     
      input += (char)SIM900.read();// Read a byte of data, convert it to character and append it to the string variable
      delay(1);
    }
    Serial.println();
    input_trimmed = input; // assign the trimmed variable with the input value
    input_trimmed.trim();
    substring = input_trimmed.substring(7,18); //Because Im using the "+" fromthe beginning of the "Clip" I use the seventh char and the 19th is where the calling fone number is in the string of recieved text
    txtMessage = input_trimmed.substring(45,61);// WORKING ON THIS!
    length_before_trim = input.length();
    length_after_trim = input_trimmed.length();
    length_substring = substring.length();
    // UNCOMMENT BELOW For debugging and or to find the inComing number in the string
    //Serial.println("Your Original Input : " +input);
    /*Serial.print("Length of Your Original Input : ");
     Serial.println(length_before_trim);
     Serial.println("The Trimmed Input : " +input_trimmed);
     Serial.print("Length of Trimmed Input : ");
     Serial.println(length_after_trim);
     Serial.println("Substring : " +substring);//"PHONE NUMBER"
     Serial.print("Length of Substring : ");
     Serial.println(length_substring);*/


    if (incoming_char=='C'){
      //Serial.println("substring >=12 ");
      SIM900.flush();
      SIM900.end(); 
      //digitalWrite(led, LOW);
    }
    if (substring.length()<=10){
      //Serial.println("substring <=10");
      SIM900.flush();
      SIM900.end();
      //digitalWrite(led, LOW);
    }
    if (substring.length()>=12){
      //Serial.println("substring >=12 ");
      SIM900.flush();
      SIM900.end(); 
      //digitalWrite(led, LOW);
    }
    if (substring>=""){
      //Serial.println("No substring ");
      SIM900.flush();
      SIM900.end(); 
      //digitalWrite(led, LOW);
    }

    //i/////////////////////////////////////////////////////////////////////////f(substring.length()==11){
    //digitalWrite(led,HIGH);
    Serial.println("Valid Text Message recieved ");
    Serial.println();
    Serial.print("txtMessage : ");
    Serial.println(txtMessage);
    if (txtMessage=="#"){
      delay(100);
        digitalWrite(led,HIGH);
      SIM900.flush();
      SIM900.end(); 
      delay(100); 
    }

    if (txtMessage=="$"){
      Serial.print("LOW : ");
      Serial.println(txtMessage);
      digitalWrite(led,LOW);
      SIM900.flush();
      SIM900.end(); 
      delay(100); 
    }/////////////////////////////////////////////////////////////////////////

    //Message();

    SIM900.flush();
    SIM900.end(); 
    delay(100); 
    // } 
  }

}


//Sends TXT Message with Google Map link to phone number recieved via txt message 
void Message(){
  //digitalWrite(led, HIGH); 
  delay(100);
  GpsData();
  delay(100);
  //digitalWrite(led, LOW); 
  SIM900.begin(19200);//and reconnect to SS so we can send the message with the lat/long cords
  SIM900.print("AT+CMGF=1\r");  // set SMS mode to text
  delay(100);
  //digitalWrite(led, HIGH); 
  SIM900.print("AT + CMGS = \"+");
  delay(100);
  //digitalWrite(led, LOW);
  SIM900.print(substring);//"PHONE NUMBER"
  Serial.print("Phone Number ");//"PHONE NUMBER"
  Serial.println(substring);//"PHONE NUMBER"
  delay(100);
  //digitalWrite(led, HIGH);
  SIM900.println("\"");  
  delay(100);
  //digitalWrite(led, LOW);
  SIM900.print(flat, 6);//Latitude with 6 decimal points
  SIM900.print(" N");
  SIM900.print(", ");
  SIM900.print(flon, 6);// Longitude with 6 decimal points
  SIM900.println(" W");
  SIM900.print("Speed: ");
  SIM900.println(gps.f_speed_mph());
  SIM900.print("Heading: ");
  SIM900.println(gps.f_course());
  SIM900.println("Click the link to view in Google Map");
  SIM900.print("http://maps.google.com/maps?q="); //creating the link to GM
  SIM900.print(flat, 6);//Latitude with 6 decimal points
  SIM900.print(",");
  SIM900.println(flon, 6);// Longitude with 6 decimal points
  //Below should create a pop up baloon with text "Location" but I cant get it to do so
  //SIM900.println("%28Location%29&t=k&z=16");
  delay(100);
  //digitalWrite(led, HIGH);
  SIM900.println((char)26); // End AT command with a ^Z, ASCII code 26
  delay(100);
  //digitalWrite(led, LOW);
  SIM900.println();
  delay(1000);//Give some time for the message to be sent
  //digitalWrite(led, HIGH);
  SIM900.flush();
  SIM900.println("AT+CMGD=1,4"); // delete all SMS
  delay(100);
  //digitalWrite(led, LOW);
  SIM900.flush();
  SIM900.end();
  Serial.println("   Message Sent!");
  Serial.println();


}

//get GPS DATA
void GpsData(){

  GPS.begin(9600);
  delay(100);
  bool newData = false;
  unsigned long chars;
  unsigned short sentences, failed;
  for (unsigned long start = millis(); millis() - start < 1000;)
  {
    while (GPS.available())
    {
      char c = GPS.read();
      // Serial.write(c); // uncomment this line if you want to see the GPS data flowing
      if (gps.encode(c)) // Did a new valid sentence come in?
        newData = true;
    }
  }
  if (newData)
  {
    //float flat, flon;
    unsigned long age;
    gps.f_get_position(&flat, &flon, &age);
    printSerial();
    /* Serial.print("LAT=");
     Serial.print(flat == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flat, 6);
     Serial.print(" LON=");
     Serial.print(flon == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flon, 6);
     Serial.print(" SAT=");
     Serial.print(gps.satellites() == TinyGPS::GPS_INVALID_SATELLITES ? 0 : gps.satellites());
     Serial.print(" PREC=");
     Serial.println(gps.hdop() == TinyGPS::GPS_INVALID_HDOP ? 0 : gps.hdop());*/
  }

  gps.stats(&chars, &sentences, &failed);

  if (chars == 0)
    Serial.println("** No characters received from GPS: check wiring **");
  delay(100);

  if (sentences == 0){
    /*Serial.print(" CHARS=");
     Serial.print(chars);
     Serial.print(" SENTENCES=");
     Serial.print(sentences);
     Serial.print(" CSUM ERR=");
     Serial.println(failed);
     Serial.println();*/
    GPS.flush();
    GPS.end();
    Serial.println("GPS is Connected, Waiting for SATELLITE Lock");
    Serial.print("Sat's Locked in. ");
    Serial.println(gps.satellites() == TinyGPS::GPS_INVALID_SATELLITES ? 0 : gps.satellites());
    // Serial.println("Check GPS location for obstructions to clear skies");
    Serial.println();

    //interval= 1000L;

  }
  GPS.flush();
  GPS.end();
}
void printSerial(){
  float Speed = (gps.f_speed_mph());
  float Heading = (gps.f_course());
  DateTime now = rtc.now();
  Serial.print("Message Received at ");
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
  Serial.print("InComing Number: ");
  Serial.println(substring);
  Serial.print("Lat: ");
  Serial.println(flat);
  Serial.print("Lat: ");
  Serial.println(flon);
  Serial.print("Speed: ");
  Serial.println(Speed);
  Serial.print("Heading: ");
  Serial.println(Heading);
  Serial.println();
  //digitalWrite(13,LOW);

}





















