/*
 * transmitter_project.c
 *
 * Created: 3/5/2019 1:57:33 PM
 * Author : dimitri
 */ 

#include <avr/io.h>
#include <util/delay.h>
//#include "uart.h"	 //User defined UART library which contains the UART routines
#include <avr/eeprom.h>
#include "string.h"
#include "io.c"
#include "timer.h"



unsigned char GetBit( unsigned char x, unsigned char k) {
	return ((x & (0x01 << k)) != 0);
}

typedef struct task {
	signed char state; //Task's current state
	unsigned long int period; //Task period
	unsigned long int elapsedTime; //Time elapsed since last task tick
	int (*TickFct)( int ); //Task tick function
} task;


unsigned char GetKeypadKey() {
	PORTC = 0xEF; // Enable col 4 with 0, disable others with 1’s
	asm ( "nop" ); // add a delay to allow PORTC to stabilize before checking
	if (GetBit(PINC,0)==0) { return ('1'); }
	if (GetBit(PINC,1)==0) { return ('4'); }
	if (GetBit(PINC,2)==0) { return ('7'); }
	if (GetBit(PINC,3)==0) { return ('*'); }
	// Check keys in col 2
	PORTC = 0xDF; // Enable col 5 with 0, disable others with 1’s
	asm ( "nop" ); // add a delay to allow PORTC to stabilize before checking
	if (GetBit(PINC,0)==0) { return ('2'); }
	if (GetBit(PINC,1)==0) { return ('5'); }
	if (GetBit(PINC,2)==0) { return ('8'); }
	if (GetBit(PINC,3)==0) { return ('0'); }
	
	// Check keys in col 3
	PORTC = 0xBF; // Enable col 6 with 0, disable others with 1’s
	asm ( "nop" ); // add a delay to allow PORTC to stabilize before checking
	if (GetBit(PINC,0)==0) { return ('3'); }
	if (GetBit(PINC,1)==0) { return ('6'); }
	if (GetBit(PINC,2)==0) { return ('9'); }
	if (GetBit(PINC,3)==0) { return ('#'); }
	
	// Check keys in col 4
	PORTC = 0x7F; // Enable col 6 with 0, disable others with 1’s
	asm ( "nop" ); // add a delay to allow PORTC to stabilize before checking
	if (GetBit(PINC,0)==0) { return ('A'); }
	if (GetBit(PINC,1)==0) { return ('B'); }
	if (GetBit(PINC,2)==0) { return ('C'); }
	if (GetBit(PINC,3)==0) { return ('D'); }
	
	return ('\0'); // default value
}

unsigned char cursorPositon;

void DisplayInput(unsigned char userInput){
	switch (userInput) {
		//case '\0': PORTB = 0x1F; break ; // All 5 LEDs on
		case '1':
			LCD_Cursor(cursorPositon + 1);
			LCD_WriteData('1');
			break ;
		case '2':
			LCD_Cursor(cursorPositon + 1);
			LCD_WriteData('2');
			break;
		case '3':
			LCD_Cursor(cursorPositon + 1);
			LCD_WriteData('3');
			break;
		case '4':
			LCD_Cursor(cursorPositon + 1);
			LCD_WriteData('4');
			break;
		case '5':
			LCD_Cursor(cursorPositon + 1);
			LCD_WriteData('5');
			break;
		case '6':
			LCD_Cursor(cursorPositon + 1);
			LCD_WriteData('6');
			break;
		case '7':
			LCD_Cursor(cursorPositon + 1);
			LCD_WriteData('7');
			break;
		case '8':
			LCD_Cursor(cursorPositon + 1);
			LCD_WriteData('8');
			break;
		case '9':
			LCD_Cursor(cursorPositon + 1);
			LCD_WriteData('9');
			break;
		case 'A':
			LCD_Cursor(cursorPositon + 1);
			LCD_WriteData('A');
			break;
		case 'B':
			LCD_Cursor(cursorPositon + 1);
			LCD_WriteData('B');
			break;
		case 'C':
			LCD_Cursor(cursorPositon + 1);
			LCD_WriteData('C');
			break;
		case 'D':
			LCD_Cursor(cursorPositon + 1);
			LCD_WriteData('D');
			break;
		case '*':
			LCD_Cursor(cursorPositon + 1);
			LCD_WriteData('*');
			break;
		case '0':
			LCD_Cursor(cursorPositon + 1);
			LCD_WriteData('0');
			break;
		case '#':
			LCD_Cursor(cursorPositon + 1);
			LCD_WriteData('#');
			break;
		default :
			break ; // Should never occur. Middle LED
	}
	return;
}


