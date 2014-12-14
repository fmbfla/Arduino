#include <Wire.h>
#include <SoftwareSerial.h>
#include <Adafruit_MCP23017.h>
#include <Adafruit_RGBLCDShield.h>
// The shield uses the I2C SCL and SDA pins. On classic Arduinos
// this is Analog 4 and 5 so you can't use those for analogRead() anymore
// However, you can connect other I2C sensors to the I2C bus and share
// the I2C bus.
Adafruit_RGBLCDShield lcd = Adafruit_RGBLCDShield();
#define OFF 0x0
#define RED 0x1
#define YELLOW 0x3
#define GREEN 0x2
#define TEAL 0x6
#define BLUE 0x4
#define VIOLET 0x5
#define WHITE 0x7
String input=""; // Declare an Empty String Variable
String input_trimmed="";// a variable to store the trimmed string
//sensors
String time=""; // a variable to store the substring 
String date="";
String tempF="";
String baro ="";
String uv ="";
String humi ="";
// variables for storing the string lengths
int length_before_trim = 0, length_after_trim = 0, length_time=0,length_date = 0, length_tempF = 0, length_baro = 0, length_uv = 0, length_humi = 0;

SoftwareSerial mySerial(3,2);//defaut 

void setup()
{
  lcd.begin(16, 2);
  lcd.setBacklight(RED);
  Serial.begin(115200); // Initialise Serial Communication
  Serial.println("lcd xbee receive strings" +input);
  mySerial.begin(9600);
}
uint8_t i=0;
void loop()
{
  uint8_t buttons = lcd.readButtons();

  if (buttons) {

    lcd.setCursor(0,0);
    if (buttons & BUTTON_UP) {
      lcd.clear();
      lcd.setBacklight(GREEN);
      tempF = input_trimmed.substring(20,32);// this is what you want out of the string
      lcd.setCursor(0,0);
      lcd.print(tempF);
      humi = input_trimmed.substring(33,45);// this is what you want out of the string
      lcd.setCursor(0,1);
      lcd.print(humi);
      delay(5000);
      lcd.clear();

    }
    if (buttons & BUTTON_DOWN) {
      lcd.setBacklight(YELLOW);
      lcd.clear();
      baro = input_trimmed.substring(45,55);// this is what you want out of the string
      lcd.setCursor(0,0);
      lcd.print("baro");
      lcd.setCursor(0,1);
      lcd.print(baro);
      Serial.println(baro);
      delay(5000);
      lcd.clear();  
    }
    if (buttons & BUTTON_LEFT) {
      lcd.setBacklight(GREEN);
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("uv");
      uv = input_trimmed.substring(55,69);// this is what you want out of the string
      lcd.setCursor(0,1);
      lcd.print(uv);
      Serial.println(uv);
      delay(5000);
      lcd.clear();
    }
    if (buttons & BUTTON_RIGHT) {
      lcd.print("RIGHT ");
      lcd.setBacklight(TEAL);
    }
    if (buttons & BUTTON_SELECT) {
      lcd.print("SELECT ");
      lcd.setBacklight(VIOLET);
    }
  }
  if(mySerial.available()) // Check if there is incoming data in the Serial Buffer
  {
    input = ""; // reset the input variable
    while(mySerial.available()) // Process incoming data till the buffer becomes empty
    {   

      input += (char)mySerial.read();// Read a byte of data, convert it to character and append it to the string variable
      delay(1);
    }

    //Use the following to locate the data you need in the string by counting the chars
    Serial.println();
    input_trimmed = input; // assign the trimmed variable with the input value
    input_trimmed.trim();

    length_before_trim = input.length();
    length_after_trim = input_trimmed.length();
    //length_substring = substring.length();
    Serial.println("Your Original Input : " +input);
    Serial.print("Length of Your Original Input : ");
    Serial.println(length_before_trim);
    Serial.println("The Trimmed Input : " +input_trimmed);
    Serial.print("Length of Trimmed Input : ");
    Serial.println(length_after_trim);
    //Serial.println("Substring : " +substring);
    //Serial.print("Length of Substring : ");
    // Serial.println(length_substring);

    lcd.setCursor(0,0);
    lcd.setBacklight(GREEN);
    time = input_trimmed.substring(0,8);
    lcd.print(time);
    date = input_trimmed.substring(9,19);
    lcd.setCursor(0,1);
    lcd.print(date);
    //Serial.println(date);

  }
  //delay(1000);
  //lcd.clear();
}



