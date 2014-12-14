

#include <Adafruit_VC0706.h>
#include <SD.h>

// comment out this line if using Arduino V23 or earlier
#include <SoftwareSerial.h>         
#define chipSelect 10
// On Uno: camera TX connected to pin 2, camera RX to pin 3:
SoftwareSerial cameraconnection = SoftwareSerial(2, 3);
Adafruit_VC0706 cam = Adafruit_VC0706(&cameraconnection);
int inputPin = 4;               // choose the input pin (for PIR sensor)
int pirState = HIGH; 
int val = 0;
void setup() {

  pinMode(inputPin, INPUT);     // declare sensor as input
  Serial.begin(9600);
  Serial.println("VC0706 Camera snapshot test");
  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    return;
  }  

  // Try to locate the camera
  if (cam.begin()) {
    Serial.println("Camera Found:");
  } 
  else {
    Serial.println("No camera found?");
    return;
  }
  // Print out the camera version information (optional)
  char *reply = cam.getVersion();
  if (reply == 0) {
    Serial.print("Failed to get version");
  } 
  else {
    Serial.println("-----------------");
    Serial.print(reply);
    Serial.println("-----------------");
  }

  cam.setImageSize(VC0706_640x480);        // biggest
  //cam.setImageSize(VC0706_320x240);        // medium
  //cam.setImageSize(VC0706_160x120);          // small

  // You can read the size back from the camera (optional, but maybe useful?)
  uint8_t imgsize = cam.getImageSize();
  Serial.print("Image size: ");
  if (imgsize == VC0706_640x480) Serial.println("640x480");
  if (imgsize == VC0706_320x240) Serial.println("320x240");
  if (imgsize == VC0706_160x120) Serial.println("160x120");

}

void loop() {
  Serial.println();
  
  Serial.println("Ready to Snap");
  val = digitalRead(inputPin);  // read input value
  delay(500);
   if (val == HIGH) {            // check if the input is HIGH
    if (pirState == LOW) {
      // we have just turned on
      Serial.println("Motion detected!");

      cam.takePicture(); 
      Serial.println("Picture taken!");

      char filename[13];
      strcpy(filename, "IMAGE00.JPG");
      for (int i = 0; i < 100; i++) {
        filename[5] = '0' + i/10;
        filename[6] = '0' + i%10;
        // create if does not exist, do not open existing, write, sync after write
        if (! SD.exists(filename)) {
          break;
        }
      }

      File imgFile = SD.open(filename, FILE_WRITE);

      uint16_t jpglen = cam.frameLength();
      Serial.print(jpglen, DEC);
      Serial.println(" byte image");

      Serial.print("Writing image to "); 
      Serial.print(filename);
      byte wCount = 0; // For counting # of writes
      while (jpglen > 0) {
        // read 32 bytes at a time;
        uint8_t *buffer;
        uint8_t bytesToRead = min(32, jpglen); // change 32 to 64 for a speedup but may not work with all setups!
        buffer = cam.readPicture(bytesToRead);
        imgFile.write(buffer, bytesToRead);

        //Serial.print("Read ");  Serial.print(bytesToRead, DEC); Serial.println(" bytes");
        if(++wCount >= 64) { // Every 2K, give a little feedback so it doesn't appear locked up
          Serial.print('.');
          wCount = 0;
        }
        jpglen -= bytesToRead;
      }
      imgFile.close();
      Serial.println("...Done!");

      cam.resumeVideo();
      //cam.setMotionDetect(true);
      pirState = HIGH;

    }
  }
  else {

    if (pirState == HIGH){
      // we have just turned of
      Serial.println("Motion ended!");
      // We only want to print on the output change, not state
      pirState = LOW;
    }
  }
}












