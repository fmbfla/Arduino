// Example 55.4

#include <SoftwareSerial.h>
SoftwareSerial SIM900(7, 8);
char Serialchar;// Will hold the incoming character from the Serial port
char incoming_char=0;

void setup()
{
  Serial.begin(19200); // for serial monitor
  Serial.println("gsmText.ino");
  SIM900.begin(19200); // for GSM shield
  SIM900power();  // turn on shield
  delay(5000);  // give time to log on to network.

  SIM900.print("AT+CMGF=1\r");  // set SMS mode to text
  delay(100);
  SIM900.print("AT+CNMI=2,2,0,0,0\r"); 
  // blurt out contents of new SMS upon receipt to the GSM shield's serial out
  delay(100);
}

void SIM900power()
// software equivalent of pressing the GSM shield "power" button
{
  digitalWrite(9, HIGH);
  delay(1000);
  digitalWrite(9, LOW);
  delay(7000);
}

void loop()
{
  if(Serial.available() >0){
    Serialchar=Serial.read(); 
    delay(10);

  
    if (Serialchar=='p'){
        Serial.println(" Sending SMS to 9412959244");// Send message to handset to notify the relays are ready
         power_on();
        Serial.println(" Ready to accept commands via SMS..");
        return;
  
    }
    //delay(10);
    if (Serialchar=='o'){

      if (digitalRead(10)==HIGH){

        Serial.println("POWERING OFF");
         delay(1000);
         /*temp();
         SIM900.println(" POWERED DOWN via SMS...");// message to send
         delay(100);
         SIM900.println((char)26); // End AT command with a ^Z, ASCII code 26
         delay(100);
         SIM900.println();*/
        //void power_off(); 
        SIM900.println("AT+CPOWD=1");//Power Down 1=Normal 0=Urgent
        return;
      }
    
    }

  }
  // Now we simply display any text that the GSM shield sends out on the serial monitor
  if(SIM900.available() >0)
  {
    incoming_char=SIM900.read(); //Get the character from the cellular serial port.
    Serial.print(incoming_char); //Print the incoming character to the terminal.
  }
}

void power_on()
// software equivalent of pressing the GSM shield "power" button
{
  SIM900.begin(19200);
  Serial.println("POWERING ON...Please wait");
  digitalWrite(9, HIGH);
  delay(1000);
  digitalWrite(9, LOW);
  delay(7000);
}
