#include <SoftwareSerial.h>

SoftwareSerial mySerial(3,2);//defaut 
void setup () {
  Serial.begin(115200);
  mySerial.begin(115200);
  Serial.println("reciever ready");
}
void loop() {
  // read from port 1, send to port 0:
  if (mySerial.available()) {
    int inByte = mySerial.read();
    Serial.write(inByte); 
  }
  
  // read from port 0, send to port 1:
  if (Serial.available()) {
    int inByte = Serial.read();
    mySerial.write(inByte); 
  }
}


