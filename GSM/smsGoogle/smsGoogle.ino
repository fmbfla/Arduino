
/*

 Arduino Uno R3
 Seeed Sim900 sheild V-2 connected to dig pins 7 & 8 for use with Software Serial
 Adafruit Flora GPS module connected to dig pins 2 & 3 for use with Software Serial
 GPS lock Status via LED on dig 13
 */
#include <SoftwareSerial.h>
#include <TinyGPS.h>
SoftwareSerial GPS(2,3);
SoftwareSerial SIM900(7,8);

TinyGPS gps;


char senderNumber[20]; 
char inchar; // Will hold the incoming character from the GSM shield
char incoming_char=0;
char num[]= "AT + CMGS = \"+19412959244\"";//number to send SMS to
unsigned char buffer[64]; // buffer array for data recieve over serial port

const int ledPin =  13;
int ledState = LOW; // ledState used to set the LED
int pwr_sens = 10;// Dig10 jumper to dtr on seeed GPRS v1 checks for HIGH or LOW (2.8v)
int count = 0;     // counter for buffer array 
int loops = 0;
int validTXT = 0; //valid received txt counter
int invalidTXT = 0; //invalid received txt counter
int txtOUT = 0; //out going txt counter


float flat, flon;


unsigned long age, date, time, chars = 0;
unsigned short sentences = 0, failed = 0;

////////////////
static void smartdelay(unsigned long ms);
static void print_float(float val, float invalid, int len, int prec);
static void print_int(unsigned long val, unsigned long invalid, int len);
static void print_date(TinyGPS &gps);
static void print_str(const char *str, int len);
/////////////////////

int adding = 2;// used for adding seconds to the interval via serial commands
long previousMillis = 0; // will store last time a Msg was sent
long interval = 60*1000L; // Auto Send interval at which to send Msg (milliseconds)

void setup()  
{
  //pinMode(pwr_sens,OUTPUT);//Dig10 
  pinMode(10,INPUT);
  pinMode(ledPin, OUTPUT);
  Serial.begin(19200);
  SIM900.begin(19200);
  Serial.println("Seeed GPRS Shield Sketch for an Arduino Based Google Map Locator using the TinyGPS library!");
  Serial.println("It's main use is to swnd a text message with the latitude and longitude");
  Serial.println(" of the device to a preset phone number with a link to show the location on GOOGLE MAPS");
  Serial.println();
  Serial.println("A Sixty Second wait time is used on Start Up, This can be changed at line 47 'interval'");
  Serial.println("sending a 'p' through the serial monitor will still activate/de-activate the GPRS during this time");
  //the interval is cjanged automaticaly during the opperation depending upon GPS Satilight lock
  //and on the fly via Serial text or a received SMS from any phone with TXT capability
  Serial.println();
  //power_on(); // enable this line to Automaticaly start the GPRS 
  if (digitalRead(10)==HIGH){//if we get voltage from the 
    Serial.println("GPRS IS ON!");
    Serial.println(" Ready to accept commands via SMS..");
    Serial.println(" Or commands via SERIAL...");
  }
  if (digitalRead(10)==LOW){
    Serial.println("GPRS IS OFF!");
    Serial.println("Ready to accept commands via SERIAL...");
  }

}//////////////////////////////////////// end setup ///////////////////////////////// 

void loop()  // run over and over again
{
  
  
  if (interval==0){
    interval = 1000;
  }
  readSerial();//If connected to a Serial device fo programing or diagnostics (computer)
  wait4TXT();// look for a text message from the GPRS every loop

  //readSIM900();
  //serialPrint();
  /* Serial.print("Speed MPH ");
   Serial.println(gps.f_speed_kmph()*0.621371);
   Serial.print("Speed KPH ");
   Serial.println(gps.f_speed_kmph());
   Serial.print("Altitude ");
   Serial.println(gps.f_altitude());*/
  unsigned long currentMillis = millis();
  if(currentMillis - previousMillis > interval) {
    // save the last time you blinked the LED 
    previousMillis = currentMillis;
    //GpsData();
    Serial.println(loops);
    loops ++;
    Serial.print("interval ");
    Serial.println(interval);
    if (digitalRead(10)==LOW){
      Serial.println("GPSR is OFF!");
    }
    if (digitalRead(10)==HIGH){
      Serial.println("GPSR is ON!");
    }
    if (ledState == LOW)
      ledState = HIGH;
    else
      ledState = LOW;
    digitalWrite(ledPin, ledState);
    //autoSend();

  }
  if (loops == 5){
    Message();
    loops = 0;
  }
  
}////////////////////////////////////////end loop ///////////////////////////////// 


