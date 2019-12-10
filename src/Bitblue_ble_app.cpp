/*Bitblue ble app Library for arduino, Created by Amizzuddin, Dec 09 2019*/

#include "Arduino.h"
#include "Bitblue_ble_app.h"

#include <AltSoftSerial.h> 			//is included for test library. REMOVE ONCE LIBRARY PASS! and change function associated with it!

AltSoftSerial bltSerial;

Bitblue_ble_app::Bitblue_ble_app(char L, char R, char U, char D, char Se, char St, char A, char B, char Y, char X){
	_k[0]= L;
	_k[1]= R;
	_k[2]= U;
	_k[3]= D;
	_k[4]= Se;
	_k[5]= St;
	_k[6]= A;
	_k[7]= B;
	_k[8]= Y;
	_k[9]= X;
}

void Bitblue_ble_app::init(){
	bltSerial.begin(9600);
	while (!Serial); 						// while the serial stream is not open, do nothing.
	bltSerial.write("BLE Connected!");				// send message on serial coommunication device.
	bltSerial.flush(); 						// ensure all serial string to be finished sending before exiting.
  	
}

void Bitblue_ble_app::read_data(){
	while( (bltSerial.available()) && (_id <= 15) ){
		_buf[_id]=bltSerial.read();
		_id++;
	}
	if(isPrintable(_buf[0])){
		check_data();				// check for data in the _buf
	}
	if( (_mod == 2) || ((_mod >=3) && (_id / _mod >= 4)) ){
		_read_seq = 1;
		clear_data();
		if( _mod >= 3){
			ch_to_byte();
		}
	}
}

void Bitblue_ble_app::check_data(){
	if( _mod == 0 ){
		for(byte x=0; (x < 10) && (_mod == 0); x++){ 
			if( _k[x] == _buf[0]  ){
				_hold_t = millis();
				_mod = 1;
			}
		}
		if( _mod != 1 ){
			clear_data();
		}
	}
	if( _mod == 1 ){
		if( (millis() - _hold_t) >= 3){
			for(byte y=1; (y<=4) && (y<=_id); y++){
			if( !isPrintable(_buf[1]) ){
					_mod = 2;
				}
				else if( (_buf[0] != _k[0]) && !isHexadecimalDigit(_buf[y]) ){
					clear_data();
				}
				else{
					if( (y >= 3) && (_buf[y] == _k[1]) ){
						_mod = y;
					}
				}
			}
		}
	}
	if( _mod >= 3 ){		
		for(byte z=0; (z < (_mod*4)) && (_id == (_mod*4)); z++){
			if( (z % _mod != 0) && !isHexadecimalDigit(_buf[z])  ){
				clear_data();
			}
			else{
				if( z % _mod == 0 ){
					if( ( z / _mod == 0) && (_buf[z] != _k[0]) ){
						clear_data();
					}
					if( ( z / _mod == 1) && (_buf[z] != _k[1]) ){
						clear_data();
					}
					if( ( z / _mod == 2) && (_buf[z] != _k[2]) ){
						clear_data();
					}
					if( ( z / _mod == 3) && (_buf[z] != _k[3]) ){
						clear_data();
					}
				}
			}
		}
	}
}

void Bitblue_ble_app::ch_to_byte(){	
	for(byte i=0, j=0; i<4; i++){		//This portion to convert char to int
		j=(i*_mod)+1;					//to access the numeric values of the stored read data from RX buffer
		for( ;j % _mod != 0; j++){		//convert then store into instruction array
			if(isDigit(_buf[j])){
				if(_mod==4){
					_dis[i] = _dis[i] * 10;					
				}
				if(_mod==3){
					_dis[i] = _dis[i] << 4;
				}
				_dis[i] += (_buf[j] & B1111);
			}
			else{
				_dis[i] = (_dis[i] << 4) + (_buf[j] - 55);
			}				
		}
	}
}

void Bitblue_ble_app::clear_data(){
	if(_read_seq == 0){
		for(byte i=0; i <= _id ; i++){
			_buf[i]='\0';
		}
		_id = 0;
		_mod = 0;
	}
	if(_read_seq == 1){
		for(byte i=0; i < 4 ; i++){
			_dis[i]=0;
		}
		_direction = 0;
	}
}

byte Bitblue_ble_app::remote_control(byte *speed, byte *default_speed, byte *mode){
	while(_read_seq == 0){//code may stuck here until ljoystix info is in.
		read_data();
	}
	if(_read_seq == 1){ //need to add non directional! as result piggy back last state known
		for(int i=0, motor=0; motor<2; motor++){//add events where it is non direction //reset the values stored in the instruction array		
			if(_mod == 2){
				speed[motor] = *default_speed;
			}
			else{
				if( _dis[motor]>0 ){
					i = (_dis[2]|_dis[3]) - (_dis[1]|_dis[0]);
					if(i<0){
						motor[speed] = (_dis[1]|_dis[0]) - (_dis[2]|_dis[3]);
					}
					else{
						motor[speed] = i;
					}
				}
				else{
					i = (_dis[2]|_dis[3]);
					motor[speed]=i;
				}
				if(i > 0){
					i=B10;
				}
				else if(i == 0){
					i=B00;
				}
				else{
					i=B01;
				}
				if(_dis[3] > _dis[2]){				//invert the insturction when joystix is at Z portion (back/reverse)
					i ^= B11;
				}
				_direction |= i;
				if(motor==0){
					_direction = _direction << 2;
					i=0;				
				}
			}
		}
		Serial.write("-");
		if(_mod == 2){
			if(_buf[0] == _k[0]){
			_direction = B0110;
			}
			else if(_buf[0] == _k[1]){
				_direction = B1001;
			}
			else if(_buf[0] == _k[2]){
				_direction = B1010;
			}
			else if(_buf[0] == _k[3]){
				_direction = B0101;
			}
			else if(_buf[0] == _k[4]){
				*mode = 4;
			}
			else if(_buf[0] == _k[5]){
				*mode = 5;
			}
			else if(_buf[0] == _k[6]){
				*mode = 0;
			}
			else if(_buf[0] == _k[7]){
				*mode = 1;
			}
			else if(_buf[0] == _k[8]){
				*mode = 2;
			}
			else{
				if(_buf[0] == _k[9]){
				*mode = 3;
				}
			}
		}
		_read_seq = 0;
		clear_data();
	}
	return _direction;
}

