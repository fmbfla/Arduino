

int ledState = LOW;             // ledState used to set the LED
long previousMillis = 0;        // will store last time LED was updated

// the follow variables is a long because the time, measured in miliseconds,
// will quickly become a bigger number than can be stored in an int.

long interval = 1000;
int adding = 1;

char incoming_char=0;
void setup() {
  // put your setup code here, to run once:
  Serial.begin(19200);
}

void loop() {
  if (interval==0){
    interval = 1000;
  }
  unsigned long currentMillis = millis();
  if(currentMillis - previousMillis > interval) {
    // save the last time you blinked the LED 
    previousMillis = currentMillis;   
    Serial.print("interval ");
    Serial.println(interval);
  }  
  if (Serial.available() >0){
    incoming_char=Serial.read();
    if ((incoming_char=='p')&& (digitalRead(10)==HIGH)){
      Serial.println("ATTENTION!!!..Turning OFF!");
      //power_off();
      return;
    }
    else  if ((incoming_char=='p')&& (digitalRead(10)==LOW)){
      Serial.println("ATTENTION!!!..Turning ON!");
      //power_on();
      return;
    }
    if (incoming_char=='s'){
      //Message();
    }
    if (incoming_char=='+'){
      adding++;
      interval = adding*1000;
      Serial.println("Adding ");
    }
    if (incoming_char=='-'){
      adding--;
      if (interval==1000){
        Serial.println("Cant go lower than 1000! ");
      }
      else
        Serial.println("Subtracting ");
      interval = adding*1000;
      adding--;
    }
  }
}




