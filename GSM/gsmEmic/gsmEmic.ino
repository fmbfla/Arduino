#include <SoftwareSerial.h>
#include "Wire.h"
#include "RTClib.h"
#include "Adafruit_BMP085.h"
// I2C pins usable for I2C only the GPRS shield uses A4/A5
// so no analog readingds are possible
const int rxPin = 2;// emicSerial S/W Serial input (connects to Emic 2 SOUT)
const int txPin = 3;// emicSerial S/W Serial output (connects to Emic 3 SIN)
const int gsmRx = 7;// GPRS S/W Serial input (From GPRS connects to  arduino pin 7)
const int gsmTx = 8;// GPRS S/W Serial output (From GPRS connects to arduino pin 8)
const int led = 10; // Pin 10 = Led to notify of call Connect/Disconnect

int sensorPin = 0;                // Adafruit Tmp36
int count=0;                      // counter for buffer array 
int numring=0;                    // Start the Ring count at 0 after every call 
int comring=2;                    // If it rings Twice Answer it
int totalCalls = 0;               // Total incomming calls
char inchar;                      // Will hold incoming character from the Serial Port 
//unsigned char buffer[64];       // buffer array for data receive over serial port

SoftwareSerial GPRS (gsmRx, gsmTx);
SoftwareSerial emicSerial  (rxPin, txPin);
Adafruit_BMP085 bmp;              // BMP Sensor

RTC_Millis rtc; // Software RTC time

///////////////////Begin Setup////////////////////////////////

void setup()
{
  bmp.begin();

  rtc.begin(DateTime(__DATE__, __TIME__));
  Serial.begin(19200);            // the Serial port of Arduino baud rate for Deugging.


  delay(500);    // 1/2 second delay
  pinMode(rxPin, INPUT);//Emic
  pinMode(txPin, OUTPUT);//Emic
  pinMode(gsmRx, INPUT);//GPRS
  pinMode(gsmTx, OUTPUT);//GPRS
  pinMode(led, OUTPUT);// Led
  digitalWrite(led, LOW);//Sets led low on startup
  GPRS.begin(19200); // Start the GPRS S/W Serial port so we can Set the auto answer via an AT command
  delay(100);
  GPRS.println("ATS0=2"); // Set auto answer to 3 rings 
  delay(100);
  GPRS.end();

  emicSerial.begin(9600);
  emicSerial.print('\n');             // Send a CR in case the system is already up
  while (emicSerial.read() != ':');   // When the Emic 2 has initialized and is ready, it will send a single ':' character, so wait here until we receive it
  delay(10);                          // Short delay
  emicSerial.flush(); 
  // Speak some text
  emicSerial.print('S');
  emicSerial.print("I am now ready to answer phone-call's");  // Send the desired string to convert to speech
  emicSerial.print('\n');
  while (emicSerial.read() != ':');   // Wait here until the Emic 2 responds with a ":" indicating it's ready to accept the next command
  emicSerial.end();//Disconnect emicSerial S/W Serial so the GPRS can use S/W Serial

  Serial.println("I am ready to answer phone call's");


}  


/////////////////////Begin Loop//////////////////////////////////

void loop()
{

  digitalWrite(led, LOW); 
  delay(100);
  GPRS.begin(19200); //Start the GPRS S/W Serial port up

  if(GPRS.available() >0)  //If a character comes in from the cellular module...
  {
    //toSerial();
    inchar=GPRS.read();
    if (inchar=='R')

    {
      delay(10);
      inchar=GPRS.read(); 
      if (inchar=='I')

      {
        delay(10);
        inchar=GPRS.read();
        if (inchar=='N')

        {
          delay(10);
          inchar=GPRS.read(); 
          if (inchar=='G')
          {
            delay(10);
          }
          numring++;  // So the phone (our GSM shield) has 'rung' once, i.e. if it were a real phone it would have sounded 'ring-ring'
          Serial.println("ring!");

          if (numring==comring)
          {
            numring=0; // reset ring counter
            delay(100);
            TriggerEmic();
            totalCalls++;
          }
        }
      }
    } 
  }
}

///////////////////////End Loop////////////////////////////////

