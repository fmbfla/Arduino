/* Example 1.0 
 
 
 This is a test to use trimmed strings to grab the incomming phone number
 and to Reply to the number Calling or Texting IN!
 
 
 */


#include <SoftwareSerial.h>
SoftwareSerial SIM900(7, 8);

String input=""; // Declare an Empty String Variable
String input_trimmed="";// a variable to store the trimmed string
String substring=""; // a variable to store the substring 
int length_before_trim = 0, length_after_trim = 0, length_substring=0; // 3 variables for storing the string lengths
//char incoming_char = 0;
//char inchar = 0;
void setup()
{
  Serial.begin(19200); // for serial monitor
  Serial.println("This is a test to use trimmed strings to grab the incomming phone number");
  Serial.println("and to Reply to the number Calling or Texting IN!");
  SIM900.begin(19200); // for GSM shield
  SIM900power();  // turn on shield
  delay(5000);  // give time to log on to network.
  SIM900.print("AT+CMGF=1\r");  // set SMS mode to text
  delay(100);
  SIM900.print("AT+CNMI=2,2,0,0,0\r"); 
  // blurt out contents of new SMS upon receipt to the GSM shield's serial out

  /*  // Now we simply display any text that the GSM shield sends out on the serial monitor
   if(SIM900.available() >0)
   {
   incoming_char=SIM900.read(); //Get the character from the cellular serial port.
   Serial.print(incoming_char); //Print the incoming character to the terminal.
   }*/
  delay(100);
}

void SIM900power()
// software equivalent of pressing the GSM shield "power" button
{
  digitalWrite(9, HIGH);
  delay(1000);
  digitalWrite(9, LOW);
  delay(7000);
  Serial.println("Ready to receive text and to Reply to the number Texting IN!");
}

void loop()
{


  if(SIM900.available()) // Check if there is incoming data in the Serial Buffer
  {
    input = ""; // reset the input variable
    while(SIM900.available()) // Process incoming data till the buffer becomes empty
    {     
      input += (char)SIM900.read();// Read a byte of data, convert it to character and append it to the string variable
      delay(1);
    }
    Serial.println();
    input_trimmed = input; // assign the trimmed variable with the input value
    input_trimmed.trim();
    substring = input_trimmed.substring(8,19);
    length_before_trim = input.length();
    length_after_trim = input_trimmed.length();
    length_substring = substring.length();
    /*Serial.println("Your Original Input : " +input);
     Serial.print("Length of Your Original Input : ");
     Serial.println(length_before_trim);
     Serial.println("The Trimmed Input : " +input_trimmed);
     Serial.print("Length of Trimmed Input : ");
     Serial.println(length_after_trim);*/
    Serial.println("Substring : " +substring);
    /*Serial.print("Length of Substring : ");
     Serial.println(length_substring);*/

    if (substring=="19412959244"){
      Serial.println("message recieved");
      digitalWrite(13,HIGH);
      sendSMS();
    }

  }
  delay(1000);
} 


void sendSMS()
{
  Serial.println(substring);
  Serial.println("sending reply");
  SIM900.print("AT+CMGF=1\r"); 
  Serial.print("AT+CMGF=1\r");  // AT command to send SMS message
  delay(100);
  SIM900.print("AT + CMGS = \"+");
  delay(100);
  Serial.print("AT + CMGS = \"+"); 
  SIM900.print(substring);
  delay(100);
  Serial.print(substring);
  SIM900.println("\"");  
  Serial.println("\"");    // recipient's mobile number, in international format
  delay(100);
  SIM900.println("Hello, world. This is a text message from an Arduino Uno."); 
  Serial.println("Hello, world. This is a text message from an Arduino Uno."); 
  // message to send
  delay(100);
  SIM900.println((char)26);                       // End AT command with a ^Z, ASCII code 26
  delay(100); 
  SIM900.println();
  delay(5000);                                     // give module time to send SMS
  Serial.println("message sent");
}





