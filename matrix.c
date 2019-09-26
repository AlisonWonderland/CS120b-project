/*
 * matrix_practice.c
 *
 *0x80 turns on the rightmost column on the LED matrix
 *
 */ 


#include <avr/io.h>

//header to enable data flow control over pins

#define F_CPU 1000000 //900 is the only one that works so far

//telling controller crystal frequency attached

#include <util/delay.h>

//header to enable delay function in program

#include "timer.h"

// character representation array should be made global for cleaner code

//better name would be displaySequence1()
// test1 arrays should be renamed to characters
void displayMatrix1()
{
	unsigned char test1[14][8]= {
		{0b00000000,0b00111100,0b01100110,0b01100000,0b00111000,0b01100000,0b01100110,0b00111100}, //3 fin
		{0b00000000,0b00111100,0b01100110,0b01100110,0b01111110,0b01100110,0b01100110,0b01100110}, //A
		{0b00000000,0b00111100,0b01100110,0b00000110,0b00111110,0b01100110,0b01100110,0b00111100}, //6
		{0b00000000,0b00111110,0b01100110,0b01100110,0b00111110,0b01100110,0b01100110,0b00111110}, //B
	};
	unsigned char rows[] = {1,2,4,8,16,32,64,128};
	unsigned char letter = 0;
	unsigned long wait = 1;
	
	while(letter < 4)
	{
		for(int m = 0; m < 8; ++m) 
		{
				 for (int n=0;n<1;n++)

				 {
					 for (int j=0;j<4;j++)
					 {
						 PORTD = ~rows[m]; //rows
						 PORTB = test1[letter][m]; 
						 _delay_us(500);
					 }
			 
					  for (int k=0;k<4;k++)
					  {
						  PORTD = ~rows[m];
						  PORTB = test1[letter][m];//show other half of character(COLUMN)
						  _delay_us(500);
					  }
				 }
		}
		
		
		if(wait % 500 == 0) {	
			if(letter < 4) {
				++letter;
			}
		}
		++wait;
	}
	
	PORTB = 0x00;
	for(unsigned char x = 0; x < 10; ++x){ //pulse it for other microcontroller to read.
		PORTA = 0x02;
	}
	for(unsigned char x = 0; x < 10; ++x){
		PORTA = 0x00;
	}
	return;
}

void displayMatrix2()
{
	unsigned char test1[14][8]= {
		{0b00000000,0b00111100,0b01100110,0b01100110,0b00111100,0b01100110,0b01100110,0b00111100}, //8
		{0b00000000,0b01111110,0b00000110,0b00111110,0b01100000,0b01100000,0b01100110,0b00111100}, //5
		{0b00000000,0b00110000,0b00111000,0b00110100,0b00110010,0b01111110,0b00110000,0b00110000}, //4
		{0b00000000,0b00111100,0b01100110,0b01100000,0b00111000,0b01100000,0b01100110,0b00111100}, //3 
		{0b00000000,0b00111100,0b01100110,0b01100110,0b01111110,0b01100110,0b01100110,0b01100110}, //A
		{0b00000000,0b00111110,0b01100110,0b01100110,0b00111110,0b01100110,0b01100110,0b00111110}, //B
	};
	unsigned char rows[] = {1,2,4,8,16,32,64,128};
	unsigned char letter = 0; 
	unsigned long wait = 1;
	
	while(letter < 6)
	{
		for(int m = 0; m < 8; ++m)
		{
			for (int n=0;n<1;n++)

			{
				for (int j=0;j<4;j++)
				{
					PORTD = ~rows[m]; //rows
					PORTB = test1[letter][m];
					_delay_us(500);
				}

				for (int k=0;k<4;k++)
				{
					PORTD = ~rows[m];
					PORTB = test1[letter][m];//show other half of character(COLUMN)
					_delay_us(500);
				}
			}
		}
		
		
		if(wait % 500 == 0) {
			if(letter < 6 ) {
				++letter;
			}
		}
		++wait;
	}
	
	PORTB = 0x00;
	for(unsigned char x = 0; x < 10; ++x){
		PORTA = 0x02;
	}
	for(unsigned char x = 0; x < 10; ++x){
		PORTA = 0x00;
	}
	return;
}