void toSerial() //Write SIM 900 response to serial
{

  while(GPRS.available()!=0)
  {
    Serial.write(GPRS.read());
  }
}
void TriggerEmic()
{
  Serial.println("In coming call!");
  GPRS.end();//Disconnect GPRS S/W Serial so the Emic can use SW Serial
  delay(500);
  digitalWrite(led, HIGH);        // Sets led HIGH to indicate an Incoming call
  DateTime now = rtc.now();

  //delay(100);
  emicSerial.begin(9600);

  // Begin Adafruit TMP36 Demo
  //getting the voltage reading from the temperature sensor
  int reading = analogRead(sensorPin);  

  // converting that reading to voltage, for 3.3v arduino use 3.3
  float voltage = reading * 5.0;
  voltage /= 1024.0; 
  //converting from 10 mv per degree wit 500 mV offset
  float temperatureC = (voltage - 0.5) * 100 ;  
  // now convert to Fahrenheit
  int temperatureF = (temperatureC * 9.0 / 5.0) + 32.0;// make this an interger so the emic will only "Say" the temp as 2 digits
  Serial.print(temperatureF); 
  Serial.println(" Degrees Fahrenheit");
  // End Adafruit TMP36 Demo
  float baromin = bmp.readPressure()* 0.0002953;// Calc for converting Pa to inHg (wunderground)

  // Begin emic AT commands
  emicSerial.print('\n');             // Send a CR in case the system is already up
  while (emicSerial.read() != ':');   // When the Emic 2 has initialized and is ready, it will send a single ':' character, so wait here until we receive it
  delay(10);                          // Short delay
  emicSerial.flush();  
  emicSerial.print("V-5\n"); // Volume
  delay(10);
  emicSerial.print("N2\n"); // Voice (Betty)
  delay(10);
  emicSerial.print("W175\n"); //Speed 
  emicSerial.print('S');// Send the speak command to Emic
  // Speak the Greating and tell the caller the Current Time and Weather Conditions
  // I'm Working on Good morning/Afternoon and Evening Phrases
  if((now.hour() >= 01) &&(now.hour() <= 10)){
    emicSerial.print("Good-Morn-ning,");
  }  
  if((now.hour() >= 10) &&(now.hour() <= 15)){
    emicSerial.print("Good-After-Noon,");
  } 
  if((now.hour() >= 15) &&(now.hour() <= 24)){
    emicSerial.print("Good-Evening,");
  } 
  emicSerial.print("The current time is,");  // Now Send the desired string to convert to speech

  // Convert the time to Speakable words (1-12 Am or PM 
  if(now.hour() < 10){            // Zero padding if value less than 10 ie."09" instead of "9"
    emicSerial.print((now.hour() > 12) ? now.hour() - 12 : ((now.hour() == 0) ? 12 : now.hour()), DEC); // Conversion to AM/PM
    emicSerial.print(":"); 
  }
  else{
    emicSerial.print((now.hour() > 12) ? now.hour() - 12 : ((now.hour() == 0) ? 12 : now.hour()), DEC); // Conversion to AM/PM
    emicSerial.print(":"); 
  }
  // Convert the Minutes to Speakable English
  if(now.minute()<=1){          // if minutes are less than 10
    emicSerial.print("oh-Clock."); // Put an the letters "oh" here so that the emic say's three-oh-five PM/AM. like we would say "3:05 PM/AM"  and not "Three zero five AM/PM".
  }
  // Convert the Minutes to Speakable English
  else if((now.minute()>=1)&&(now.minute()<=9)) {          // if minutes are less than 10
    emicSerial.print("oh,"); // Put an the letters "oh" here so that the emic say's three-oh-five PM/AM. like we would say "3:05 PM/AM"  and not "Three zero five AM/PM".
    emicSerial.print(now.minute());
  }
  else{
    emicSerial.print(now.minute()); //emic will say the minutes as they are read
  }

  if(now.hour() < 12){           // Adding the AM/PM sufffix 
    emicSerial.print(" AM,"); // ie; if less than 12, say AM

  }
  else{
    emicSerial.print(" PM,");//If more than 12, then say PM

  }

  emicSerial.print("And the current Temperature is, ");
  emicSerial.print(temperatureF);
  emicSerial.print(" Degrees Fair-en-Height,");
  emicSerial.print("With a Barometric Pressure of, ");
  emicSerial.print(baromin);
  emicSerial.print("Inches of Mercury,");
  emicSerial.print(" Thank You, for calling the Ardweeno, Time and Temperature Device,");
  emicSerial.print("Good Bye!");
  emicSerial.print('\n');
  while (emicSerial.read() != ':');   // Wait here until the Emic 2 responds with a ":" indicating it's ready to accept the next command
  delay(18500L);    // 12.5 second delay so the emic can speak all the text to the caller.
  emicSerial.end();//Disconnect emicSerial S/W Serial so the GPRS can use S/W Serial
  delay(500);    // 1/2 second delay
  GPRS.begin(19200); //Start the GPRS S/W Serial port up
  digitalWrite(led, LOW);//Turn the LED off to indicate the HANG UP 
  GPRS.println("ATH");  // Send the hang up command
  Serial.print("Total Call's = ");
  Serial.println(totalCalls);
  delay(100);

}













