/*L298N(Motor Driver) Library for arduino, Created by Amizzuddin, Dec 09 2019*/

#ifndef Bitblue_ble_app_H
#define Bitblue_ble_app_H

#include "Arduino.h"

class Bitblue_ble_app{
 public:
	/************************************************************************************************
     * App			Left	Right	  Up	 Down	Select	 Start	    A      B	   Y	   X	*
     * -------------------------------------------------------------------------------------------- *
     * Arduino		_k[0]	_k[1]	 _k[2]	 _k[3]	 _k[4]	 _k[5]	  _k[6]	  _k[7]	  _k[8]   _k[9]	* 
     ************************************************************************************************/
    Bitblue_ble_app(char L, char R, char U, char D, char Se, char St, char A, char B, char Y, char X);
	
    /****************************************
     *   Initialize Serial and confirms     *
     * 	   connection by sending string     *
     ****************************************/
    void init();
	
	/********************************************************
     * Functions will return direction instruction for	 	*
     * motor driver from bitblue controller app.			*
     ********************************************************/
    byte remote_control(byte *speed, byte *default_speed, byte *mode);

	//add mode selection

 private:
	byte _read_seq = 0;
	char _k[10];				//app keypad setting
	char _buf[17]={'\0'};
	byte _dis[4]= {0};			//0=L,1=R,2=U,3=Z cartesian position
	byte _id=0;					//_buf array pointer
	byte _mod=0;				//1=unknown,2=single,3=Hexidecimal,4=decimal data
	byte _direction = 0;
	unsigned long _hold_t;		//use to determine if its single or DEC/HEX data
	void read_data();
	void check_data();
	void ch_to_byte();
	void clear_data();
};

#endif
