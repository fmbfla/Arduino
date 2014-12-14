

/***************************************************
 * Adafruit CC3000 Breakout/Shield Simple HTTP Server
 * 
 * This is a simple implementation of a bare bones
 * HTTP server that can respond to very simple requests.
 * Note that this server is not meant to handle high
 * load, concurrent connections, SSL, etc.  A 16mhz Arduino 
 * with 2K of memory can only handle so much complexity!  
 * This server example is best for very simple status messages
 * or REST APIs.
 * 
 * See the CC3000 tutorial on Adafruit's learning system
 * for more information on setting up and using the
 * CC3000:
 * http://learn.adafruit.com/adafruit-cc3000-wifi  
 * 
 * Requirements:
 * 
 * This sketch requires the Adafruit CC3000 library.  You can
 * download the library from:
 * https://github.com/adafruit/Adafruit_CC3000_Library
 * 
 * For information on installing libraries in the Arduino IDE
 * see this page:
 * http://arduino.cc/en/Guide/Libraries
 * 
 * Usage:
 * 
 * Update the SSID and, if necessary, the CC3000 hardware pin 
 * information below, then run the sketch and check the 
 * output of the serial port.  After connecting to the 
 * wireless network successfully the sketch will output 
 * the IP address of the server and start listening for 
 * connections.  Once listening for connections, connect
 * to the server IP from a web browser.  For example if your
 * server is listening on IP 192.168.1.130 you would access
 * http://192.168.1.130/ from your web browser.
 * 
 * Created by Tony DiCola and adapted from HTTP server code created by Eric Friedrich.
 * 
 * This code was adapted from Adafruit CC3000 library example 
 * code which has the following license:
 * 
 * Designed specifically to work with the Adafruit WiFi products:
 * ----> https://www.adafruit.com/products/1469
 * 
 * Adafruit invests time and resources providing this open source code, 
 * please support Adafruit and open-source hardware by purchasing 
 * products from Adafruit!
 * 
 * Written by Limor Fried & Kevin Townsend for Adafruit Industries.  
 * BSD license, all text above must be included in any redistribution      
 ****************************************************/
#include <Adafruit_CC3000.h>
#include <SPI.h>
#include "utility/debug.h"
#include "utility/socket.h"
#include <Wire.h>
#include "RTClib.h"
#include <DHT.h>

#define DHTPIN A0     // what pin we're connected to

#define DHTTYPE DHT22   // DHT 22  (AM2302)

DHT dht(DHTPIN, DHTTYPE);
int sensorPin = 0; //the analog pin the TMP36's Vout (sense) pin is connected to
//the resolution is 10 mV / degree centigrade with a

RTC_Millis rtc;
// These are the interrupt and control pins
#define ADAFRUIT_CC3000_IRQ   3  // MUST be an interrupt pin!
// These can be any two pins
#define ADAFRUIT_CC3000_VBAT  5
#define ADAFRUIT_CC3000_CS    10
// Use hardware SPI for the remaining pins
// On an UNO, SCK = 13, MISO = 12, and MOSI = 11

Adafruit_CC3000 cc3000 = Adafruit_CC3000(ADAFRUIT_CC3000_CS, ADAFRUIT_CC3000_IRQ, ADAFRUIT_CC3000_VBAT,
SPI_CLOCK_DIVIDER); // you can change this clock speed

#define WLAN_SSID       "Michie 2"   // cannot be longer than 32 characters!
#define WLAN_PASS       "webf113b"
// Security can be WLAN_SEC_UNSEC, WLAN_SEC_WEP, WLAN_SEC_WPA or WLAN_SEC_WPA2
#define WLAN_SECURITY   WLAN_SEC_WPA2

#define LISTEN_PORT           80      // What TCP port to listen on for connections.  
// The HTTP protocol uses port 80 by default.

#define MAX_ACTION            10      // Maximum length of the HTTP action that can be parsed.

#define MAX_PATH              64      // Maximum length of the HTTP request path that can be parsed.
// There isn't much memory available so keep this short!

#define BUFFER_SIZE           MAX_ACTION + MAX_PATH + 20  // Size of buffer for incoming request data.
// Since only the first line is parsed this
// needs to be as large as the maximum action
// and path plus a little for whitespace and
// HTTP version.

