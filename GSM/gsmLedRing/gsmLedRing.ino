#include <SoftwareSerial.h>

char inchar;                      // Will hold incoming character from the Serial Port 
SoftwareSerial GPRS(7,8);
// unsigned char buffer[64];         // buffer array for data receive over serial port
int count=0;                      // counter for buffer array 
int WTA = 10;                     // Pin 10 = Wind Talker Announcement Trigger
int numring=0;                   
int comring=2; 

void setup()
{
  GPRS.begin(19200);              // the GPRS baud rate   
  Serial.begin(19200);            // the Serial port of Arduino baud rate.


    /*
  pinMode(9, OUTPUT);             // Power up SIM900 GPRS
  digitalWrite(9,LOW);
  delay(1000);
  digitalWrite(9,HIGH);
  delay(2000);
  digitalWrite(9,LOW);
  delay(1000);*/

  pinMode(WTA, OUTPUT);          // Defines pin 10 as a +5V Digital Output
  digitalWrite(WTA, LOW);        //Sets WT trigger back low
  delay(1000);

  GPRS.println("ATS0=2");        // Set auto ansewer to 2 rings 
}  

void TriggerWTA()
{
  // pinMode(WTA, OUTPUT);        // Defines pin 10 as a +5V Digital Output
  digitalWrite(WTA, HIGH);        // Sets WT trigger HIGH 
  delay(5000);                    // wait for 30 seconds...
  digitalWrite(WTA, LOW);         //Sets WT trigger back low 
  delay(5000);                    // wait for 30 seconds...
  //GPRS.println("ATH");          // hang up
}


void loop()
{
  if(GPRS.available() >0)          //If a character comes in from the cellular module...
  {
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

            numring++;                // So the phone (our GSM shield) has 'rung' once, i.e. if it were a real phone it would have sounded 'ring-ring'
            if (numring==comring)
            {
              numring=0; // reset ring counter
              TriggerWTA();

            }
          }
        }
      }
    } 
  }
}



