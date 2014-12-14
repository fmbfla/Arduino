
// GPSR/GSM Arduino Remote vehical Starter device "REOMMENDED FOR AUTOMATIC TRANSMISSION ONLY!" (Bait Car)
// Seeed GPSR, Seeed Relay shield, Arduino UNO R3, some wire small alligator clips or solder, wire cutters/Srippers
// ****CAUTION, USE A VEHICAL WIRING DIAGRAM TO TRACE THE "IGNITON ON/OFF" and "STARTING WIRE" From the KEY SWITCH ****
// ****Connecting directly to the ECU/MPU or the STARTER solenoid circuit can/will DAMAGE the ECU/MPU or START the ENGINE IN GEAR!!!!!****
#include "Wire.h"
#include "RTClib.h"
#include <SoftwareSerial.h>
SoftwareSerial mySerial(7,8);//defaut 
char inchar; // Will hold the incoming character from the GSM shield
char Serialchar;
int pwr=digitalRead(10);
int relayPin4 = 4; //Ingnition ON/OFF
int relayPin5 = 5; // Start
int relayPin6 = 6; // Un-LOCK DOORS
int count = 0;
// Seeed GPRS shield uses pin 7 and pin 8 on the Arduino for SWS
// The Relay shield uses pin 7 pin on a relay coil and affects the SoftwareSerial communication. 
// so you either have to use Hardware Serial( MUST Disconnect the GPRS Shield to up-Load Sketch's)
// or bend/cut or re=route to pin 9 to be able to utilize all four relays.(Remember 7/8 are used for SWS) 
unsigned long previousMillis = 0;  // will store last time ignition State was updated
// interval at which to Have the ignition ON before turning it off again(milliseconds)
unsigned long interval = 30000*2;
RTC_Millis rtc; // Software RTC time

void setup()
{
  rtc.begin(DateTime(__DATE__, __TIME__));
  // set up the digital pins to control
  pinMode(relayPin4,OUTPUT);
  pinMode(relayPin5,OUTPUT);  
  pinMode(relayPin6,OUTPUT);
  pinMode(pwr,INPUT);  
  digitalWrite(relayPin4, LOW);
  digitalWrite(relayPin5,LOW);  
  digitalWrite(relayPin6,LOW); 
  // wake up the GSM shield
  //SIM900power();
  mySerial.begin(19200);
  delay(100);
  Serial.begin(19200);
    Serial.println("GPRS/LCD SHIELD...");
if (pwr==HIGH){
  Serial.println("GPRS IS ON!");
}
if (pwr==LOW){
  Serial.println("GPRS IS OFF!");
}

 /* delay(10000);  // give time to log on to network.
  mySerial.print("AT+CMGF=1\r");  // set SMS mode to text
  delay(100);
  mySerial.print("AT+CNMI=2,2,0,0,0\r");
  // blurt out contents of new SMS upon receipt to the GSM shield's serial out
  delay(100);/*
  mySerial.println("AT + CMGS = \"+19412959244\"");                                     // recipient's mobile number, in international format
   delay(100);
   mySerial.println("Ready to accept commands via TEXT...");        // message to send*/
   delay(100);
   mySerial.println((char)26);                       // End AT command with a ^Z, ASCII code 26
   delay(100);
  Serial.println("Ready to accept commands via TEXT...");
  Serial.print("System Time: ");
  time();
  Serial.println();
}