void DisplayMenu() {
	LCD_Cursor(1);
	LCD_DisplayString(1, "1. Start Game   2. Fastest times");
	return;
}

void CallReceiver() {
	PORTA = PORTA | 0x04; 
	while(!((PINA & 0x10) == 0x10)) {} 
	PORTA = PORTA & 0xFB;
	//PORTA = PORTA & 0xFB; 
	LCD_ClearScreen();
	
	return;
}

unsigned char waitTick = 0;
unsigned char timerTick = 0; //use it 
unsigned char matrixSequence1[] = {'3', 'A', '6', 'B'};
unsigned char matrixSequence2[] = {'8', '5', '4', '3', 'A', 'B'};
unsigned char matrixSequence3[] = {'6', '2', '6', '1', '3', '1'};
unsigned char sequenceSize[] = {4, 6, 6};
unsigned char userSequence[6];
unsigned char userInput;
unsigned char matrixStage; //keeps track of what the display is tracking
unsigned char sequenceIndex;

unsigned char checkSequence1(){
	for(unsigned char i = 0; i < sequenceSize[0]; ++i) {
		if(userSequence[i] != matrixSequence1[i]) {
			return 0;
		}
	}
	return 1;
}

unsigned char checkSequence2(){
	for(unsigned char i = 0; i < sequenceSize[1]; ++i) {
		if(userSequence[i] != matrixSequence2[i]) {
			return 0;
		}
	}
	return 1;
}

unsigned char checkSequence3(){
	for(unsigned char i = 0; i < sequenceSize[2]; ++i) {
		if(userSequence[i] != matrixSequence3[i]) {
			return 0;
		}
	}
	return 1;
}

enum Game_States {Intro, Wait, Start, ShowScore, KeypadInput, CheckInput} Game_State;
int Game_Tick(int Game_state){
	unsigned char read_string = 0;
	
	switch (Game_state){
		case Intro : 
			DisplayMenu();
			Game_state = Wait;
			break;
		case Wait : 
			if((~PINB & 0x01) == 0x01) {
				Game_state = Start;
			}
			else if((~PINB & 0x08) == 0x08) {
				Game_state = ShowScore;
			}
			break;
		case Start : //display matrix
			CallReceiver();
			Game_state = KeypadInput;
			break;
		case ShowScore :
			Game_state = Wait;
			break;
		case KeypadInput : 
			if(sequenceIndex < sequenceSize[matrixStage]) 
			{
				Game_state = KeypadInput;
			}
			else 
			{
				sequenceIndex = 0;
				Game_state = CheckInput;
			}
			break;
		case CheckInput :
			if(matrixStage < 3) 
			{
				Game_state = Start;
			}
			else
			{
				matrixStage = 0;
				Game_state = Intro; 
			}
			break;
		default: 
			Game_state = Wait;
			break;
	}//end transitions
	
	switch (Game_state)
	{
		case Intro :
			break;
		case Wait :
			break;
		case Start : 
			LCD_Cursor(1);
			LCD_DisplayString(1, "Waiting for receiver");
			break;
		case ShowScore : //reading eeprom here
			LCD_ClearScreen();
			LCD_Cursor(1);
			LCD_DisplayString(1, "Showing score");
			break;
		case KeypadInput :
			userInput = GetKeypadKey();
			
			if(userInput != '\0'){
				userSequence[sequenceIndex] = userInput;
				++sequenceIndex;
				DisplayInput(userInput);
			}
			_delay_ms(2000);
			
			break;
		case CheckInput :
			if(matrixStage == 0)
			{
				if(checkSequence1()) {
					LCD_Cursor(1);
					LCD_DisplayString(1, "Correct!");
				}
				else {
					LCD_Cursor(1);
					LCD_DisplayString(1, "Wrong! the sequence is 3A6B");
				}
				++matrixStage;
			}
			else if(matrixStage == 1)
			{
				if(checkSequence2()) {
					LCD_Cursor(1);
					LCD_DisplayString(1, "Correct!");
				}
				else {
					LCD_Cursor(1);
					LCD_DisplayString(1, "Wrong! the sequence is 8543AB");
				}
				++matrixStage;
			}
			else if(matrixStage == 2)
			{
				if(checkSequence3()) {
					LCD_Cursor(1);
					LCD_DisplayString(1, "Correct!");
				}
				else {
					LCD_Cursor(1);
					LCD_DisplayString(1, "Wrong! the sequence is 626131");
				}
				++matrixStage; //after last index
			}
			_delay_ms(50000);
			break;
		default:
			break;
	}
	
	return Game_state;
}

