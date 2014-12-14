



//Serial Relay - Arduino will patch a 
//serial link between the computer and the SIM900 Shield
//at 19200 bps 8-N-1
//Computer is connected to Hardware UART
//SIM900 Shield is connected to the Software UART 
/*_______________________________________
    To Read the SIM900 RTC clock
    AT+CCLK?
    To set the SIM900 TIME AND DATE
    AT+CCLK="14/09/10,21:34:30+05" 
_______________________________________*/

 #include <SoftwareSerial.h>
//SoftwareSerial SIM900(10, 11);// arduino MEGA 2560 
SoftwareSerial SIM900(7, 8);// arduino UNO 
unsigned char buffer[64]; // buffer array for data recieve over serial port
int count=0;     // counter for buffer array 
void setup()
{
  SIM900.begin(19200);               // the SIM900 baud rate   
  Serial.begin(19200);             // the Serial port of Arduino baud rate.
 Serial.println("GSM Serial test");
   Serial.println("POWERING ON!!...Please wait");
  digitalWrite(9, HIGH);
  delay(1000);
  digitalWrite(9, LOW);
  delay(7000);
  SIM900.flush();
  delay(100);
  Serial.println("Ready!");
}
 
void loop()
{
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
}
void clearBufferArray()              // function to clear buffer array
{
  for (int i=0; i<count;i++)
    { buffer[i]=NULL;}                  // clear all index of array with command NULL
}
