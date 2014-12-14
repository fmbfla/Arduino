char inchar; // Will hold the incoming character from the GSM shield

int led1 = 10;

void setup()
{
  // set up the digital pins to control
  pinMode(led1, OUTPUT);
  digitalWrite(led1, LOW);

  // wake up the GSM shield
  SIM900power();
  Serial.begin(19200);
  delay(20000);  // give time to log on to network.
  Serial.print("AT+CMGF=1\r");  // set SMS mode to text
  delay(100);
  Serial.print("AT+CNMI=2,2,0,0,0\r");
  // blurt out contents of new SMS upon receipt to the GSM shield's serial out
  delay(100);
}

void SIM900power()
// software equivalent of pressing the GSM shield "power" button
{
  digitalWrite(9, HIGH);
  delay(1000);
  digitalWrite(9, LOW);
  delay(7000);
}

void loop()
{
  //If a character comes in from the cellular module...
  if(Serial.available() >0)
  {
    inchar=Serial.read();
    if (inchar=='#')
    {
      delay(10);

      inchar=Serial.read();
      if (inchar=='a')
      {
        delay(10);
        inchar=Serial.read();
        if (inchar=='0')
        {
          digitalWrite(led1, LOW);
        }
        else if (inchar=='1')
        {
          digitalWrite(led1, HIGH);
        }
        delay(10);
        //  SIM900.println("AT+CMGD=1,4"); // delete all SMS
      }
    }
  }
}