void EEPROM_write(unsigned int uiAddress, unsigned char ucData)
{
	/* Wait for completion of previous write */
	while(EECR & (1<<EEPE))
	;
	/* Set up address and Data Registers */
	EEAR = uiAddress;
	EEDR = ucData;
	/* Write logical one to EEMPE */
	EECR |= (1<<EEMPE);
	/* Start eeprom write by setting EEPE */
	EECR |= (1<<EEPE);
}

unsigned char EEPROM_read(unsigned int uiAddress)
{
	/* Wait for completion of previous write */
	while(EECR & (1<<EEPE))
	;
	/* Set up address register */
	EEAR = uiAddress;
	/* Start eeprom read by writing EERE */
	EECR |= (1<<EERE);
	/* Return data from Data Register */
	return EEDR;
}

//static task Tasks[1];

int main(void)
{
    DDRA = 0x0F; PORTA = 0xF0;  //for lcd
	DDRB = 0x00; PORTB = 0xFF;
    DDRC = 0xF0; PORTC = 0x0F; // PC7..4 outputs init 0s, PC3..0 inputs init 1s
    DDRD = 0xFF; PORTD = 0x00; //for lcd
	
	unsigned char ByteOfData = 0;
	char buffer[8];
	
    LCD_init();
	
	//ByteOfData = 0x16;
	//eeprom_write_byte (( uint8_t *) 46, ByteOfData );                        // write a byte of data to eeprom
	//_delay_ms(10);

	ByteOfData = eeprom_read_byte(( uint8_t *) 46);						 // read a byte of data from eeprom
	itoa(ByteOfData,buffer,10);
    
	//LCD_ClearScreen();
	LCD_Cursor(1);
	LCD_DisplayString(1, buffer);
	
	/*
    unsigned long GamePeriod = 100;
    unsigned long period = 100;
    

    unsigned char i = 0;
    
    Tasks[i].state = Intro;
    Tasks[i].period = GamePeriod;
    Tasks[i].elapsedTime = GamePeriod;
    Tasks[i].TickFct = &Game_Tick;
    
    TimerSet(period);
    TimerOn();
    
    
    while (1) {
	    for ( i = 0; i < 1; i++ ) {
		    // Task is ready to tick
		    if ( Tasks[i].elapsedTime == Tasks[i].period ) {
			    // Setting next state for task
			    Tasks[i].state = Tasks[i].TickFct(Tasks[i].state);
			    // Reset the elapsed time for next tick.
			    Tasks[i].elapsedTime = 0;
		    }
		    Tasks[i].elapsedTime += period;
	    }
	    while (!TimerFlag);
	    TimerFlag = 0;
	    
    }
	*/
}

/*
enum CheckKeypad_States {Wait_KP, GetKey, Display} State;
unsigned char x;

int KeypadTick(int state){
	switch (state){
		case Wait_KP : state = GetKey;
			break;
		case GetKey : state = Display;
			break;
		case Display : state = GetKey;
			break;
		default: State = GetKey;
			break;
	}//end transitions
	
	switch (state)
	{
		case Wait_KP :
			break;
		case GetKey : x = GetKeypadKey();
			break;
		case Display :
		
		switch (x) {
			//case '\0': PORTB = 0x1F; break ; // All 5 LEDs on
			case '1':
				LCD_Cursor(1);
				LCD_WriteData('1');
				break ;
			case '2':
				LCD_Cursor(1);
				LCD_WriteData('2');
				break;
			case '3':
			LCD_Cursor(1);
			LCD_WriteData('3');
			break;
			case '4':
			LCD_Cursor(1);
			LCD_WriteData('4');
			break;
			case '5':
			LCD_Cursor(1);
			LCD_WriteData('5');
			break;
			case '6':
			LCD_Cursor(1);
			LCD_WriteData('6');
			break;
			case '7':
			LCD_Cursor(1);
			LCD_WriteData('7');
			break;
			case '8':
			LCD_Cursor(1);
			LCD_WriteData('8');
			break;
			case '9':
			LCD_Cursor(1);
			LCD_WriteData('9');
			break;
			case 'A':
			LCD_Cursor(1);
			LCD_WriteData('A');
			break;
			case 'B':
			LCD_Cursor(1);
			LCD_WriteData('B');
			break;
			case 'C':
			LCD_Cursor(1);
			LCD_WriteData('C');
			break;
			case 'D':
			LCD_Cursor(1);
			LCD_WriteData('D');
			break;
			case '*':
			LCD_Cursor(1);
			LCD_WriteData('*');
			break;
			case '0':
			LCD_Cursor(1);
			LCD_WriteData('0');
			break;
			case '#':
			LCD_Cursor(1);
			LCD_WriteData('#');
			break;
			default :
				break ; // Should never occur. Middle LED
		}
		break;
		default:
			break;
	}
	
	return state;
}
*/

