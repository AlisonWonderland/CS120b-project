/*
 * transmitter_project.c
 *
 * Created: 3/5/2019 1:57:33 PM
 * Author : dimitri
 */ 

#include <avr/io.h>
#include "io.c"
#include "timer.h"

unsigned char GetBit( unsigned char x, unsigned char k) {
	return ((x & (0x01 << k)) != 0);
}

typedef struct task {
	/*Tasks should have members that include: state, period,
	a measurement of elapsed time, and a function pointer.*/
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

void DisplayMenu() {
	LCD_Cursor(1);
	LCD_DisplayString(1, "1. Start Game");
	//add 2. See fastest completion time
	return;
}

void CallReceiver() {
	//wait for signal back
	//A3 is output already changed port
	//A4 is input
	PORTA = PORTA | 0x04;
	while(!((~PINA & 0x08) == 0x08)) {
	}
	LCD_Cursor(1);
	LCD_DisplayString(1, "Received");
	PORTA = PORTA & 0xFB;
	
	return;
}

unsigned char waitTick = 0;
unsigned char timerTick = 0;

enum Game_States {Intro, Wait, Start, ShowScore, KeypadInput, CheckInput} Game_State;
int Game_Tick(int Game_state){
	switch (Game_state){
		case Intro : 
			DisplayMenu();
			Game_state = Wait;
			break;
		case Wait : //if user holds button for more than .5 seconds then showscore else start game
			if((~PINB & 0x01) == 0x01) {
				Game_state = Start;
			}
			else if((~PINB & 0x08) == 0x08) {
				Game_state = ShowScore;
			}
			break;
		case Start : //display matrix
			//Game_state = UserInput;
			CallReceiver();
			Game_state = KeypadInput;
			break;
		case ShowScore :
			Game_state = Wait;
			break;
		case KeypadInput : 
			++timerTick;
			Game_state = Wait;
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
		case ShowScore :
			LCD_ClearScreen();
			LCD_Cursor(1);
			LCD_DisplayString(1, "Showing score");
		case KeypadInput :
			break;
		default:
			break;
	}
	
	return Game_state;
}


static task Tasks[1];

int main(void)
{
    DDRA = 0x07; PORTA = 0xF8;  //for lcd
	DDRB = 0x00; PORTB = 0xFF;
    DDRC = 0xF0; PORTC = 0x0F; // PC7..4 outputs init 0s, PC3..0 inputs init 1s
    DDRD = 0xFF; PORTD = 0x00; //for lcd
    LCD_init();
    
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

