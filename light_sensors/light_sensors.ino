
#include <Wire.h>
#include <Adafruit_BMP085.h>
#include "Adafruit_SI1145.h"
#include <TSL2561.h>
Adafruit_SI1145 uv = Adafruit_SI1145();
Adafruit_BMP085 bmp;
TSL2561 tsl(TSL2561_ADDR_FLOAT); 



void setup() {
  Serial.begin(9600);
 /* if (!bmp.begin()) {
	Serial.println("Could not find a valid BMP085 sensor, check wiring!");
	while (1) {}
  }
  Serial.println("OK found bmp Sensor!");*/
  if (! uv.begin()) {
    Serial.println("Didn't find Si1145 Sensor");
    while (1);
  }

  Serial.println("OK found Si1145 Sensor!");
  
   /* Initialise the sensor */
  if (tsl.begin()) {
    Serial.println("Found TSL sensor");
  } else {
    Serial.println("No sensor?");
    while (1);
  }
  
tsl.setGain(TSL2561_GAIN_16X);      // set 16x gain (for dim situations)
  
  // Changing the integration time gives you a longer time over which to sense light
  // longer timelines are slower, but are good in very low light situtations!
  tsl.setTiming(TSL2561_INTEGRATIONTIME_13MS);  // shortest integration time (bright light)
  //tsl.setTiming(TSL2561_INTEGRATIONTIME_101MS);  // medium integration time (medium light)
  //tsl.setTiming(TSL2561_INTEGRATIONTIME_402MS);  // longest integration time (dim light)
  
  // Now we're ready to get readings!

  Serial.println("");
}
  
void loop() {
    Serial.print("Temperature = ");
    Serial.print(bmp.readTemperature());
    Serial.println(" *F");
    Serial.println("===================");
  Serial.print("Vis: "); Serial.println(uv.readVisible());
  Serial.print("IR: "); Serial.println(uv.readIR());
  
  // Uncomment if you have an IR LED attached to LED pin!
  //Serial.print("Prox: "); Serial.println(uv.readProx());

  float UVindex = uv.readUV();
  // the index is multiplied by 100 so to get the
  // integer index, divide by 100!
  UVindex /= 100.0;  
  Serial.print("UV: ");  Serial.println(UVindex);
    Serial.println();
  // Simple data read example. Just read the infrared, fullspecrtrum diode 
  // or 'visible' (difference between the two) channels.
  // This can take 13-402 milliseconds! Uncomment whichever of the following you want to read
  uint16_t x = tsl.getLuminosity(TSL2561_VISIBLE);     
  //uint16_t x = tsl.getLuminosity(TSL2561_FULLSPECTRUM);
  //uint16_t x = tsl.getLuminosity(TSL2561_INFRARED);
  
  Serial.println(x, DEC);

  // More advanced data read example. Read 32 bits with top 16 bits IR, bottom 16 bits full spectrum
  // That way you can do whatever math and comparisons you want!
  uint32_t lum = tsl.getFullLuminosity();
  uint16_t ir, full;
  ir = lum >> 16;
  full = lum & 0xFFFF;
  Serial.print("IR: "); Serial.println(ir);   
  Serial.print("Full: "); Serial.println(full);  
  Serial.print("Visible: "); Serial.println(full - ir); 
  
  Serial.print("Lux: "); Serial.println(tsl.calculateLux(full, ir));
  
    delay(1000);
}
