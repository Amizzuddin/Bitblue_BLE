/*Bitblue ble app Library for arduino, Created by Amizzuddin, Dec 09 2019*/

#include <Arduino.h>
#include "Bitblue_ble_app.h"

#if ADD_SERIAL
#include <SoftwareSerial.h>
SoftwareSerial altSerial(2,3);
#endif

Bitblue_ble_app::Bitblue_ble_app(char Left, char Right, char Up, char Down, char Select, char Start, char A, char B, char Y, char X){
	key[0] = Left;
	key[1] = Right;
	key[2] = Up;
	key[3] = Down;
	key[4] = Select;
	key[5] = Start;
	key[6] = A;
	key[7] = B;
	key[8] = Y;
	key[9] = X;
}

void Bitblue_ble_app::initialize(int baud_rate){
	Serial.begin(baud_rate);
	while(!Serial);

	#ifdef SoftwareSerial_h
	altSerial.begin(baud_rate);
	altSerial.write("BLE OK!");
	#else
	Serial.write("BLE OK!");
	#endif
}

void Bitblue_ble_app::write(byte data1, byte data2){
	#if ADD_SERIAL
	altSerial.print(data1);
	altSerial.write(",");
	altSerial.println(data2);
	#else
	Serial.print(data1);
	Serial.write(",");
	Serial.println(data2);
	#endif
}

void Bitblue_ble_app::read_keypad(){
	#if ADD_SERIAL
	while( altSerial.available() && !iskeypadRead_complete() ){
		*(rx_buffer + pointer) = altSerial.read();
		pointer++;
		if( data_type > 2 ){
			clear_buffer_content();
		}
	}
	#else //need to change the statements
	while( Serial.available() && !iskeypadRead_complete() ){
		*(rx_buffer + pointer) = Serial.read();
		pointer++;
		if(data_type > 2 ){
			clear_buffer_content();
		}
	}
	#endif
}

bool Bitblue_ble_app::iskeypadRead_complete(){ //Acknowledge if data is fullset and can also use as buffer has been read or not
	if(data_type <= 1){ //incomplete data in the buffer
		check_data_type(); //check contents in the rx_buffer
	}

	if(data_type == 2){ //data is fullset in the rx_buffer but not been extracted
		return true;
	}
	else{//no data/incomplete data in the buffer
		return false;
	}
}

void Bitblue_ble_app::read_joystix(){ //read the data in serial and store in the rx_buffer
	#if ADD_SERIAL
	while( altSerial.available() && !isjoystixRead_complete() ){
		*(rx_buffer + pointer) = altSerial.read();
		pointer++;
		if( pointer > RX_BUFFER ){
			clear_buffer_content();
		}
	}
	#else //need to change the statements
	while( Serial.available() && !isjoystixRead_complete() ){
		*(rx_buffer + pointer) = Serial.read();
		pointer++;
		if(pointer > RX_BUFFER ){
			clear_buffer_content();
		}
	}
	#endif
}

bool Bitblue_ble_app::isjoystixRead_complete(){ //Acknowledge if data is fullset and can also use as buffer has been read or not
	if(data_type <= 1){ //incomplete data in the buffer
		check_data_type(); //check contents in the rx_buffer
	}

	if(data_type > 2){ //returns according to data received in the rx_buffer -for joystix data
		return ((pointer % data_type) == 0) && ((pointer / data_type) == 4);
	}
	else{//no data/incomplete data in the buffer
		return false;
	}
}

/*Start private function*/
void Bitblue_ble_app::check_data_type(){
	if( data_type == 0 ){
		if(pointer > 0){ //proceed only if there is data in buffer
			if( check_first_buffer_content() ){
				data_type = 1;
				hold_time = millis();
			}
		}
	}
	else{
		if( data_type == 1 ){
			if( millis() - hold_time >= HOLD_BUFFER_DURATION ){
				if( pointer == 1 ){
					data_type = 2;
				}
				else{
					if( rx_buffer[0] == key[0] ){ //to ensure correct first marker
						if( pointer > 3 ){
							data_type = ( rx_buffer[3] == key[1] ) ? 3 : 4;
						}
					}
					else{
						clear_buffer_content();
					}
				}
			}
		}
	}	
}

