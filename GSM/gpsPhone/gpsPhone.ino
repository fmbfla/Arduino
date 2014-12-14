#include <SoftwareSerial.h>
SoftwareSerial SIM900(7, 8); // configure software serial port
char inchar; // Will hold the incoming character from the GSM shield
char Serialchar;// Will hold the incoming character from the Serial port
char ATD[]="ATD";
char number[]= "12394639571;";//number to make call to
int pwr_sens = 10;// Dig10 jumper to dtr on seeed GPRS v1 checks for HIGH or LOW (2.8v)
char incoming_char=0;

void setup()
{
  Serial.begin(19200);
  SIM900.begin(19200);               
  Serial.println("GPRS PHONE TEST");

  pinMode(10,INPUT); 
  if (digitalRead(10)==HIGH){
    Serial.println("PHONE is CURRENTLY ON!");
    Serial.println("Press 'o + enter' to TURN OFF");
  }
  if (digitalRead(10)==LOW){
    Serial.println("PHONE is CURRENTLY OFF!");
    Serial.println("Press 'p + enter' to TURN ON");
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
  SIM900.print("AT+CLIP=1\r"); // turn on caller ID notification
  delay(100);
  readSIM();

}

void callSomeone()
{
  Serial.print("Calling ");
  Serial.println(number);
  SIM900.print(ATD);
  SIM900.println(number);
  delay(100);
}

void readSIM(){
 while(SIM900.available())
    Serial.write(SIM900.read());
}


void loop()
{
  readSIM();
  if(Serial.available() >0){
    Serialchar=Serial.read(); 
    delay(10);
    if (Serialchar=='p'){
      if(digitalRead(10)==LOW){
        power_on();
        Serial.println(" Ready to make call..");
      }
      if (digitalRead(10)==HIGH){
        Serial.println("GPRS IS ALREADY ON");
      }
    }
    if (Serialchar=='c'){
      if (digitalRead(10)==HIGH){
        callSomeone();
      }
      if (digitalRead(10)==LOW){
        Serial.print("GPRS IS OFF.");
        Serial.println("POWER UP TO MAKE CALL!");
      }
    }

    if (Serialchar=='h'){
      Serial.println("Hanging Up");
      delay(1000);            // wait for 30 seconds...
      SIM900.println("ATH");   // hang up
    }
    if (Serialchar=='o'){

      if (digitalRead(10)==HIGH){
        Serial.println("POWERING OFF");
        SIM900.println("AT+CPOWD=1");//Power Down 1=Normal 0=Urgent
      }
      //delay(10);
      if (Serialchar=='o'){
        if (digitalRead(10)==LOW){
          Serial.println("GPRS IS ALREADY OFF");
        }
      }
    }
  }
}