void power_on()
// software equivalent of pressing the GSM shield "power" button
{
  SIM900.begin(19200);
  Serial.println("POWERING ON!!...Please wait");
  digitalWrite(9, HIGH);
  delay(1000);
  digitalWrite(9, LOW);
  delay(7000);
  SIM900.println("AT+CMGF=1\r");  // set SMS mode to text
  delay(100);
  SIM900.println("AT+CNMI=2,2,0,0,0\r");
  delay(100);
  SIM900.println("AT+CLIP=1\r"); // turn on caller ID notification
  delay(100);
  SIM900.println(num);
  delay(100);
  SIM900.print("GPRS is Active!");
  delay(100);
  SIM900.println((char)26); // End AT command with a ^Z, ASCII code 26
  delay(100);
  SIM900.println();
  delay(100);
  SIM900.println("AT+CMGD=1,4"); // delete all SMS
  //readSIM900();
  delay(5000);
  Serial.println("GPRS is NOW ON!");
  //SIM900.flush();
  SIM900.end();
  return;

}
void power_off(){
  SIM900.begin(19200);
  Serial.println("POWERING OFF!!...Please wait");
  SIM900.println("AT+CMGF=1\r");  // set SMS mode to text
  delay(100);
  SIM900.print("AT+CNMI=2,2,0,0,0\r");
  delay(100);
  SIM900.println(num);
  delay(100);
  SIM900.print("GPRS is Powering Down!");
  delay(100);
  SIM900.println((char)26); // End AT command with a ^Z, ASCII code 26
  delay(100);
  SIM900.println();
  delay(2000);//give some time to send message
  SIM900.println("AT+CMGD=1,4"); // delete all SMS
  SIM900.flush();
  SIM900.end();
  //readSIM900();
  delay(1000);
  digitalWrite(9, HIGH);
  delay(1000);
  digitalWrite(9, LOW);
  delay((7000));
  Serial.println("GPRS is NOW OFF!");
  Serial.println();
  return;
}
void readSIM900(){
  SIM900.begin(19200);
  if (SIM900.available())              // if date is comming from softwareserial port ==> data is comming from SIM900 shield
  {
    while(SIM900.available())          // reading data into char array 
    {
      buffer[count++]=SIM900.read();     // writing data into array
      if(count == 64)break;
    }
    Serial.write(buffer,count);            // if no data transmission ends, write buffer to hardware serial port
    clearBufferArray();              // call clearBufferArray function to clear the storaged data from the array
    count = 0;                       // set counter of while loop to zero


  }
  if (Serial.available())            // if data is available on hardwareserial port ==> data is comming from PC or notebook
    SIM900.write(Serial.read());       // write it to the SIM900 shield
  delay(100);
  SIM900.flush();
  SIM900.end();
}
void clearBufferArray()              // function to clear buffer array
{
  for (int i=0; i<count;i++)
  { 
    buffer[i]=NULL;
  }                  // clear all index of array with command NULL
}
void readSerial(){
  if (Serial.available() >0){
    incoming_char=Serial.read();
    if ((incoming_char=='p')&& (digitalRead(10)==HIGH)){
      Serial.println("ATTENTION!!!..Turning OFF!");
      power_off();
      return;
    }
    else  if ((incoming_char=='p')&& (digitalRead(10)==LOW)){
      Serial.println("ATTENTION!!!..Turning ON!");
      power_on();
      return;
    }
    if (incoming_char=='s'){
      Message();
    }

    if (incoming_char=='+'){
      adding++;
      interval = adding*1000;
      Serial.println("Adding ");
    }
    if (incoming_char=='-'){
      adding--;
      if (interval==1000){
        Serial.println("Cant go lower than 1000! ");
      }
      else
        Serial.println("Subtracting ");
      interval = adding*1000;
      adding--;
      
    }
  }
  Serial.flush();
}

