#include <DHT.h>


#define DHTPIN A0             // Analog 0 pin we're connected to
#define DHTTYPE DHT22
# define hall 3               // Digital 3 Receive the pulse from sensor on pin 2/Interupt 0
# define led 6 

int period = 2500;           // Measurement period (miliseconds)
// Variable definitions
unsigned int Sample = 0;       // Sample number
unsigned int rainCount = 0;      // B/W counter for sensor 
int buttonState;             // the current reading from the input pin
int lastButtonState = HIGH;   // the previous reading from the input pin
// Below is for bebouncing the rainBucket's hall effect switch. 
// I'm tring this out because from time to time, I get false hits due to something unknown at this time.
// The following variables are long's because the time, measured in miliseconds,
// will quickly become a bigger number than can be stored in an int.
long lastDebounceTime = 0;  // the last time the output pin was toggled
long debounceDelay = 10;    // the debounce time; increase if the output flickers
DHT dht(DHTPIN, DHTTYPE);
//--Begin of setup
void setup()
{
  dht.begin();
  Serial.begin(19200);
  pinMode(hall, INPUT);
  digitalWrite(hall, HIGH);
  pinMode(led, OUTPUT);
  digitalWrite(led, HIGH);
  Serial.println("Rain Gauge");

}//--End of setup

void loop()
{
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  float f = dht.readTemperature(true);

  Rain();
  Sample++;
  Serial.print("Sample: ");
  Serial.println(Sample);
  Serial.print("Seconds: ");
  if (isnan(t) || isnan(h)) {
    Serial.println("Failed to read from DHT");
  } else {
    Serial.print("Humidity: "); 
    Serial.print(h);
    Serial.print(" %\t");
    Serial.print("Temperature: "); 
    Serial.print(f);
    Serial.println(" *F");
  }
  Serial.print("Inches of Rain: ");
  Serial.println();
  Serial.print(rainCount / 2 / 100.0);
  Serial.println();
 // digitalWrite(12, HIGH);
 delay(1000);
// digitalWrite(12, LOW);
}//-------End of loop

void Rain(){


  int reading = digitalRead(hall);

  // check to see if you just pressed the button 
  // (i.e. the input went from LOW to HIGH),  and you've waited 
  // long enough since the last press to ignore any noise:  

  // If the switch changed, due to noise or pressing:
  if (reading != lastButtonState) {
    // reset the debouncing timer
    lastDebounceTime = millis();
  } 
  
  if ((millis() - lastDebounceTime) > debounceDelay) {
    // whatever the reading is at, it's been there for longer
    // than the debounce delay, so take it as the actual current state:

    // if the button state has changed:
    if (reading != buttonState) {
       attachInterrupt(1,addrain,CHANGE);
      }
  }

  lastButtonState = reading;
   digitalWrite(led, LOW);
}



// save the tips and add them up
void addrain(){
  rainCount++;
 digitalWrite(led, HIGH);
}



