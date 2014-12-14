
/*
 * This MODIFIED sketch connects to The wunderground REALTIME weather website @ (http://www.rtupdate.wunderground.com)
 * and is using 50% recycled electrons from Florida Power and Light (FPL)
 * and 50% New electrons from the SUN using a solar cell to capture those really big fuzzy ones :ÃƒÅ½Ã‚Â©P
 * Using an Arduino Wiznet Ethernet shield To upload your weather conditions to wunderground.com,
 * We make a standard HTTP GET request with the ID, PASSWORD and weather conditions as GET parameters.
 * You must first create an account with wunderground for this to werk!
 * The anemometer/windvane are from ebay looke for a cheap one and upgrade it or make your own
 * code for the Anemometer from: Authors: M.A. de Pablo & C. de Pablo S. on there home-made version af an anomometer
 * Various notes throughout attempt to explain why i did what i did to make it werk.
 * This sketch also incorporates an NTP RTC update code to make sure that the time is always correct and
 * resets the RTC Soft or Hard to UTC ZULU to comp for time drift, (Wunderground wants "UTC ZULU HOUR" for RTU).
 * Results for this can be monitored @   http://www.wunderground.com/personal-weather-station/dashboard?ID=KFLFORTM67
 
 Binary sketch size: 28,574 bytes (of a 32,256 byte maximum) as of 11/21/2014
 Binary sketch size: 28,314 bytes (of a 32,256 byte maximum) with Serial data ommited; 
 misc links,
 * http://en.wikipedia.org/wiki/Dew_point Dew point Calc
 * http://wiki.wunderground.com/index.php/PWS_-_Upload_Protocol Information on the Upload Protocol for wunderground.
 
 
 Circuit:
 * Arduino UNO adafruit http://www.adafruit.com/products/50
 * Ethernet shield attached to pins 10, 11, 12, 13 http://www.adafruit.com/products/201
 * DHT to pin A0 adafruit http://www.adafruit.com/products/393
 * Wind direction to pin A1 this is a DAVIS instrument which incorperates the anemometer (or make a home made one using a 360 var resistor)
 * Anemometer to Digtal 2 (interrupt 0)this is a DAVIS instrument (reed switch) 
 * and it looks like this http://www.skygeek.com/davis-instrument-7890-anemometer-mast-mount.html?utm_source=googlebase&utm_medium=shoppingengine&utm_content=davis-instrument-7890-anemometer-mast-mount&utm_campaign=froogle&gclid=CLLugIr107wCFbB9Ogodi14AiA
 * Rain tip to Digital 3 (interrupt 1) (hall effect sensor) http://www.adafruit.com/products/158
 * BMP to I2C adafruit https://www.adafruit.com/products/391
 * RTC to I2C adafruit https://www.adafruit.com/products/264
 **
 
 Modified  To upload weather conditions to wunderground.com
 1,Jan 2014
 By the drunkin programer
 *
 */


#include "SPI.h"
#include "Ethernet.h"
#include "Wire.h"
#include "DHT.h"
#include "Adafruit_BMP085.h"
#include "Adafruit_SI1145.h"
#include "RTClib.h"
#include "OneWire.h"
#include "DallasTemperature.h"

// Data wire is plugged into port 2 on the Arduino

#define DHTPIN A0
#define DHTTYPE DHT22                // DHT 22  (AM2302)
// Pins
#define vane A1              // Wind direction
#define soil A2
#define leaf A3
#define anemometer  2         // D2 Wind speed
//#define rainTip  3            // D3 Rain bucket
#define ONE_WIRE_BUS 4
#define led  6              // Visual Notification for whatever

#define RTU 1850//2000 ms sometimes puts wunderground posts into the 4 second range
//IPAddress SERVER(38,102,137,157);  // numeric IP for wunderground.com
char SERVER[] = "rtupdate.wunderground.com";           // Realtime update server
//char SERVER [] = "weatherstation.wunderground.com";  //standard server
char WEBPAGE [] = "GET /weatherstation/updateweatherstation.php?";
char ID [] = "KFLFORTM67";
char PASSWORD [] = "XXXXXXX";

byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };//Hard config for ethernet card mac addy

// assign an IP address for the controller:
IPAddress ip(192,168,1,110);
IPAddress gateway(192,168,1,1);	
IPAddress subnet(255, 255, 255, 0);
EthernetClient client;

const unsigned long period = 1000;// Measurement period for the anemometer(miliseconds)
const unsigned long IDLE_TIMEOUT_MS = 1000; // Time to wait for the WunderGround server to respond
const int radio = 60;             // Radius from center of Anemometer to center of one of the Anemometer cups

unsigned int loops = 0; 
unsigned int counter = 0;         // pulse count for wind sensor 
unsigned int RPM = 0;             // Revolutions per minute for wind sensor
unsigned int winddir = 0.0;       // Wind direction for anemometer
unsigned int rainin = 0;          // inches of rain  
unsigned int dailyrainin = 0;     // dailyRain
unsigned int Sample = 0;       // Sample number
float windspeedmph = 0/ 0.445;    // Wind speed (m/s) for wind sensor
float windgustmph = 0;            // Max wind speed for wind sensor
const float pi = 3.14159265;      //RPM Calcs

Adafruit_SI1145 uv = Adafruit_SI1145();// UV Sensor
EthernetUDP Udp;                  // UDP for the time server
DHT dht(DHTPIN, DHTTYPE);         // DHT 22  (AM2302)
Adafruit_BMP085 bmp;              // BMP Sensor


OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
DeviceAddress insideThermometer;
RTC_DS1307 rtc; //USe for Hardware RTC time
//RTC_Millis rtc; //Use for Software RTC time
void setup(void)  
{  

  Serial.begin(115200);
  bmp.begin();//adafruit BMP sensor
  dht.begin();//adafruit DHT22 sensor
  uv.begin();//adafruit SI1145 U.V. sensor
  sensors.begin();//dallas one wire
  rtc.begin();//Use if a Hardware RTC device is connected
  //rtc.adjust(DateTime(__DATE__, __TIME__));

  //****************Software RTC functions****************************
  // If a Using Software RTC
  
  // rtc.begin(DateTime(F(__DATE__), F(__TIME__))); 
  
  //****************Software RTC functions****************************
  
  pinMode(led, OUTPUT);
  pinMode(anemometer, INPUT); //Anemometer input(spins like mad when the wind blows).
  digitalWrite(anemometer, HIGH); // set this high so when it detects a change it records it via INT(0)
  // Tells me whats Loaded on the arduino Chip if I change it out
  Serial.println("Mega Wunderground Sketch");

  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet");
    Serial.println("Check the Ethernet shield, CAT5 Cable and or the WIFI Repeter, Router");
    while (1);
  }
  Serial.println("Ethernet shield Ready");
  Serial.print("Local I.P: ");
  Serial.println(Ethernet.localIP());

}
// ______________________________End of the Setup Function_______________________________
void loop(void)
{
  oneWire.reset_search();
  //DateTime now = rtc.now();

  //delay(1000);

  //int time = now.second();
  // Serial.print("Starting Transmission "); Serial.print(time); Serial.println(" seconds");
  //Serial.println("_______________");
  //Serial.print("Begin Process  ");
  //Serial.print(now.second());
  // Serial.println(" Seconds");
  windvelocity();
  RPMcalc();
  WindSpeed();
  Heading();

  // Send the Data
  sendData();
PrintSerial();
  //used for checking sensor data 
  //PrintSerial();
  // max wind speed holder(wunderground)
  if (windspeedmph > windgustmph) {
    windgustmph = windspeedmph;
  }


  client.stop();  // Stop/disconnect the ethernet Client
  delay(RTU);// 2000sometimes puts wunderground posts into the 4 second range
}// end loop

