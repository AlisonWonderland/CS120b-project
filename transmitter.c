/*
 * transmitter_project.c
 *
 * Created: 3/5/2019 1:57:33 PM
 * Author : dimitri
 */ 

#include <avr/io.h>
#include <util/delay.h>
#include <avr/eeprom.h>
#include "string.h"
#include "io.c"
#include "timer.h"

int timerTick = 0; //use it
char seconds[] = " seconds"; //used in the ShowTime case
unsigned char matrixSequence1[] = {'3', 'A', '6', 'B'};
unsigned char matrixSequence2[] = {'8', '5', '4', '3', 'A', 'B'};
unsigned char matrixSequence3[] = {'6', '2', '6', '1', '3', '1'};
unsigned char sequenceSize[] = {4, 6, 6};
unsigned char userSequence[6];
unsigned int recordArray[3];
unsigned char userInput;
unsigned char matrixStage; //keeps track of what the display is tracking
unsigned char sequenceIndex;
unsigned char correctInput;

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


void DisplayInput(unsigned char userInput){
	switch (userInput) {
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
	return;
}


void DisplayMenu() {
	LCD_Cursor(1);
	LCD_DisplayString(1, "1. Start Game   2. Fastest times");
	return;
}

void CallReceiver() {
	PORTA = PORTA | 0x04; //send signal to microcontroller #2 to display sequence on LED matrix
	while(!((PINA & 0x10) == 0x10)) {} //waiting for microcontroller #2 to send signal back after sequence was displayed
	PORTA = PORTA & 0xFB; //stop sending signal to microcontroller #2
	LCD_ClearScreen();
	
	return;
}

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

void loadRecords() {
	for(unsigned char i = 0; i < 3; ++i){
		recordArray[i] = eeprom_read_byte(( uint8_t *) i);
	}
	return;
}

void printSeconds(){
	for(unsigned char i = 0; i < 7; ++i) {
		LCD_WriteData(seconds[i]);
	}
	return;
}

void displayScores(){
	LCD_ClearScreen();
	LCD_Cursor(1);
	LCD_DisplayString(1, "1st Stage Record   ");
	tickToSeconds(recordArray[0]);
	printSeconds();
	_delay_ms(35000);
	LCD_ClearScreen();
	LCD_Cursor(1);
	LCD_DisplayString(1, "2nd Stage Record   ");
	tickToSeconds(recordArray[1]);
	printSeconds();
	_delay_ms(35000);
	LCD_ClearScreen();
	LCD_Cursor(1);
	LCD_DisplayString(1, "3rd Stage Record   ");
	tickToSeconds(recordArray[2]);
	printSeconds();
	_delay_ms(35000);
	return;
}

void tickToSeconds(int ticks){ 
	int tempTicks = ticks * 2;
	
	unsigned char hundreds = 0;
	for(unsigned char i = 0; i < 10; ++i){
		if(tempTicks >= 100) {
			tempTicks -= 100;
			++hundreds;
		}
	}
	
	if(hundreds != 0){ //To make the time look nicer
		LCD_WriteData(hundreds + '0');
	}
	
	unsigned char tens = 0;
	for(unsigned char i = 0; i < 10; ++i){
		if(tempTicks >= 10) {
			tempTicks -= 10;
			++tens;
		}
	}
	
	LCD_WriteData(tens + '0');
	
	LCD_WriteData('.');
	
	unsigned char ones = 0;
	for(unsigned char i = 0; i < 10; ++i){
		if(tempTicks > 0) {
			tempTicks -= 1;
			++ones;
		}
	}
	
	LCD_WriteData(ones + '0');
	
	return;
}

enum Game_States {Intro, Wait, Start, ShowScore, KeypadInput, CheckInput, Record} Game_State;
	
int Game_Tick(int Game_state){
	
	switch (Game_state){
		case Intro : 
			loadRecords();
			timerTick = 0;
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
			Game_state = Intro;
			break;
		case KeypadInput : 
			++timerTick; //increment time
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
			if((matrixStage < 3) && (recordArray[matrixStage - 1] > timerTick) && correctInput)
			{
				Game_state = Record;
			}
			else if((matrixStage < 3))
			{
				Game_state = Start;
			}
			else if(matrixStage == 3)
			{
				matrixStage = 0;
				Game_state = Intro; 
			}
			break;
		case Record :
			Game_state = Start;
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
			displayScores();
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
					LCD_DisplayString(1, "Completion time(seconds):");
					LCD_WriteData(' ');
					tickToSeconds(timerTick);
					correctInput = 0x01;
				}
				else {
					LCD_Cursor(1);
					LCD_DisplayString(1, "Wrong! the sequence is 3A6B");
					correctInput = 0x00;
				}
			}
			else if(matrixStage == 1)
			{
				if(checkSequence2()) {
					LCD_Cursor(1);
					LCD_DisplayString(1, "Completion time(seconds):");
					tickToSeconds(timerTick);
					correctInput = 0x01;
				}
				else {
					LCD_Cursor(1);
					LCD_DisplayString(1, "Wrong! the sequence is 8543AB");
					correctInput = 0x00;
				}
			}
			else if(matrixStage == 2)
			{
				if(checkSequence3()) {
					LCD_Cursor(1);
					LCD_DisplayString(1, "Completion time(seconds):");
					tickToSeconds(timerTick);
					correctInput = 0x01;
				}
				else {
					LCD_Cursor(1);
					LCD_DisplayString(1, "Wrong! the sequence is 626131");
					correctInput = 0x00;
				}
			}
			++matrixStage; //after last index
			_delay_ms(50000);
			break;
		case Record :
			LCD_Cursor(1);
			LCD_DisplayString(1, "A new record!");
			eeprom_write_byte((uint8_t *) (matrixStage-1), timerTick);
			_delay_ms(50000);
			break;
		default:
			break;
	}
	
	return Game_state;
}

static task Tasks[1];

int main(void)
{
    DDRA = 0x0F; PORTA = 0x00;  //for lcd PORTA = f0
	DDRB = 0x00; PORTB = 0xFF; //for buttons
    DDRC = 0xF0; PORTC = 0x0F; // PC7..4 for outputs, PC3..0 for inputs
    DDRD = 0xFF; PORTD = 0x00; //for lcd
	
	LCD_init();
	
	/*
	uncomment to reset values.
	eeprom_write_byte(( uint8_t *) 0, 99);
	eeprom_write_byte(( uint8_t *) 1, 99);
	eeprom_write_byte(( uint8_t *) 2, 99);    
	*/
	
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
	
}