#define TIMEOUT_MS            500    // Amount of time in milliseconds to wait for
// an incoming request to finish.  Don't set this
// too high or your server could be slow to respond.

Adafruit_CC3000_Server httpServer(LISTEN_PORT);
uint8_t buffer[BUFFER_SIZE+1];
int bufindex = 0;
char action[MAX_ACTION+1];
char path[MAX_PATH+1];
int refreshPage = 60; //default is 10sec. 
void setup(void)
{
  Serial.begin(115200);
  rtc.begin(DateTime(F(__DATE__), F(__TIME__)));
  Serial.println(F("Hello, CC3000!\n")); 
  dht.begin();
  Serial.print("Free RAM: "); 
  Serial.println(getFreeRam(), DEC);

  // Initialise the module
  Serial.println(F("\nInitializing..."));
  if (!cc3000.begin())
  {
    Serial.println(F("Couldn't begin()! Check your wiring?"));
    while(1);
  }

  Serial.print(F("\nAttempting to connect to ")); 
  Serial.println(WLAN_SSID);
  if (!cc3000.connectToAP(WLAN_SSID, WLAN_PASS, WLAN_SECURITY)) {
    Serial.println(F("Failed!"));
    while(1);
  }

  Serial.println(F("Connected!"));

  Serial.println(F("Request DHCP"));
  while (!cc3000.checkDHCP())
  {
    delay(100); // ToDo: Insert a DHCP timeout!
  }  

  // Display the IP address DNS, Gateway, etc.
  while (! displayConnectionDetails()) {
    delay(1000);
  }

  // ******************************************************
  // You can safely remove this to save some flash memory!
  // ******************************************************
  Serial.println(F("\r\nNOTE: This sketch may cause problems with other sketches"));
  Serial.println(F("since the .disconnect() function is never called, so the"));
  Serial.println(F("AP may refuse connection requests from the CC3000 until a"));
  Serial.println(F("timeout period passes.  This is normal behaviour since"));
  Serial.println(F("there isn't an obvious moment to disconnect with a server.\r\n"));

  // Start listening for connections
  httpServer.begin();

  Serial.println(F("Listening for connections..."));
}

