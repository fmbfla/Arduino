#include <SoftwareSerial.h>
#include <Wire.h>
#include <Adafruit_MCP23017.h>
#include <Adafruit_RGBLCDShield.h>

Adafruit_RGBLCDShield lcd = Adafruit_RGBLCDShield();
SoftwareSerial sensor(3, 2);
#define OFF 0x0
#define RED 0x1
#define YELLOW 0x3
#define GREEN 0x2
#define TEAL 0x6
#define BLUE 0x4
#define VIOLET 0x5
#define WHITE 0x7
unsigned char buffer[64]; // buffer array for data recieve over serial port
int count=0;     // counter for buffer array 
void setup(){
  Serial.begin(9600);             // the Serial port of Arduino baud rate.
  sensor.begin(9600);
  lcd.begin(16, 2);
  lcd.setBacklight(YELLOW);
  lcd.setCursor(0, 0);
  //lcd.setCursor(0, 1);
  lcd.print("BMP Xbee test");
  Serial.println("BMP Xbee test");
}
void loop()
{

lcd.clear();
  if (sensor.available())              // if date is comming from softwareserial port ==> data is comming from SIM900 shield
  {
    
    while(sensor.available())          // reading data into char array 
    {

      buffer[count++]=sensor.read();     // writing data into array
      if(count == 64)break;
    }
    Serial.write(buffer,count);            // if no data transmission ends, write buffer to hardware serial port
    Serial.println();
      clearBufferArray();              // call clearBufferArray function to clear the storaged data from the array
    count = 0;   // set counter of while loop to zero
  }
}
void clearBufferArray()              // function to clear buffer array
{
  for (int i=0; i<count;i++)
  { 
    buffer[i]=NULL;
  }    
  uint8_t buttons = lcd.readButtons();

  if (buttons) {
    lcd.clear();
    lcd.setCursor(0,0);
    if (buttons & BUTTON_UP) {
      lcd.setBacklight(RED);
    }
    if (buttons & BUTTON_DOWN) {
      lcd.setBacklight(YELLOW);
    }
    if (buttons & BUTTON_LEFT) {
      lcd.setBacklight(GREEN);
    }
    if (buttons & BUTTON_RIGHT) {
      lcd.setBacklight(TEAL);
    }
    if (buttons & BUTTON_SELECT) {
      lcd.setBacklight(VIOLET);
    }
  }
}