void displayMatrix3()
{
	unsigned char test1[14][8]= {
		{0b00000000,0b00111100,0b01100110,0b00000110,0b00111110,0b01100110,0b01100110,0b00111100}, //6
		{0b00000000,0b00111100,0b01100110,0b01100000,0b00110000,0b00001100,0b00000110,0b01111110}, //2
		{0b00000000,0b00111100,0b01100110,0b00000110,0b00111110,0b01100110,0b01100110,0b00111100}, //6
		{0b00000000,0b00011000,0b00011000,0b00011100,0b00011000,0b00011000,0b00011000,0b01111110}, //1
		{0b00000000,0b00111100,0b01100110,0b01100000,0b00111000,0b01100000,0b01100110,0b00111100}, //3
		{0b00000000,0b00011000,0b00011000,0b00011100,0b00011000,0b00011000,0b00011000,0b01111110}, //1
	};
	unsigned char rows[] = {1,2,4,8,16,32,64,128};
	unsigned char letter = 0; 
	unsigned long wait = 1;
	
	while(letter < 6)
	{
		for(int m = 0; m < 8; ++m)
		{
			for (int n=0;n<1;n++)

			{
				for (int j=0;j<4;j++)
				{
					PORTD = ~rows[m];
					PORTB = test1[letter][m];
					_delay_us(500);
				}
				
				for (int k=0;k<4;k++)
				{
					PORTD = ~rows[m];
					PORTB = test1[letter][m];
					_delay_us(500);
				}
			}
		}
		
		
		if(wait % 500 == 0) {
			if(letter < 6 ) {
				++letter;
			}
		}
		++wait;
	}
	
	PORTB = 0x00;
	for(unsigned char x = 0; x < 10; ++x){
		PORTA = 0x02;
	}
	for(unsigned char x = 0; x < 10; ++x){
		PORTA = 0x00;
	}
	return;
}

enum States{Wait, Display1, Display2, Display3} state;
unsigned char stage;
	
void displayTick()
{
	switch (state){
		case Wait :
			if(((PINA & 0x01) == 0x01) && stage == 0) {
				state = Display1;
			}
			else if(((PINA & 0x01) == 0x01) && stage == 1) {
				state = Display2;
			}
			else if(((PINA & 0x01) == 0x01) && stage == 2) {
				state = Display3;
			}
			break;
		case Display1 :
			state = Wait; 
			++stage;
			break;
		case Display2 :
			state = Wait;
			++stage;
			break;
		case Display3 :
			state = Wait;
			stage = 0; //going back to initial sequence
			break;
		default:
			state = Wait;
			break;
	}
	
	switch (state){
		case Wait :
			break;
		case Display1 :
			displayMatrix1();
			break;
		case Display2 :
			displayMatrix2();
			break;
		case Display3 :
			displayMatrix3();
			break;
		default:
			state = Wait;
			break;
	}
	return;
}

int main(void)
{
	DDRA = 0xFE; PORTA = 0x00; 
	
    DDRB = 0xFF; PORTB = 0x00;
	DDRD = 0xFF; PORTD = 0x00;
	
	while(1)
	{
		displayTick();
	}
}

/*
Keeping this here for future use.
Array that contains the rows of bits that are needed to represented each letter/number. This bits needed to be reversed.
unsigned char test1[14][8]= {
	{0b00000000,0b00111100,0b01100110,0b01100110,0b01111110,0b01100110,0b01100110,0b01100110}, //A
	{0b00000000,0b00111110,0b01100110,0b01100110,0b00111110,0b01100110,0b01100110,0b00111110}, //B
	{0b00000000,0b00111100,0b01100110,0b00000110,0b00000110,0b00000110,0b01100110,0b00111100}, //C
	{0b00000000,0b00111110,0b01100110,0b01100110,0b01100110,0b01100110,0b01100110,0b00111110}, //D
	{0b00000000,0b00111100,0b01100110,0b01110110,0b01101110,0b01100110,0b01100110,0b00111100}, //0
	{0b00000000,0b00011000,0b00011000,0b00011100,0b00011000,0b00011000,0b00011000,0b01111110}, //1
	{0b00000000,0b00111100,0b01100110,0b01100000,0b00110000,0b00001100,0b00000110,0b01111110}, //2
	{0b00000000,0b00111100,0b01100110,0b01100000,0b00111000,0b01100000,0b01100110,0b00111100}, //3 
	{0b00000000,0b00110000,0b00111000,0b00110100,0b00110010,0b01111110,0b00110000,0b00110000}, //4
	{0b00000000,0b01111110,0b00000110,0b00111110,0b01100000,0b01100000,0b01100110,0b00111100}, //5
	{0b00000000,0b00111100,0b01100110,0b00000110,0b00111110,0b01100110,0b01100110,0b00111100}, //6
	{0b00000000,0b01111110,0b01100110,0b00110000,0b00110000,0b00011000,0b00011000,0b00011000}, //7
	{0b00000000,0b00111100,0b01100110,0b01100110,0b00111100,0b01100110,0b01100110,0b00111100}, //8
	{0b00000000,0b00111100,0b01100110,0b01100110,0b01111100,0b01100000,0b01100110,0b00111100} //9
};

*/