void loop()
{
  if(Serial.available() >0)
  {
while(Serial.available())          // reading data into char array 
    {
      Serial.write(Serialchar);

      //If we get a "#" The arduino will look for a specific char
      Serialchar=Serial.read(); 
      if (Serialchar=='p')
      {
        if (pwr==LOW){
          Serial.println("Powering UP");
          SIM900power();
        }
        if (pwr==HIGH){
          Serial.println("Power is already on");
        }
      }
      if (Serialchar=='o'){//turn off GPRS

        int pwr=digitalRead(10);
        if (pwr==LOW){
          //lcd.print("GPRS is alreay OFF!");
          Serial.println("Power is already OFF");
        }
        if (pwr==HIGH){
          mySerial.println("AT+CPOWD=1");
          //lcd.print("GPRS IS OFF!");
          Serial.println("GPRS IS OFF!");
          delay(3000);
          //lcd.setBacklight(OFF);
        }
      }
    }
    //If we get a "#" The arduino will look for a specific char
    Serialchar=Serial.read(); 
    if (Serialchar=='#')
    {
      if (Serialchar=='r')
      {
        count=1;
        digitalWrite (relayPin4,HIGH);

        time();
        delay(100);
        mySerial.println("AT + CMGS = \"+19412959244\"");  // recipient's mobile number, in international format
        delay(100);
        mySerial.println(" FAN is ON!");// message to send
        delay(100);
        mySerial.println((char)26); // End AT command with a ^Z, ASCII code 26
        delay(10); 
        Serial.println(" FAN is ON!");// message to send

      }

      delay(10);
      Serialchar=Serial.read();
      if (Serialchar=='p')

      {
        SIM900power();
        delay(10000);  // give time to log on to network.
        mySerial.print("AT+CMGF=1\r");  // set SMS mode to text
        delay(100);
        mySerial.print("AT+CNMI=2,2,0,0,0\r");
        // blurt out contents of new SMS upon receipt to the GSM shield's serial out
        delay(100);
      }
    }
  }

  if(mySerial.available() >0)
  {
    //If we get a "#" The arduino will look for a specific char
    inchar=mySerial.read(); 
    if (inchar=='#')
    {

      delay(10);
      inchar=mySerial.read();
      // if we get an "r" for "RUN" Turn the Ignition "ON"
      if (inchar=='r')

      {
        count=1;
        digitalWrite (relayPin4,HIGH);
        time();
        delay(100);
        mySerial.println("AT + CMGS = \"+19412959244\"");  // recipient's mobile number, in international format
        delay(100);
        mySerial.println(" FAN is ON!");// message to send
        delay(100);
        mySerial.println((char)26); // End AT command with a ^Z, ASCII code 26
        delay(10); 
        Serial.println(" FAN is ON!");// message to send
        delay(100);
        mySerial.println("AT+CMGD=1,4"); // delete all SMS

        // ***CAUTION USE A VEHICAL WIRING DIAGRAM TO TRACE THE STARTING WIRE From the KEY SWITCH ***
        // ****Connecting directly to the starter solenoid circuit can/will start the engine IN GEAR!!!!!****

      }

      if (inchar=='k'){
        time();
        delay(10);

        digitalWrite(relayPin4, LOW);// connected to the "ECU main relay Circuit"
        delay(100);
        mySerial.println("AT + CMGS = \"+19412959244\"");                                     // recipient's mobile number, in international format
        delay(100);
        mySerial.println(" ENGINE KILLED VIA TEXT!");// message to send
        Serial.println(" ENGINE KILLED VIA TEXT!");
        delay(100);
        mySerial.println((char)26); // End AT command with a ^Z, ASCII code 26
        delay(100);
      }
    }
  }
 
    

    }


void SIM900power()
// software equivalent of pressing the GSM shield "power" button
{
  digitalWrite(9, HIGH);
  delay(1000);
  digitalWrite(9, LOW);
  delay(7000);
  mySerial.print("AT+CMGF=1\r");  // set SMS mode to text
  delay(100);
  mySerial.print("AT+CNMI=2,2,0,0,0\r");
  // blurt out contents of new SMS upon receipt to the GSM shield's serial out
  delay(100);
}


void time(){


  DateTime now = rtc.now();
  if(now.hour() < 10){            // Zero padding if value less than 10 ie."09" instead of "9"
    Serial.print((now.hour() > 12) ? now.hour() - 12 : ((now.hour() == 0) ? 12 : now.hour()), DEC); // Conversion to AM/PM
    Serial.print(":"); 
  }
  else{
    Serial.print((now.hour() > 12) ? now.hour() - 12 : ((now.hour() == 0) ? 12 : now.hour()), DEC); // Conversion to AM/PM
    Serial.print(":"); 
  }

  // Convert the Minutes to Speakable English
  if((now.minute()<=9)) {          // if minutes are less than 10
    Serial.print("0"); // Put an the letters "oh" here so that the emic say's three-oh-five PM/AM. like we would say "3:05 PM/AM"  and not "Three zero five AM/PM".
    Serial.print(now.minute());
  }
  else{
    Serial.print(now.minute()); //emic will say the minutes as they are read
  }
  Serial.print(":");
  if((now.second()<=9)) {          // if minutes are less than 10
    Serial.print("0"); // Put an the letters "oh" here so that the emic say's three-oh-five PM/AM. like we would say "3:05 PM/AM"  and not "Three zero five AM/PM".
    Serial.print(now.second());
  }
  else{
    Serial.print(now.second());
  }
  // Convert the time to Speakable words (1-12 Am or PM 
  if(now.hour() < 10){            // Zero padding if value less than 10 ie."09" instead of "9"
    mySerial.println((now.hour() > 12) ? now.hour() - 12 : ((now.hour() == 0) ? 12 : now.hour()), DEC); // Conversion to AM/PM
    mySerial.println(":"); 
  }
  else{
    mySerial.println((now.hour() > 12) ? now.hour() - 12 : ((now.hour() == 0) ? 12 : now.hour()), DEC); // Conversion to AM/PM
    mySerial.println(":"); 
  }

  // Convert the Minutes to Speakable English
  if((now.minute()<=9)) {          // if minutes are less than 10
    mySerial.println("0"); // Put an the letters "oh" here so that the emic say's three-oh-five PM/AM. like we would say "3:05 PM/AM"  and not "Three zero five AM/PM".
    mySerial.println(now.minute());
  }
  else{
    mySerial.println(now.minute()); //emic will say the minutes as they are read
  }
  mySerial.println(":");
  if((now.second()<=9)) {          // if minutes are less than 10
    mySerial.println("0"); // Put an the letters "oh" here so that the emic say's three-oh-five PM/AM. like we would say "3:05 PM/AM"  and not "Three zero five AM/PM".
    mySerial.println(now.second());
  }
  else{
    mySerial.println(now.second());
  }

}










































