/*This .ino is a test for develop the library to interface with Bitblue app from ios. */

#include <AltSoftSerial.h>
#include <Bitblue_ble_app.h>

AltSoftSerial altSerial;
Bitblue_ble_app Bitblue;

char data = ' ';
int dvalue = 0;

void setup() {
  altSerial.begin(9600);
  altSerial.println("Connected!");
  Serial.begin(9600);
  Serial.println("Connected!");
  
}

void loop() {
  data = Bitblue.read_data();

  if( data == 'F' || data == 'R' || data == 'L' || data == 'B' ){
    data = Bitblue.direction_value();
  }

  Serial.print(data);
  Serial.print(": ");
  Serial.print(dvalue);
  Serial.println("\n");
  
}
