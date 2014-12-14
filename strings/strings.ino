/*  Induino R3 User Guide - Program 17.0 - Working with Strings - Basic String Operations
The program receives a String Serially, Trims it, Extracts a substring and prints both the lower and upper case versions of it
It also prints the length of the string before and after trimming
use the following string input as text (key it in from the Serial monitor)
"   This is a Test String   " // there are 3 leading and 3 trailing spaces
 */


String input=""; // Declare an Empty String Variable
String input_trimmed="";// a variable to store the trimmed string
String substring=""; // a variable to store the substring 
int length_before_trim = 0, length_after_trim = 0, length_substring=0; // 3 variables for storing the string lengths

void setup()
{
  Serial.begin(19200); // Initialise Serial Communication
}

void loop()
{
  if(Serial.available()) // Check if there is incoming data in the Serial Buffer
  {
    input = ""; // reset the input variable
    while(Serial.available()) // Process incoming data till the buffer becomes empty
    {     
      input += (char)Serial.read();// Read a byte of data, convert it to character and append it to the string variable
      delay(1);
    }
    Serial.println();
    input_trimmed = input; // assign the trimmed variable with the input value
    input_trimmed.trim();
    substring = input_trimmed.substring(12,14);
    length_before_trim = input.length();
    length_after_trim = input_trimmed.length();
    length_substring = substring.length();
    Serial.println("Your Original Input : " +input);
    Serial.print("Length of Your Original Input : ");
    Serial.println(length_before_trim);
    Serial.println("The Trimmed Input : " +input_trimmed);
    Serial.print("Length of Trimmed Input : ");
    Serial.println(length_after_trim);
    Serial.println("Substring : " +substring);
    Serial.print("Length of Substring : ");
    Serial.println(length_substring);
  }
}
