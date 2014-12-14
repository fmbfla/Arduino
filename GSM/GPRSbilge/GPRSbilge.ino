
#include "Wire.h"
#include "RTClib.h"
#include <SoftwareSerial.h>
SoftwareSerial mySerial(7,8);//defaut 
char inchar; // Will hold the incoming character from the GSM shield
char Serialchar;// Will hold the incoming character from the Serial port
char num[]= "AT + CMGS = \"+19412959244\"";//number to send SMS to
int sensorPin = 0; //the analog pin the TMP36's Vout (sense) pin is connected to
int relayPin4 = 4; //Bilge pump
int relayPin5 = 5; // Accessory
int relayPin6 = 6; // Accessory
//  DigPin7 USED FOR SOFTWARESERIAL! "Conflicts with Relay Shield", uncoment if using Hardware jumpers on the Seeed Shield.
//int relayPin6 = 7; // Accessorie
int pwr_sens = 10;// Dig10 jumper to dtr on seeed GPRS v1 checks for HIGH or LOW (2.8v)

RTC_Millis rtc; // Software RTC time

void setup()
{
  rtc.begin(DateTime(__DATE__, __TIME__));
  // set up the digital pins to control
  pinMode(relayPin4,OUTPUT);//Dig4 unused
  pinMode(relayPin5,OUTPUT);//Dig5 unused
  pinMode(relayPin6,OUTPUT);//Dig6 unused
  //  DigPin7 USED FOR SOFTWARESERIAL! "Conflicts with Relay Shield", uncoment if using Hardware jumpers on the Seeed Shield.
  //  pinMode(relayPin7,OUTPUT);//Dig7  
  pinMode(pwr_sens,OUTPUT);//Dig10 
  pinMode(10,INPUT);
  digitalWrite(relayPin4, LOW);//Dig4 
  digitalWrite(relayPin5,LOW); //Dig5  
  digitalWrite(relayPin6,LOW); //Dig6 
  // USED FOR SOFTWARESERIAL "Conflicts with Relay Shield", uncoment if using Hardware jumpers on the Seeed Shield.
  //digitalWrite(relayPin7,LOW); //Dig7    

  Serial.begin(19200);
  Serial.println("GPRS Bilge TEST..");
  Serial.println();

  if (digitalRead(10)==HIGH){//if we get voltage from the 
    Serial.println("GPRS IS ON!");
    Serial.println(" Ready to accept commands via SMS..");
    Serial.println(" Or commands via SERIAL...");
  }
  if (digitalRead(10)==LOW){
    Serial.println("GPRS IS OFF!");
    Serial.println("Ready to accept commands via SERIAL...");
  }
  Serial.println();
  Serial.print("System Start time: ");
  time();
  tmp36();
  Serial.println();
  mySerial.begin(19200);
  delay(100);

  //power_on();
  mySerial.println("AT + CMGS = \"+19412959244\"");  // recipient's mobile number, in international format
   delay(100);
   temp();
   mySerial.println(" Ready to accept commands via SMS... ");// message to send
   delay(100);
   mySerial.println(" #r= bilge on #k=bilge off");// message to send
   delay(100); 
   mySerial.println((char)26); // End AT command with a ^Z, ASCII code 26
   delay(100);
   mySerial.println(); 
   mySerial.flush();
}

void loop()
{

  if(mySerial.available() >0){
    inchar=mySerial.read();

    if (inchar=='#'){
      delay(10);
      inchar=mySerial.read();
      delay(10);
      if (inchar=='r'){
        time();
        //Serial.println("Bilge is ON!");// message to send
        digitalWrite (relayPin4,HIGH);
        mySerial.println(num);  // recipient's mobile number, in international format
        delay(100);
        temp();
        tmp36();
        mySerial.println(" Bilge is ON!");// message to send
        delay(100);
        mySerial.println((char)26); // End AT command with a ^Z, ASCII code 26
        delay(100);
        mySerial.println(); 
        delay(100);
        mySerial.println("AT+CMGD=1,4"); // delete all SMS
        mySerial.flush();
        return;
      }

      if (inchar=='k'){
        time();
        //Serial.println("Bilge is OFF!");
        digitalWrite(relayPin4, LOW);// connected to the "ECU main relay Circuit"
        mySerial.println(num);                                     // recipient's mobile number, in international format
        delay(100);
        temp();
        mySerial.println(" Bilge is OFF!");// message to send
        delay(100);
        mySerial.println((char)26); // End AT command with a ^Z, ASCII code 26
        delay(100);
        mySerial.println();
        delay(100);
        mySerial.println("AT+CMGD=1,4"); // delete all SMS
        mySerial.flush();
        return;
      }
    }
  }
}




void power_on()
// software equivalent of pressing the GSM shield "power" button
{
  Serial.println("POWERING ON...Please wait");
  digitalWrite(9, HIGH);
  delay(1000);
  digitalWrite(9, LOW);
  delay(7000);
  mySerial.print("AT+CLIP=1\r"); // turn on caller ID notification
  delay(100);
  mySerial.print("AT+CNMI=2,2,0,0,0\r"); 
  delay(100);
  Serial.println("READY!");
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
    Serial.println(now.second());
  }
  else{
    Serial.println(now.second());
  }
}
void temp(){// Convert the time to Speakable words (1-12 Am or PM 
  DateTime now = rtc.now();

  if(now.hour() < 10){            // Zero padding if value less than 10 ie."09" instead of "9"
    mySerial.print((now.hour() > 12) ? now.hour() - 12 : ((now.hour() == 0) ? 12 : now.hour()), DEC); // Conversion to AM/PM
    mySerial.print(":"); 
  }
  else{
    mySerial.print((now.hour() > 12) ? now.hour() - 12 : ((now.hour() == 0) ? 12 : now.hour()), DEC); // Conversion to AM/PM
    mySerial.print(":"); 
  }

  // Convert the Minutes to Speakable English
  if((now.minute()<=9)) {          // if minutes are less than 10
    mySerial.print("0"); // Put an the letters "oh" here so that the emic say's three-oh-five PM/AM. like we would say "3:05 PM/AM"  and not "Three zero five AM/PM".
    mySerial.print(now.minute());
  }
  else{
    mySerial.print(now.minute()); //emic will say the minutes as they are read
  }
  mySerial.print(":");
  if((now.second()<=9)) {          // if minutes are less than 10
    mySerial.print("0"); // Put an the letters "oh" here so that the emic say's three-oh-five PM/AM. like we would say "3:05 PM/AM"  and not "Three zero five AM/PM".
    mySerial.println(now.second());
  }
  else{
    mySerial.println(now.second());
  }
}

void tmp36(){
  //getting the voltage reading from the temperature sensor
  int reading = analogRead(sensorPin);  
  // converting that reading to voltage, for 3.3v arduino use 3.3
  float voltage = reading * 5.0;
  voltage /= 1024.0; 
  float temperatureC = (voltage - 0.5) * 100 ;  //converting from 10 mv per degree wit 500 mV offset
  float temperatureF = (temperatureC * 9.0 / 5.0) + 32.0;
  Serial.print("Current temp: ");
  Serial.print(temperatureF); 
  Serial.println(" degrees F");
  mySerial.print("Current temp: ");
  mySerial.print(temperatureF); 
  mySerial.println(" degrees F");
}



































