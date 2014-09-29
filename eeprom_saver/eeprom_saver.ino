#include <EEPROM.h>
#include "EEPROMAnything.h"

// EEPROM saver
// Saves the "constants" that are specific to an etherhouse device
// into the eeprom of the arduino.
// This separation of config/code to allow the source code to be identical
// between two clients, at the expense of a bit of complexity and code space

// start reading from the first byte (address 0) of the EEPROM
int address = 0;
byte value;

const int MY_ID=0;
const char MY_ID_CHAR[]="0";
const char MY_API_KEY[]="testkey0";
const byte my_mac[] = { 0x74,0x69,0x69,0x2D,0x30,MY_ID };

// EEPROM layout

/*
00000: MY_ID
00010: MY_ID_CHAR
00020: MY_API_KEY

*/

void setup() {
  
  clear_all_eeprom();
  
  EEPROM_writeAnything(0x00, MY_ID);
  EEPROM_writeAnything(0x10, MY_ID_CHAR[0]);
  EEPROM_writeAnything(0x20, MY_API_KEY);
 
 // initialize serial and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }
 
}


void loop() {

  // read a byte from the current address of the EEPROM
  value = EEPROM.read(address);
  
  Serial.print(address);
  Serial.print("\t");
  Serial.print(value, DEC);
  Serial.print("\t");
  Serial.print(value, HEX);
  Serial.print("\t");
  Serial.print(value, BIN);
  Serial.print("\t");
  Serial.write(value);
  Serial.println();
  
  // advance to the next address of the EEPROM
  address = address + 1;
  
  // there are only 512 bytes of EEPROM, from 0 to 511, so if we're
  // on address 512, wrap around to address 0
  if (address == 512)
    address = 0;
    
  delay(500);

}

void clear_all_eeprom(){
  // write a 0 to all 512 bytes of the EEPROM
  for (int i = 0; i < 512; i++)
    EEPROM.write(i, -1);
}