void sendData(){
  DateTime now = rtc.now();
  //nuts and bolts
  float UVindex = uv.readUV();// Uv Index Provided by an Adafruit Si1145 UV/IR/Visible Light Sensor
  UVindex /= 100.0;
  float humidity = dht.readHumidity(); 
  float baromin = bmp.readPressure()* 0.0002953;// Calc for converting Pa to inHg (wunderground)
  float c = dht.readTemperature();//Centigrade reading to calculate the DewPoint
  int soilmoisture = analogRead(soil);
  int lumin = (uv.readVisible()*0.09290304);
  int Solar = (lumin * UVindex);
  float tempf = dht.readTemperature(true);//Returns value in Fahrenheit
  int leafwetness = analogRead(A3);
 float soiltempf =(sensors.getTempCByIndex(0) * 9.0 / 5.0 + 32.0);
  soilmoisture = map(soilmoisture, 0, 1023, 0, 99);
  soilmoisture = 99 - soilmoisture; 
  leafwetness = map(leafwetness, 0, 1023, 0, 15);
  leafwetness = 15 - leafwetness; 

  double VaporPressureValue = humidity * 0.01 * 6.112 * exp((17.62 * c) / (c + 243.12));
  double Numerator = 243.12 * log(VaporPressureValue) - 440.1;
  double Denominator = 19.43 - (log(VaporPressureValue));
  double dewptf = Numerator / Denominator * 9.0 / 5.0 + 32;

  if (client.connect(SERVER, 80)) { 
    digitalWrite(led, HIGH);
    client.print(WEBPAGE); 
    client.print("ID=");
    client.print(ID);
    client.print("&PASSWORD=");
    client.print(PASSWORD);
    client.print("&dateutc=");
    client.print(now.year());
    client.print("-");
    client.print(now.month());
    client.print("-");
    client.print(now.day());
    client.print("+");
    client.print(now.hour()); 
    client.print("%3A");
    client.print(now.minute());
    client.print("%3A");
    client.print(now.second());
    client.print("&winddir=");
    client.print(winddir);
    client.print("&windspeedmph=");
    client.print(windspeedmph,1);
    client.print("&windgustmph=");
    client.print(windgustmph,1);
    client.print("&windgustmph_10m=");
    client.print(windgustmph,1);//[mph past 10 minutes wind gust mph ]
    client.print("&tempf=");
    client.print(tempf);
    client.print("&dewptf=");
    client.print(dewptf);
    client.print("&humidity=");
    client.print(humidity);
    client.print("&rainin=");
    client.print(rainin / 2.0 / 100.0);
    client.print("&dailyrainin=");
    client.print(dailyrainin / 2.0 / 100.0);
    client.print("&baromin=");
    client.print(baromin,1);
    client.print("&UV=");
    client.print(UVindex);
    client.print("&soiltempf=");
    client.print(soiltempf);
    client.print("&soilmoisture=");
    client.print(soilmoisture);
    client.print("&solarradiation=");
    client.print(Solar);
    client.print("&leafwetness=");
    client.print(leafwetness);// ADDED THE ZEORO TO DROP THE POINT ON WUNDERGROUD
    //client.print("&indoortempf="); 
    //client.print("&indoorhumidity=");
    client.print("&softwaretype=Arduino%20UNO%20version1&action=updateraw&realtime=1&rtfreq=2.5");//Rapid Fire
    client.println();


  }
  else {
    Serial.println(F("Connection failed"));  
    delay(2000);  
    return;
  }
  //Serial.println("Server Response!");
  unsigned long lastRead = millis();
  while (client.connected() && (millis() - lastRead < IDLE_TIMEOUT_MS)) {
    while (client.available()) {
      char c = client.read();
      Serial.print(c);
      lastRead = millis();
      digitalWrite(led, LOW); //Blinks while recieving responce
    }
  }

}



