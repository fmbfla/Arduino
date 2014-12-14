#include <SoftwareSerial.h>

#include <Wire.h>
#include <Adafruit_BMP085.h>
#include <SD.h>

// On the Ethernet Shield, CS is pin 4. Note that even if it's not
// used as the CS pin, the hardware CS pin (10 on most Arduino boards,
// 53 on the Mega) must be left as an output or the SD library
// functions will not work.
#define aref_voltage 3.3         // we tie 3.3V to ARef and measure it with a multimeter!
SoftwareSerial sensor(2, 3);
int tempPin = 1;        //the analog pin the TMP36's Vout (sense) pin is connected to
int tempReading;        // the analog reading from the sensor
const int ledPin =  13;      // the number of the LED pin

// Variables will change:
int ledState = LOW;             // ledState used to set the LED
long previousMillis = 0;        // will store last time LED was updated

// the follow variables is a long because the time, measured in miliseconds,
// will quickly become a bigger number than can be stored in an int.
long interval = 1000;           // interval at which to blink (milliseconds)

/*************************************************** 
 * This is an example for the BMP085 Barometric Pressure & Temp Sensor
 * 
 * Designed specifically to work with the Adafruit BMP085 Breakout 
 * ----> https://www.adafruit.com/products/391
 * 
 * These displays use I2C to communicate, 2 pins are required to  
 * interface
 * Adafruit invests time and resources providing this open source code, 
 * please support Adafruit and open-source hardware by purchasing 
 * products from Adafruit!
 * 
 * Written by Limor Fried/Ladyada for Adafruit Industries.  
 * BSD license, all text above must be included in any redistribution
 ****************************************************/

// Connect VCC of the BMP085 sensor to 3.3V (NOT 5.0V!)
// Connect GND to Ground
// Connect SCL to i2c clock - on '168/'328 Arduino Uno/Duemilanove/etc thats Analog 5
// Connect SDA to i2c data - on '168/'328 Arduino Uno/Duemilanove/etc thats Analog 4
// EOC is not used, it signifies an end of conversion
// XCLR is a reset pin, also not used here

Adafruit_BMP085 bmp;

void setup() {
  analogReference(EXTERNAL);
  pinMode(ledPin, OUTPUT);
  Serial.begin(9600);
  Serial.println("BMP Xbee test");
  sensor.begin(9600);
  sensor.println("BMP Xbee test");
  if (!bmp.begin()) {
    Serial.println("Could not find a valid BMP085 sensor, check wiring!");
    while (1) {
    }
  }
}

void loop() {

  unsigned long currentMillis = millis();

  if(currentMillis - previousMillis > interval) {
    // save the last time you blinked the LED 
    previousMillis = currentMillis;   

    // if the LED is off turn it on and vice-versa:
    if (ledState == LOW){
      //Serial.print(" Temperature = ");
      tempReading = analogRead(tempPin);  

  //Serial.print("Temp reading = ");
  //Serial.print(tempReading);     // the raw analog reading

  // converting that reading to voltage, which is based off the reference voltage
  float voltage = tempReading * aref_voltage;
  voltage /= 1024.0; 

  // print out the voltage
  //Serial.print(" - ");
  //Serial.print(voltage); Serial.println(" volts");

  // now print out the temperature
  float temperatureC = (voltage - 0.5) * 100 ;  //converting from 10 mv per degree wit 500 mV offset
  //to degrees ((volatge - 500mV) times 100)
  //Serial.print(temperatureC); Serial.println(" degrees C");

  // now convert to Fahrenheight
   float temperatureF = (temperatureC * 9.0 / 5.0) + 32.0;
   //Serial.print("tmp Temperature = ");
  // Serial.print(temperatureF,1); Serial.println(" *F ");
   //Serial.print("bmp Temperature = ");
   Serial.print(temperatureF,1);
   Serial.println(" *F ");
   //Serial.print("Pressure = ");
   Serial.print(bmp.readPressure()* 0.0002953,1);
   Serial.print(" inHg");
   
   
   
   Serial.println();
   //delay(500);  
    ledState = HIGH;
  }
    else
      ledState = LOW;

    // set the LED with the ledState of the variable:
    digitalWrite(ledPin, ledState);

  }
}


