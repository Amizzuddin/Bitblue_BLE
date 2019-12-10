/*Program is use to interface arduino with bitblue app for both ios and android	*
 *The program needs to disconnect from serial port in order for it to work.		*
 *The pin connection as per below:-*/

// HM10 module         	  TX  		 Rx
// ----------          --------   -------
// Arduino Uno        	  0          1

//You also require the change the following 
  
#include "AltSoftSerial.h"
#include "Bitblue_ble_app.h"

byte dir = 0;								//motor direction
byte motor_speed[2] = {0};
byte default_spd = 50;						//values need to range from 0 to 255
byte mode = 0;

AltSoftSerial altSerial;
Bitblue_ble_app bitblue( 'L' , 'R' , 'U' , 'D' , 'C' , 'S' , 'A' , 'B' , 'Y' , 'X' );  //cannot change U to other letters (in app) as it gives problems
       //keypad setting(left ,right,  up , down,select,start, a  , b   ,  y  ,  x)

void setup() {
  bitblue.init();                         //Initialize Bluetooth communication and prints message via BLE phone app 
  Serial.begin(9600);   			            //Initialize Serial communication
  Serial.write("Serial Connected!\n");		//Message print on Serial monitor
}

void loop() {
  if(altSerial.available()){
    dir = bitblue.remote_control(motor_speed, &default_spd, &mode);
    //Serial.write("\n");
    Serial.print(dir);
    Serial.write(",m1:");
    Serial.print(motor_speed[0]);
    Serial.write(",m2:");
    Serial.print(motor_speed[1]);
    Serial.write(",dir:");
    Serial.print(dir);
    Serial.write(",mode:");
    Serial.println(mode);
  }    
}
