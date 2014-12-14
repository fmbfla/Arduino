#include <SoftwareSerial.h>
#include <TinyGPS.h>
char inchar; // Will hold the incoming character from the GSM shield
char num[]= "AT + CMGS = \"+19412959244\"";//number to send SMS to
TinyGPS gps;
SoftwareSerial GPS(3,2);
SoftwareSerial SIM900(7,8);
void setup()  
{
  Serial.begin(115200);
  Serial.println("smsGoogle Locator using the TinyGPS library test!");
  power_on();
}///////end setUP

void loop()                     // run over and over again
{
  SIM900.begin(19200);
  delay(10);
  if(SIM900.available() >0){
    delay(10);
    inchar=SIM900.read();
    if (inchar=='s'){//read the Status of the relays on request
      Serial.println("Message recieved");
      if (gps.satellites()>3){
      delay(10);
      Message();
      }
      if (gps.satellites()<3){
      delay(10);
      noData();
      }
    }
  }
}///end loop
//////////////////////////////////////////////////////////////////////////////////////////
void Message(){
  SIM900.end();
  delay(10);
  GPS.begin(9600);
  bool newData = false;
  unsigned long chars;
  unsigned short sentences, failed;

  // For one second we parse GPS data and report some key values
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
  delay(10);
  //collect gps data and send SMS to recipient
  float flat, flon;
  unsigned long age;
  gps.f_get_position(&flat, &flon, &age);
  delay(10);
  GPS.end();
  delay(10);
  SIM900.begin(19200);
  delay(100);
  SIM900.println(num);  // recipient's mobile number, in international format
  delay(100);
  //Send SMS Google Map link to recipient 
  SIM900.print("http://maps.google.com/maps?q=");
  SIM900.print(flat, 6);
  SIM900.print(",");
  SIM900.print(flon, 6);
  SIM900.print("%28Location%29&t=k&z=16");
  delay(100);
  SIM900.println((char)26); // End AT command with a ^Z, ASCII code 26
  delay(100);
  SIM900.println();
  delay(100);
  SIM900.println("AT+CMGD=1,4"); // delete all SMS
  delay(100);
  SIM900.flush();
  SIM900.end();
 
 //Debugging with Serial monitor while connected to computer to verify message sent
  Serial.println();
  Serial.print("https://maps.google.com/maps?q=");
  Serial.print(flat, 6);
  Serial.print(",");
  Serial.print(flon, 6);
  Serial.print("%28Drew%29&t=k&z=16");
  Serial.println();
  Serial.println("   Message Sent!");
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
  SIM900.print("AT+CLIP=1\r"); // turn on caller ID notification
  delay(100);
  SIM900.print("AT+CNMI=2,2,0,0,0\r");
  delay(100);
  SIM900.println(num);  // recipient's mobile number, in international format
  delay(100);
  SIM900.print("Ready to Locate!");
  delay(100);
  SIM900.println((char)26); // End AT command with a ^Z, ASCII code 26
  delay(100);
  SIM900.println();
  delay(100);
  SIM900.println("AT+CMGD=1,4"); // delete all SMS
  SIM900.end();
  Serial.println("Ready to Locate!");
  delay(1000);
}

void noData(){
    delay(100);
  SIM900.println(num);  // recipient's mobile number, in international format
  delay(100);
  SIM900.print("No GPS DATA");
  SIM900.print("Tray again in 5 Minutes");
  delay(100);
  SIM900.println((char)26); // End AT command with a ^Z, ASCII code 26
  delay(100);
  SIM900.println();
  delay(100);
  SIM900.println("AT+CMGD=1,4"); // delete all SMS
  SIM900.end();
}
















