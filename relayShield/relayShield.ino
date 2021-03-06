//  RelayShieldDemoCode.pde  to control seeed relay shield by arduino.
//  Copyright (c) 2010 seeed technology inc.
//  Author: Steve Chang
//  Version: september 2, 2010
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License, or (at your option) any later version.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

unsigned char relayPin[4] = {4,5,6,7};

void setup()
{
  int i;
  for(i = 0; i < 4; i++)
  {
    pinMode(relayPin[i],OUTPUT);
  }
}

// just an simple demo to close and open 4 relays
// every other 1s.
void loop()
{

  int i=0;
  for(i = 0; i < 4; i++)
  {
    digitalWrite(relayPin[i],HIGH);
  }
  delay(1000);
  for(i = 0; i < 4; i++)
  {
    digitalWrite(relayPin[i],LOW);
  } 
  delay(1000);
  
}
