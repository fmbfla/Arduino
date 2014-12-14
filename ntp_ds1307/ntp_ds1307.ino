// Date and time functions using a DS1307 RTC connected via I2C and Wire lib
#include "SPI.h"
#include "Ethernet.h"
#include "EthernetUdp.h"	      // New from IDE 1.0
#include <Wire.h>
#include "RTClib.h"

// byte timeServer[] = {192, 43, 244, 18}; // time.nist.gov NTP server
byte timeServer0[] = {
  132, 163, 4, 101};    // ntp1.nl.net NTP server  
byte timeServer1[] = {
  132, 163, 4, 102}; // time-b.timefreq.bldrdoc.gov NTP server
byte timeServer2[] = {
  132, 163, 4, 103};
const int NTP_PACKET_SIZE= 48;     // NTP time stamp is in the first 48 bytes of the message
byte pb[NTP_PACKET_SIZE];          // buffer to hold incoming and outgoing packets 

byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };//Hard config for ethernet card mac addy
EthernetClient client;
unsigned int localPort = 8888;
RTC_DS1307 rtc;
EthernetUDP Udp; 
void setup () {
  Serial.begin(57600);
#ifdef AVR
  Wire.begin();
#else
  Wire1.begin(); // Shield I2C pins connect to alt I2C bus on Arduino Due
#endif
  rtc.begin();

  if (! rtc.isrunning()) {
    Serial.println("RTC is NOT running!");
    // following line sets the RTC to the date & time this sketch was compiled
    //rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
  }
   if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet");
    Serial.println("Check the Ethernet shield, CAT5 Cable and or the WIFI Repeter");
    //Ethernet.begin(mac, ip);
    // Serial.println(ip);
    while (1);
  }
  Serial.println("Ethernet shield Ready");
  Serial.print("Local I.P: ");
  Serial.println(Ethernet.localIP());
  Serial.println("Attempting to set local time VIA the NTP. ");

  ntp();//Set the RTC on first boot, it will fail the first time but Complets the second time

}

void loop () {
    DateTime now = rtc.now();
    
    Serial.print(now.year(), DEC);
    Serial.print('/');
    Serial.print(now.month(), DEC);
    Serial.print('/');
    Serial.print(now.day(), DEC);
    Serial.print(' ');
    Serial.print(now.hour(), DEC);
    Serial.print(':');
    Serial.print(now.minute(), DEC);
    Serial.print(':');
    Serial.print(now.second(), DEC);
    Serial.println();
  
    
    Serial.println();
    delay(3000);
}
void ntp(){

  Udp.begin(localPort);// Start the UDP on the selected port
  // Serial.println();
  delay(100);
  int packetSize = Udp.parsePacket(); //<<<<<<<<<<<<NEW LINE HERE 
  // send an NTP packet to a time server
  sendNTPpacket(timeServer1);
  // wait to see if a reply is available
  delay(100);
  if ( Udp.available() ) {
    // read the packet into the buffer
    Udp.read(pb, packetSize);      // New from IDE 1.0 <<<<<<<<<<<<<MODIFIED LINE

    unsigned long t1, t2, t3, t4;
    t1 = t2 = t3 = t4 = 0;
    for (int i=0; i< 4; i++)
    {
      t1 = t1 << 8 | pb[16+i];      
      t2 = t2 << 8 | pb[24+i];      
      t3 = t3 << 8 | pb[32+i];      
      t4 = t4 << 8 | pb[40+i];
    }

    float f1,f2,f3,f4;
    f1 = ((long)pb[20] * 256 + pb[21]) / 65536.0;      
    f2 = ((long)pb[28] * 256 + pb[29]) / 65536.0;      
    f3 = ((long)pb[36] * 256 + pb[37]) / 65536.0;      
    f4 = ((long)pb[44] * 256 + pb[45]) / 65536.0;

    const unsigned long seventyYears = 2208988800UL;
    t1 -= seventyYears;
    t2 -= seventyYears;
    t3 -= seventyYears;
    t4 -= seventyYears;

    PrintDateTime(DateTime(t4)); 
    // Serial.println(f4,4);
    Serial.println();
    // Adjust timezone and DST... in my case substract 4 hours for Florida Time
    // or work in UTC?

    //t4 -= (4 * 3600L);     // Notice the L for long calculations!!
    t4 -= (2/3600L);     // Divide by 2 for DST
    t4 += 1;               // adjust the delay(1000) at begin of loop!
    if (f4 > 0.4) t4++;    // adjust fractional part, see above
    Serial.println("UpDating CLOCK ");
    //Serial.print("RTC before: ");
    PrintDateTime(rtc.now());// Show me the time
    rtc.adjust(DateTime(t4));
    Serial.println("Updated Time : ");
    PrintDateTime(rtc.now());
    Serial.println();
    Serial.println("done ...");
  }
  else
  {
    //Serial.println("ARRRRRRRRRRRRrrr !!!");// wait 10 more loop's.
    Serial.println("No UDP available ...");
    delay(1000);//wait a second or more before trying again 
    ntp(); //Try to get the time from the NTP SERVER
  }

}
void PrintDateTime(DateTime t)
{
  char datestr[24];
  sprintf(datestr, "%04d-%02d-%02d  %02d:%02d:%02d  ", t.year(), t.month(), t.day(), t.hour(), t.minute(), t.second());
  Serial.println(datestr);  
}


// send an NTP request to the time server at the given address 
unsigned long sendNTPpacket(byte *address)
{
  // set all bytes in the buffer to 0
  memset(pb, 0, NTP_PACKET_SIZE); 
  // Initialize values needed to form NTP request
  // (see URL above for details on the packets)
  pb[0] = 0b11100011;   // LI, Version, Mode
  pb[1] = 0;     // Stratum, or type of clock
  pb[2] = 6;     // Polling Interval
  pb[3] = 0xEC;  // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  pb[12]  = 49; 
  pb[13]  = 0x4E;
  pb[14]  = 49;
  pb[15]  = 52;

  // all NTP fields have been given values, now
  // you can send a packet requesting a timestamp: 
#if ARDUINO >= 100
  // IDE 1.0 compatible:
  Udp.beginPacket(address, 123); //NTP requests are to port 123
  Udp.write(pb,NTP_PACKET_SIZE);
  Udp.endPacket(); 
#else
  Udp.sendPacket( pb,NTP_PACKET_SIZE,  address, 123); //NTP requests are to port 123
#endif	  

}