bool Bitblue_ble_app::check_first_buffer_content(){ //check if first data received in the keymapping list
	if(rx_buffer[0] == key[0]){
		return true;
	}
	else if (rx_buffer[0] == key[1]){
		return true;
	}
	else if (rx_buffer[0] == key[2]){
		return true;
	}
	else if (rx_buffer[0] == key[3]){
		return true;
	}
	else if (rx_buffer[0] == key[4]){
		return true;
	}
	else if (rx_buffer[0] == key[5]){
		return true;
	}
	else if (rx_buffer[0] == key[6]){
		return true;
	}
	else if (rx_buffer[0] == key[7]){
		return true;
	}
	else if (rx_buffer[0] == key[8]){
		return true;
	}
	else if (rx_buffer[0] == key[9]){
		return true;
	}
	else{
		if( rx_buffer[0] != '\0'){ //if its not part of mapping, it should be NULL char
			clear_buffer_content();
		}
		return false;
	}
}

void Bitblue_ble_app::clear_buffer_content(){
	#if DEBUG
	Serial.write("CLR:");
	Serial.write(rx_buffer, RX_BUFFER);
	Serial.write(",");
	Serial.print(pointer);
	Serial.write(",");
	Serial.print(data_type);
	Serial.write(",");
	Serial.write("\n");
	#endif
	// Serial.write("CLR:");
	// Serial.write(rx_buffer, RX_BUFFER);
	// Serial.write(",");
	// Serial.print(pointer);
	// Serial.write(",");
	// Serial.print(data_type);
	// Serial.write(",");
	// Serial.println();

	memset(rx_buffer,'\0', pointer);
	pointer = 0;
	data_type = 0;
	hold_time = 0;
}
/*end private function*/

bool Bitblue_ble_app::iskeypad(){
	if(data_type == 2){
		return true;
	}
	else{
		return false;
	}
}

bool Bitblue_ble_app::isjoystix(){
	if(data_type > 2){
		return true;
	}
	else{
		return false;
	}
}

char Bitblue_ble_app::getkeypad(){
	char keypad_value = rx_buffer[0];
	clear_buffer_content();
	return keypad_value;
}

void Bitblue_ble_app::getjoystix(byte *direction, byte *left_motor_pwm, byte *right_motor_pwm){
    for(byte heading=0, buffer_ptr, turn, zero_contents=0; heading < 4; heading++){
        buffer_ptr = (heading * data_type) + 1;
        if(isNonzero_content(buffer_ptr)){
			byte temp = char_to_byte(buffer_ptr);
			if(heading < 2){
				turn = heading;
				if( (255 - temp) <= 0){
					*right_motor_pwm = *left_motor_pwm = temp;
				}
				else{
					*right_motor_pwm = *left_motor_pwm = 255 - temp;
				}
				
			}
			else{
				if(turn == 0){
					if(temp > *right_motor_pwm){
						*right_motor_pwm = temp;
					}
					else{
						*left_motor_pwm = temp;
					}
				}
				else{
					if(temp > *left_motor_pwm){
						*left_motor_pwm = temp;
					}
					else{
						*right_motor_pwm = temp;
					}
				}
				*direction = (heading == 2) ? 0b1010: 0b0101;
			}
        }
		else{
			zero_contents += 1;
			if(zero_contents == 3){
				*direction = *left_motor_pwm = *right_motor_pwm = 0;
			}
		}
    }

	clear_buffer_content();
}

byte Bitblue_ble_app::char_to_byte(byte ptr){
    byte converted=0;
    for( ; (ptr % data_type) != 0; ptr++){
        if( isDigit(rx_buffer[ptr]) ){//convert numeric
            switch(data_type){
                case 4: converted*=10;break;
                case 3: converted<<=4;break;
            }
            converted += (rx_buffer[ptr] & 0b1111);
        }
        else{
            converted <<= 4;
            converted += (rx_buffer[ptr]-55);
        }
    }
	#if DEBUG
	Serial.print(converted);
	Serial.write(",");
	#endif
    return converted;
}

bool Bitblue_ble_app::isNonzero_content(byte ptr){
	for( ; (ptr % data_type) != 0; ptr++){
		if(rx_buffer[ptr] != '0'){
			return true;
		}
	}
	return false;
}

