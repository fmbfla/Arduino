// Example 55.5

#include <SoftwareSerial.h> 
char inchar; // Will hold the incoming character from the GSM shield
SoftwareSerial SIM900(7, 8);

int numring=0;
int comring=3; 
int onoff=0; // 0 = off, 1 = on

void setup()
{
  Serial.begin(19200);
  // set up the digital pins to control
  pinMode(12, OUTPUT);
  pinMode(13, OUTPUT); // LEDs - off = red, on = green
  digitalWrite(12, HIGH);
  digitalWrite(13, LOW);

  // wake up the GSM shield
  SIM900power(); 
  SIM900.begin(19200);
  SIM900.print("AT+CLIP=1\r"); // turn on caller ID notification
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

void doSomething()
{
  if (onoff==0)
  {
    onoff=1;
    digitalWrite(12, HIGH);
    digitalWrite(13, LOW);
    Serial.println("D12 high D13 low");
  } 
  else
    if (onoff==1)
    {
      onoff=0;
      digitalWrite(12, LOW);
      digitalWrite(13, HIGH);
      Serial.println("D12 low D13 high");
    }
}

void loop() 
{
  if(SIM900.available() >0)
  {
    inchar=SIM900.read(); 
    if (inchar=='R')
    {
      delay(10);
      inchar=SIM900.read(); 
      if (inchar=='I')
      {
        delay(10);
        inchar=SIM900.read();
        if (inchar=='N')
        {
          delay(10);
          inchar=SIM900.read(); 
          if (inchar=='G')
          {
            delay(10);
            // So the phone (our GSM shield) has 'rung' once, i.e. if it were a real phone
            // it would have sounded 'ring-ring' or 'blurrrrr' or whatever one cycle of your ring tone is
            numring++;
            Serial.println("ring!");
            if (numring==comring)
            {
              numring=0; // reset ring counter
              doSomething();
            }
          }
        }
      }
    }
  }
}