// For debugging
void PrintSerial(){
  DateTime now = rtc.now();
  float UVindex = uv.readUV();// Uv Index Provided by an Adafruit Si1145 UV/IR/Visible Light Sensor
  UVindex /= 100.0 ;
  int lumin = (uv.readVisible()*0.09290304);
  int Solar = (lumin * UVindex);
  //float tempf = dht.readTemperature();//Returns value in celsius 
  float tempf = dht.readTemperature(true);//Returns value in Fahrenheit
  float humidity = dht.readHumidity(); 
  float baromin = bmp.readPressure()* 0.0002953;// Calc for converting Pa to inHg (wunderground)
  float c = dht.readTemperature();//Centigrade reading to calculate the DewPoint
  double VaporPressureValue = humidity * 0.01 * 6.112 * exp((17.62 * c) / (c + 243.12));
  double Numerator = 243.12 * log(VaporPressureValue) - 440.1;
  double Denominator = 19.43 - (log(VaporPressureValue));
  double dewptf = Numerator / Denominator * 9.0 / 5.0 + 32;  // Send the Data

  int soilmoisture = analogRead(A3);
  soilmoisture = map(soilmoisture, 0, 1023, 0, 99);
  soilmoisture = 99 - soilmoisture; 
  int leafwetness = analogRead(leaf);
  leafwetness = map(leafwetness, 0, 1023, 0, 15);
  leafwetness = 15 - leafwetness;

  // for Debug, or you can sit up all night watching it.
  Serial.println();
  Serial.println("______debugging______");
  Serial.print("Loops: ");
  Serial.print("\t");
  Serial.println(loops);
  Serial.print("wind dir:");
  Serial.print("\t");
  Serial.print(winddir);
  Serial.println(" deg's");
  Serial.print("wind:");
  Serial.print("\t\t");
  Serial.println(windspeedmph,1);
  Serial.print("wind gust mph:");
  Serial.print("\t");
  Serial.println(windgustmph,1);
  Serial.print("Outside temp:");
  Serial.print("\t");
  Serial.print(tempf);
  Serial.println(" *F");
  Serial.print("Soil Temp ");
  Serial.print("\t");
  Serial.println(sensors.getTempCByIndex(0) * 9.0 / 5.0 + 32.0);
  Serial.print("Internal Temp ");
  Serial.print("\t");
  Serial.print(bmp.readTemperature());
  Serial.println(" *F");
  Serial.print("Dew point:");
  Serial.print("\t");
  Serial.print(dewptf);
  Serial.println(" *F");
  Serial.print("Humidity:");
  Serial.print("\t");
  Serial.print(humidity);
  Serial.println(" %");
  Serial.print("Rain:");
  Serial.print("\t\t");
  Serial.print(rainin / 2.0 / 100.0);
  Serial.println(" in");
  Serial.print("Daily rain:");
  Serial.print("\t");
  Serial.print(dailyrainin / 2.0 / 100.0);
  Serial.println(" in");
  Serial.print("barometer:");
  Serial.print("\t");
  Serial.print(baromin,1 );
  Serial.println(" inHg");
  Serial.print("U.V."); 
  Serial.print("\t\t"); 
  Serial.println(UVindex);
  Serial.print("Solar Power: ");
  Serial.print("\t");
  Serial.print(Solar);
  Serial.println(" W/m^2");
  Serial.print("Soil moisture: ");
  Serial.print("\t");
  Serial.print(soilmoisture,1);
  Serial.println(" %");
  Serial.print("Soil temp: ");
  Serial.print("\t");
  Serial.print(sensors.getTempCByIndex(0) * 9.0 / 5.0 + 32.0,1);
  Serial.println(" *F");
  Serial.print("Leaf wetness: ");
  Serial.print("\t");
  Serial.print(leafwetness,1);
  Serial.println(" %");
  Serial.print("Time: ");
  Serial.print("\t");
  if(now.hour() < 10){                // Zero padding if value less than 10 ie."09" instead of "9"
    Serial.print(" ");
    Serial.print((now.hour()> 12) ? now.hour() - 12 : ((now.hour() == 0) ? 12 : now.hour()), DEC); // Conversion to AM/PM  
  }
  else{
    Serial.print((now.hour()> 12) ? now.hour() - 12 : ((now.hour() == 0) ? 12 : now.hour()), DEC); // Conversion to AM/PM
  }
  Serial.print(':');
  if(now.minute() < 10){         // Zero padding if value less than 10 ie."09" instead of "9"
    Serial.print("0");
    Serial.print(now.minute(), DEC);
  }
  else{
    Serial.print(now.minute(), DEC);
  }
  Serial.print(':');
  if(now.second() < 10){         // Zero padding if value less than 10 ie."09" instead of "9"
    Serial.print("0");
    Serial.print(now.second(), DEC);
  }
  else{
    Serial.print(now.second(), DEC);
  }
  if(now.hour() < 12){                  // Adding the AM/PM sufffix
    Serial.println(" AM");
  }
  else{
    Serial.println(" PM");
  }
    // Clear the Daily rain amount 
  if ((now.hour()==24)  && (now.minute()==0) && (now.second() < 10 )){
    dailyrainin = 0;
    //rainCount = 0;
    rainin = 0;
    windgustmph = 0;
    //ntp();
  }
  loops ++;// used to reset Max Wind Speed
  if (loops == 5){ //clear out the max wind speed every so often 
    loops = 0;
    windgustmph = 0;
  }
  Serial.println("__End_debugging______");
  Serial.println();
}

