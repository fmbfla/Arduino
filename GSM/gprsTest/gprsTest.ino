/*Note: this code is a demo for how to using GPRS Shield to send SMS message and dial a voice call.
 
 The microcontrollers Digital Pin 7 and hence allow unhindered communication with GPRS Shield using SoftSerial Library. 
 IDE: Arduino 1.0 or later */

#include <GSM.h>
//#include <SoftwareSerial.h>
//#include <String.h>

//SoftwareSerial sms(7,8);

// initialize the library instances
GSM gsmAccess;
GSM_SMS sms;

char senderNumber[20]; 
void setup()
{
  digitalWrite(9, HIGH);
  delay(1000);
  digitalWrite(9, LOW);
  delay((7000));
   //sms.begin(19200);               // the GPRS baud rate   
  Serial.begin(19200);    // the GPRS baud rate 
  delay(500);
  sms.println("ATS0=12");// set to auto answer to twelve rings then answer
  delay(500);
  sms.println("AT+CALS=10");// set the ring tone type
  delay(500);
  sms.println("AT+CRSL=3");// set the ring tone volume
  delay(500);
  sms.println("AT+CLVL=99");// set the speaker volume
  delay(500);
  Serial.println("Send a Lower case 't' through the serial monitor to send a text message");
}

void loop()
{
  char c;
  
  // If there are any SMSs available()  
  if (sms.available())
  {
    Serial.println("Message received from:");
    
    // Get remote number
    sms.remoteNumber(senderNumber, 20);
    Serial.println(senderNumber);

    // An example of message disposal    
    // Any messages starting with # should be discarded
    if(sms.peek()=='#')
    {
      Serial.println("Discarded SMS");
      sms.flush();
    }
    
    // Read message bytes and print them
    while(c=sms.read())
      Serial.print(c);
      
    Serial.println("\nEND OF MESSAGE");
    
    // Delete message from modem memory
    sms.flush();
    Serial.println("MESSAGE DELETED");
  }

  delay(1000);

}

///SendTextMessage()
///this function is to send a sms message
void SendTextMessage()
{
  sms.print("AT+CMGF=1\r");    //Because we want to send the SMS in text mode
  delay(100);
  sms.println("AT+CMGS = \"+19412959244\"");//send sms message, be careful need to add a country code before the cellphone number
  delay(100);
  sms.println("adafruit.com");//the content of the message
  delay(100);
  sms.println((char)26);//the ASCII code of the ctrl+z is 26
  delay(100);
  sms.println();
  sms.println("AT+CMGD=1,4");
}

///DialVoiceCall
///this function is to dial a voice call
void DialVoiceCall()
{
  sms.println("ATD + + 12394639571;");//dial the number
  delay(100);
  sms.println();
}

void ShowSerialData()
{
  while(sms.available()!=0)
    Serial.write(sms.read());
}




