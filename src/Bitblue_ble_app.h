/************************************************************************************************************************
 * This program is design for RACE curicullum                                                                  *
 * 1) Encoder wheel, 1 rotation = 20 ticks = 21cm = 210mm                                                                  *
 * 2)                                                 *
 * **********************************************************************************************************************/
/*L298N(Motor Driver) Library for arduino, Created by Amizzuddin, Dec 09 2019*/

#ifndef Bitblue_ble_app_H
#define Bitblue_ble_app_H

#define ADD_SERIAL 0
#define DEBUG 0
#define RX_BUFFER 17
#define HOLD_BUFFER_DURATION 3 //hold buffer for data_type = 0 (unknown)



class Bitblue_ble_app{
 public:
	/************************************************************************************************
     * App			Left	Right	  Up	 Down	Select	 Start	    A      B	   Y	   X	*
     * -------------------------------------------------------------------------------------------- *
     * Arduino		_k[0]	_k[1]	 _k[2]	 _k[3]	 _k[4]	 _k[5]	  _k[6]	  _k[7]	  _k[8]   _k[9]	* 
     ************************************************************************************************/
    Bitblue_ble_app(char Left, char Right, char Up, char Down, char Select, char Start, char A, char B, char Y, char X);
	
    /****************************************
     *   Initialize Serial and confirms     *
     * 	   connection by sending string     *
     ****************************************/
    void initialize(int baud_rate);

	void read_joystix();
    bool isjoystixRead_complete();
    void getjoystix(byte *direction, byte *left_motor_pwm, byte *right_motor_pwm);

    void read_keypad();
    bool iskeypadRead_complete();
    char getkeypad();
    
    bool iskeypad();
    bool isjoystix();
    
    /*To send data to Bitblue APP*/
    void write(byte data1, byte data2);

 private:
    char key[10];
    unsigned long hold_time = 0;
	byte pointer = 0;	//array pointer
    char rx_buffer[RX_BUFFER] = {'\0'};
    byte data_type = 0;	//0=nothing, 1=hold_state, 2=keypad, 3=hex, 4=decimal
    
    void clear_buffer_content();
	void check_data_type();
    bool check_first_buffer_content();
    byte char_to_byte(byte ptr);
    bool isNonzero_content(byte ptr);
	
};
#endif