//Wait for TXT message
void wait4TXT(){

  SIM900.begin(19200);
  delay(10);
  if (SIM900.available() >0){
    incoming_char=SIM900.read();

    if (incoming_char=='s'){//read the Status of the relays on request
      validTXT++; //count the incomming txt from the GPRS
      Serial.println("Text Message recieved ");
      delay(10);
      Message();// send message to pre-defined mobile number
      delay(10);
      SIM900.flush();
      SIM900.end();
      delay(10);
    }
    else invalidTXT++;// count the flubs
  }
}
//Sends TXT Message with Google Map link to pre-defined mobile number 
void Message(){
  GpsData();
  txtOUT ++ ;
  //delay(10);
  SIM900.begin(19200);//and reconnect to SS so we can send the message with the lat/long cords
  SIM900.print("AT+CMGF=1\r");  // set SMS mode to text
  delay(100);
  SIM900.println(num);
  delay(100);
  // this is the main part
  SIM900.println("Text Received");
  SIM900.println(" ");
  SIM900.print("http://maps.google.com/maps?q="); //creating the link to GM
  SIM900.print(flat, 6);//Latitude with 6 decimal points
  SIM900.print(",");
  SIM900.println(flon, 6);// Longitude with 6 decimal points
  //Below should create a pop up baloon with text "Location" but I cant get it to do so
  SIM900.print("%28Location%29&t=k&z=16");
  delay(10);
  SIM900.println((char)26); // End AT command with a ^Z, ASCII code 26
  delay(10);
  SIM900.println();
  delay(10);
  SIM900.println("AT+CMGD=1,4"); // delete all SMS
  delay(10);
  SIM900.flush();
  SIM900.end();
  Serial.println("   Message Sent!");
  Serial.println();
  //Debugging with Serial monitor while connected to computer to verify message sent
  serialPrint();

}

//get GPS DATA
void GpsData(){
  //readSerial();
  //digitalWrite(led, HIGH);
  GPS.begin(9600);
  delay(10);
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
    /* Serial.print("LAT=");
     Serial.print(flat == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flat, 6);
     Serial.print(" LON=");
     Serial.print(flon == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flon, 6);
     Serial.print(" SAT=");
     Serial.print(gps.satellites() == TinyGPS::GPS_INVALID_SATELLITES ? 0 : gps.satellites());
     Serial.print(" PREC=");
     Serial.print(gps.hdop() == TinyGPS::GPS_INVALID_HDOP ? 0 : gps.hdop());*/
    serialPrint();
    interval= 60*1000L;
  }

  gps.stats(&chars, &sentences, &failed);

  if (chars == 0)
    Serial.println("** No characters received from GPS: check wiring **");
  if (sentences == 0){
    /*Serial.print(" CHARS=");
     Serial.print(chars);
     Serial.print(" SENTENCES=");
     Serial.print(sentences);
     Serial.print(" CSUM ERR=");
     Serial.println(failed);
     Serial.println();*/
    Serial.println("GPS is Connected, Waiting for SATELLITE Lock");
    Serial.print("Sat's Locked in. ");
    Serial.println(gps.satellites() == TinyGPS::GPS_INVALID_SATELLITES ? 0 : gps.satellites());
    // Serial.println("Check GPS location for obstructions to clear skies");
    Serial.println();
    //interval= 1000L;
    delay(10);
  }
  GPS.end();
}

void autoSend(){
  GpsData();
  //if (gps.f_speed_kmph()>=1.0)
  Serial.println("Automatic location report!");
  SIM900.begin(19200);
  SIM900.println("AT + CMGS = \"+19412959244\"");
  SIM900.println("AT + CMGS = \"+"); 
  SIM900.println(num);  // pre-defined mobile number, in international format
  SIM900.println("\"");
  delay(100);
  SIM900.println("Automatic location report! (60 second)");
  SIM900.println(" ");
  SIM900.print("http://maps.google.com/maps?q="); //creating the link to GM
  SIM900.print(flat, 6);//Latitude with 6 decimal points
  SIM900.print(",");
  SIM900.println(flon, 6);// Longitude with 6 decimal points
  //Below should create a pop up baloon with text "Location" but I cant get it to do so
  SIM900.print("%28Location%29&t=k&z=16");
  delay(10);
  delay(100);
  SIM900.println((char)26); // End AT command with a ^Z, ASCII code 26
  delay(100);
  SIM900.println();
  //readSIM900();
  delay(100);
  SIM900.println("AT+CMGD=1,4"); // delete all SMS
  delay(100);
  Message();
  delay(100);
  //serialPrint();
  SIM900.flush();
  SIM900.end();
}



//Serial Monitor if connected comment out to save some space
void serialPrint(){
  //float flat, flon;
  Serial.print(" ");// to even the serial print out
  Serial.print(flat, 4);
  Serial.println(" N");
  Serial.print(flon, 4);
  Serial.println(" W");
   Serial.print(gps.f_speed_kmph());
  Serial.println(gps.f_course());
}






