//Wind Speed Trigger/timer
void windvelocity(){

  windspeedmph = 0;
  counter = 0;
  attachInterrupt(0, addcount, RISING);//anemometer
  unsigned long millis();                     
  long startTime = millis();
  while(millis() < startTime + period) {
  }
  detachInterrupt(0);

}
//RPM calc needed for wind speed calc
void RPMcalc(){
  RPM=((counter*4)*60)/(period/1000);  // Calculate revolutions per minute (RPM)
}
void WindSpeed(){
  windspeedmph = ((2 * pi * radio * RPM)/60) / 1000;
}
void addcount(){
  counter++;
}
// Voltage to Compass Degree conversion
void Heading(){
  float windDir =(analogRead(vane)* 5.00 / 1023.0);    // Wind direction                
  if ((windDir > 4.94)||(windDir < 0.01))
    (winddir = 0.0);// North
  if ((windDir >= 0.02)&&(windDir < 0.30))
    (winddir = 22.5);//NNE
  if ((windDir >= 0.31)&&(windDir < 0.72))
    (winddir = 45.0);//NE
  if ((windDir >= 0.72)&&(windDir < 0.99))
    (winddir= 67.5);//ENE
  if ((windDir >= 1.00)&&(windDir < 1.25))
    (winddir = 90.0);//E
  if ((windDir >= 1.26)&&(windDir < 1.52))
    (winddir= 112.5);//ESE
  if ((windDir >= 1.53)&&(windDir < 1.91))
    (winddir= 135.0);//SE
  if ((windDir >= 1.92)&&(windDir < 2.40))
    (winddir = 157.5);//SSE
  if ((windDir >= 2.41)&&(windDir < 2.73))
    (winddir = 180.0);//S
  if ((windDir >= 2.74)&&(windDir < 2.96))
    (winddir = 202.5);//SSW
  if ((windDir >= 2.97)&&(windDir < 3.37))
    (winddir = 225.0);//SW
  if ((windDir >= 3.38)&&(windDir < 3.55))
    (winddir = 247.5);//WSW
  if ((windDir >= 3.56)&&(windDir < 3.85))
    (winddir = 270.0);//W
  if ((windDir >= 4.13)&&(windDir < 4.06))
    (winddir = 292.5);//WNW
  if ((windDir >= 4.07)&&(windDir < 4.32))
    (winddir = 315.0);//NW
  if ((windDir >= 4.33)&&(windDir < 4.93))
    (winddir = 337.5);//NNW
}






