void loop(void)
{
  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  // Read temperature as Celsius
  float t = dht.readTemperature();
  // Read temperature as Fahrenheit
  float f = dht.readTemperature(true);

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
  float hi = dht.computeHeatIndex(f, h);
  double VaporPressureValue = h * 0.01 * 6.112 * exp((17.62 * t) / (t + 243.12));
  double Numerator = 243.12 * log(VaporPressureValue) - 440.1;
  double Denominator = 19.43 - (log(VaporPressureValue));
  double DewPoint = Numerator / Denominator;

  // Try to get a client which is connected.
  Adafruit_CC3000_ClientRef client = httpServer.available();
  if (client) {
    Serial.println(F("Client connected."));
    // Process this request until it completes or times out.
    // Note that this is explicitly limited to handling one request at a time!

    // Clear the incoming data buffer and point to the beginning of it.
    bufindex = 0;
    memset(&buffer, 0, sizeof(buffer));

    // Clear action and path strings.
    memset(&action, 0, sizeof(action));
    memset(&path,   0, sizeof(path));

    // Set a timeout for reading all the incoming data.
    unsigned long endtime = millis() + TIMEOUT_MS;

    // Read all the incoming data until it can be parsed or the timeout expires.
    bool parsed = false;
    while (!parsed && (millis() < endtime) && (bufindex < BUFFER_SIZE)) {
      if (client.available()) {
        buffer[bufindex++] = client.read();
      }
      parsed = parseRequest(buffer, bufindex, action, path);
    }

    // Handle the request if it was parsed.
    if (parsed) {
      Serial.println(F("Processing request"));
      Serial.print(F("Action: ")); 
      Serial.println(action);
      Serial.print(F("Path: ")); 
      Serial.println(path);
      // Check the action to see if it was a GET request.
      //if (strcmp(action, "GET") == 0) {

      // Respond with the path that was accessed.
      // First send the success response code.
      client.println("HTTP/1.1 200 OK");
      client.println("Content-Type: text/html");
      client.println("Connnection: close");
      client.println();
      client.println("<!DOCTYPE HTML>");
      client.println("<head>");
      // add a meta refresh tag, so the browser pulls again every x seconds:
      client.print("<meta http-equiv=\"refresh\" content=\"");
      client.print(refreshPage);
      client.println("; url=/\">");
      client.fastrprintln(F("HTTP:// Server Sketch"));
      client.fastrprint(F("You accessed path: ")); 
      client.fastrprintln(path);
      //Start to create the html table
      client.println("");
      DateTime now = rtc.now();
      client.fastrprint(F("<CENTER>"));
      client.fastrprint(F("<font color=red size='6'>"));
      client.fastrprint(F("Time: "));
      if(now.hour() < 10){                // Zero padding if value less than 10 ie."09" instead of "9"
        client.fastrprint(F(" "));
        client.print((now.hour()> 12) ? now.hour() - 12 : ((now.hour() == 0) ? 12 : now.hour()), DEC); // Conversion to AM/PM  
      }
      else{
        client.print((now.hour() > 12) ? now.hour() - 12 : ((now.hour() == 0) ? 12 : now.hour()), DEC); // Conversion to AM/PM
      }
      client.fastrprint(F(":"));
      if(now.minute() < 10){         // Zero padding if value less than 10 ie."09" instead of "9"
        client.fastrprint(F("0"));
        client.print(now.minute(), DEC);
      }
      else{
        client.print(now.minute(), DEC);
      }
      client.fastrprint(F(":"));
      if(now.second() < 10){         // Zero padding if value less than 10 ie."09" instead of "9"
        client.fastrprint(F("0"));
        client.print(now.second(), DEC);
      }
      else{
        //client.println("<font color=red size='6'>");
        client.print(now.second(), DEC);
        //client.println("</font>");
      }
      if(now.hour() < 12){                  // Adding the AM/PM sufffix
        //client.println("<font color=red size='6'> AM</font>");
      }
      else{
        //client.println("<font color=red size='6'> PM</font>");
      }
      client.fastrprint(F("</font>"));
      client.fastrprint(F("</CENTER>"));

      int reading = analogRead(sensorPin);  
      // converting that reading to voltage, for 3.3v arduino use 3.3
      float voltage = reading * 5.0;
      voltage /= 1024.0; 
      // print out the voltage
      //Serial.print(voltage); Serial.println(" volts");
      float temperatureC = (voltage - 0.5) * 100 ;  //converting from 10 mv per degree wit 500 mV offset
      float temperatureF = (temperatureC * 9.0 / 5.0) + 32.0;
      //Serial.print(temperatureF); Serial.println(" degrees F");
      //Printing the Temperature depending on temp
      if (temperatureF>77.00){
        client.fastrprint(F("<tr>\n"));        
        client.fastrprint(F("<td><h4>"));
        client.fastrprintln(F("<font color=red size='6'>Temperature </font>"));
        client.fastrprint(F("  "));
        client.fastrprint(F("<font size='5'>"));
        client.print(f,1);
        client.fastrprint(F(" &deg;F</h3></td>\n"));
        client.fastrprint(F("</font>"));
        client.fastrprint(F("<td></td>"));
        client.fastrprint(F("</tr>"));
        client.fastrprint(F("</tr>\n"));  
      }  
      else {
        client.fastrprint(F("<tr>\n"));        
        client.fastrprint(F("<td><h4>"));
        client.fastrprintln(F("<font color=green size='6'>Temperature</font>"));
        client.fastrprint(F(" "));
        client.fastrprint(F("<font size='5'>"));
        client.print(f,1);
        client.fastrprint(F(" &deg;F</h3></td>\n"));
        client.fastrprint(F("</font>"));
        client.fastrprint(F("<td></td>"));
        client.fastrprint(F("</tr>"));
        client.fastrprint(F("</tr>\n")); 
      }
      client.fastrprint(F("<tr>\n"));        
      client.fastrprint(F("<td><h4>"));
      client.fastrprint(F("<font color=maroon size='6'>"));
      client.print("Humidity: "); 
      client.fastrprint(F("<font size='5'>"));
      client.print(h);
      client.println(" %\t");
      client.fastrprint(F("<tr>\n"));        
      client.fastrprint(F("<td><h4>"));
      client.fastrprint(F("<font color=blue size='6'>"));
      client.fastrprint(F("Heat index: "));
      client.fastrprint(F("<font size='5'>"));
      client.print(hi);
      client.fastrprintln(F(" *F"));
      client.fastrprint(F("<tr>\n"));        
      client.fastrprint(F("<td><h4>"));
      client.fastrprint(F("<font color=purple size='6'>"));
      client.fastrprint(F("Dew point "));
      client.fastrprint(F("<font size='5'>"));
      client.print(((9.0 / 5.0) * DewPoint) + 32);
      client.fastrprint(F(" *F\n"));

      Serial.print("Humidity: "); 
      Serial.print(h);
      Serial.print(" %\t");
      Serial.print("Temperature: "); 
      Serial.print(t);
      Serial.print(" *C ");
      Serial.print(f);
      Serial.print(" *F\t");
      Serial.print("Heat index: ");
      Serial.print(hi);
      Serial.println(" *F");
      Serial.print("Dewpoint ");


      /*
        //Printing the Humidity
       client.print("<td><h4>");
       client.print("Humidity");
       client.print("</h4></td>\n");
       client.print("<td></td>");             
       client.print("<td>");
       client.print("<h3>");
       client.print("95");
       client.print(" %</h3></td>\n");
       client.print("<td></td>");
       client.print("</tr>");
       client.print("<tr>\n");        
       //Printing the Barometer
       client.print("<td><h4>");
       client.print("Barometer");
       client.print("</h4></td>\n");
       client.print("<td></td>");             
       client.print("<td>");
       client.print("<h3>");
       client.print("30.02");
       client.print(" inHg</h3></td>\n");
       client.print("<td></td>");
       client.print("</tr>");
       // client.println("</table>");
       //client.println("</FORM>");
       */
      // }
      //  else {
      // Unsupported action, respond with an HTTP 405 method not allowed error.
      //    client.fastrprintln(F("HTTP/1.1 405 Method Not Allowed"));
      //    client.fastrprintln(F(""));
      //  }
    }

    // Wait a short period to make sure the response had time to send before
    // the connection is closed (the CC3000 sends data asyncronously).
    delay(100);

    // Close the connection when done.
    Serial.println(F("Client disconnected"));
    client.close();
  }
}

