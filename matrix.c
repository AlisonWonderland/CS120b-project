/*
 * matrix_practice.c
 *
 *0x80 turns on the rightmost column
 *
 */ 

/*
#include <avr/io.h>
//#include <util/delay.h>

int main() 
{
	//button
	DDRC = 0xFF; PORTC = 0x00;
	DDRB = 0x00; PORTB = 0xFF;
	
	while(1)
	{
		if((~PINB & 0x01) == 0x01) 
		{
			PORTC = 0xFF;
		}
		else
		{
			PORTC = 0x00;
		}
	}
	
	return 0;
}
*/

#define F_CPU 16000000    // AVR SYS CLOCK FREQUENCY
#define BAUD 9600         // BAUD RATE

#include <avr/io.h>
#include <util/delay.h>

//FUNCTION DECLARATIONS:

#define FOSC 1843200 // Clock Speed
#define BAUD 9600
#define MYUBRR FOSC/16/BAUD-1

void USART_Init( unsigned int ubrr)
{
	//Set baud rate
	UBRR0H = (unsigned char)(ubrr>>8);
	UBRR0L = (unsigned char)ubrr;
	//Enable receiver and transmitter
	UCSR0B = (1<<RXEN0)|(1<<TXEN0);
	//Set frame format: 8data, 2stop bit
	UCSR0C = (1<<USBS0)|(3<<UCSZ00);
}

void USART_Transmit( unsigned char data )
{
	// Wait for empty transmit buffer
	while ( !( UCSR0A & (1<<UDRE0)) )
	;
	// Put data into buffer, sends the data
	UDR0 = data;
}

unsigned char USART_Receive( void )
{
	/* Wait for data to be received */
	while ( !(UCSR0A & (1<<RXC0)) )
	;
	/* Get and return received data from buffer */
	return UDR0;
}

int main( void )
{
	//...
	USART_Init(MYUBRR);
	USART_Transmit(0x01);
	USART_Receive();
	//...
	return 0;
}



/*
#include <avr/io.h>

//header to enable data flow control over pins

#define F_CPU 1000000 //900 is the only one that works so far

//telling controller crystal frequency attached

#include <util/delay.h>

//header to enable delay function in program

#include "timer.h"


int main(void)
{
    DDRB = 0xFF; PORTB = 0x00;
	DDRD = 0xFF; PORTD = 0x00;
	unsigned char test1[14][8]= {
		{0b00000000,0b00111100,0b01100110,0b01100110,0b01111110,0b01100110,0b01100110,0b01100110}, //A
		{0b00000000,0b00111110,0b01100110,0b01100110,0b00111110,0b01100110,0b01100110,0b00111110}, //B
		{0b00000000,0b00111100,0b01100110,0b00000110,0b00000110,0b00000110,0b01100110,0b00111100}, //C
		{0b00000000,0b00111110,0b01100110,0b01100110,0b01100110,0b01100110,0b01100110,0b00111110}, //D
		{0b00000000,0b00111100,0b01100110,0b01110110,0b01101110,0b01100110,0b01100110,0b00111100}, //0
		{0b00000000,0b00011000,0b00011000,0b00011100,0b00011000,0b00011000,0b00011000,0b01111110}, //1 
		{0b00000000,0b00111100,0b01100110,0b01100000,0b00110000,0b00001100,0b00000110,0b01111110}, //2
		{0b00000000,0b00111100,0b01100110,0b01100000,0b00111000,0b01100000,0b01100110,0b00111100}, //3 fin
		{0b00000000,0b00110000,0b00111000,0b00110100,0b00110010,0b01111110,0b00110000,0b00110000}, //4
		{0b00000000,0b01111110,0b00000110,0b00111110,0b01100000,0b01100000,0b01100110,0b00111100}, //5
		{0b00000000,0b00111100,0b01100110,0b00000110,0b00111110,0b01100110,0b01100110,0b00111100}, //6
		{0b00000000,0b01111110,0b01100110,0b00110000,0b00110000,0b00011000,0b00011000,0b00011000}, //7
		{0b00000000,0b00111100,0b01100110,0b01100110,0b00111100,0b01100110,0b01100110,0b00111100}, //8
		{0b00000000,0b00111100,0b01100110,0b01100110,0b01111100,0b01100000,0b01100110,0b00111100} //9
		};
	unsigned char rows[] = {1,2,4,8,16,32,64,128};
	unsigned char letter = 13; //change back to zero
	unsigned long wait = 1;
	
    while (1) 
    {
		//for(int l = 0; l < 2; ++l) { //could use wait here to make it cleaner
			for(int m = 0; m < 8; ++m) //uncomment this and see if m works with rows. next figure columns out.
			{
				 for (int n=0;n<1;n++)//execute 200 times for the eye to catch

				 {
					 for (int j=0;j<4;j++)
					 {
						 PORTD = ~rows[m]; //rows
						 PORTB = test1[letter][m]; //change binary values and add ~. THen don't need to reverse bin values
						 _delay_us(500);
					 }
			 
					//PORTD = 0x00;
			 
					  for (int k=0;k<4;k++)
					  {
						  PORTD = ~rows[m];// ROW
						  PORTB = test1[letter][m];//show other half of character(COLUMN)
						  _delay_us(500);
					  }
			  
					 //PORTD = 0x00;
				 }
			}
		//}
		
		
		if(wait % 500 == 0) {
			//PORTB = 0x00;
			/*
			if(letter < 4) {
				++letter;
			}
			
			if(letter > 0) {
				--letter;
			}
			//for(int i = 0; i < 5000; ++i) {}
		}
		
		if(wait < 100000)
		{
			++wait;
		}
		
    }
}
*/
/*
//sending a signal to ~PINA & 0x01 works so micro controller might work
//timer.h affects led matrix. could be used still just don't use it with the display characters list state.
//shut screen down wait
//program that takes bit strings and reverses them
//_delay_ms(220); doesnt work
*/
