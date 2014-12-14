/*
*The board for the wetness sensor converts an Analog signal into a Digital
 * ON or OFF Signal that can be Adjusted with the potentiometer thats built into the board.
 * this works out great for being able to turn something on or off depending on the amount
 * of moisture being sensed.
 * Simple to adjust on the bench or on target to find the right percentage to make the switching happen.
 ******CONNECTIONS*********
 * Connect the vcc to 5.0 and the Gnd to gnd on MCU
 * Connect + and - on the ADC board to the Sensor.
 * Connect the A0 pin on ADC board to any Analog pin (this sketch is using A0)
 * Connect the DIG pin to any Digital pin not in use (this sketch is using D7)
 */
// set pin numbers:

const int leaf =  A0;
const int OnF = 7;
const int led =  11; 

void setup() {
  // initialize serial communication at 9600 bits per second:
  Serial.begin(115200);
  pinMode(led,OUTPUT);
  digitalWrite(led,LOW);
}


void loop() {
  int dig = digitalRead(OnF);
  int leafwetness = analogRead(leaf);  
  Serial.print("RAW ");
  Serial.print("\t");
  Serial.println(analogRead(leaf));
  float voltage = (analogRead(A0)) * (5.0 / 1023.0);
  Serial.print("voltage: ");
  Serial.println(voltage);
  leafwetness = map(leafwetness, 0, 1023, 0, 15);
  leafwetness = 15 - leafwetness;
  Serial.print("Leaf wetness: "); 
  Serial.print("\t"); 
  Serial.print(leafwetness);
  Serial.println(" %");
  dig = 1 - dig;
  //if it's a 1 (LOW) print high, or turn something on
  if (dig==0){
    Serial.println("OFF");
    digitalWrite(led,LOW);
  }
  else{
    //if it's a 0 (HIGH) print high, or turn something OFF
    Serial.println("ON");
    digitalWrite(led,HIGH);
  }
  Serial.println();
  delay(1000);


}