// Return true if the buffer contains an HTTP request.  Also returns the request
// path and action strings if the request was parsed.  This does not attempt to
// parse any HTTP headers because there really isn't enough memory to process
// them all.
// HTTP request looks like:
//  [method] [path] [version] \r\n
//  Header_key_1: Header_value_1 \r\n
//  ...
//  Header_key_n: Header_value_n \r\n
//  \r\n
bool parseRequest(uint8_t* buf, int bufSize, char* action, char* path) {
  // Check if the request ends with \r\n to signal end of first line.
  if (bufSize < 2)
    return false;
  if (buf[bufSize-2] == '\r' && buf[bufSize-1] == '\n') {
    parseFirstLine((char*)buf, action, path);
    return true;
  }
  return false;
}

// Parse the action and path from the first line of an HTTP request.
void parseFirstLine(char* line, char* action, char* path) {
  // Parse first word up to whitespace as action.
  char* lineaction = strtok(line, " ");
  if (lineaction != NULL)
    strncpy(action, lineaction, MAX_ACTION);
  // Parse second word up to whitespace as path.
  char* linepath = strtok(NULL, " ");
  if (linepath != NULL)
    strncpy(path, linepath, MAX_PATH);
}

// Tries to read the IP address and other connection details
bool displayConnectionDetails(void)
{
  uint32_t ipAddress, netmask, gateway, dhcpserv, dnsserv;

  if(!cc3000.getIPAddress(&ipAddress, &netmask, &gateway, &dhcpserv, &dnsserv))
  {
    Serial.println(F("Unable to retrieve the IP Address!\r\n"));
    return false;
  }
  else
  {
    Serial.print(F("\nIP Addr: ")); 
    cc3000.printIPdotsRev(ipAddress);
    Serial.print(F("\nNetmask: ")); 
    cc3000.printIPdotsRev(netmask);
    Serial.print(F("\nGateway: ")); 
    cc3000.printIPdotsRev(gateway);
    Serial.print(F("\nDHCPsrv: ")); 
    cc3000.printIPdotsRev(dhcpserv);
    Serial.print(F("\nDNSserv: ")); 
    cc3000.printIPdotsRev(dnsserv);
    Serial.println();
    return true;
  }
}








