/*
 * Ir Remote 3.0.cpp
 *
 * Created: 7/8/2018 11:06:00 PM
 * Author : KanKam
 */ 

#include "main.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/atomic.h>
#include <util/delay.h>




const char *digPin[4] = {digPin1, digPin2, digPin3, digPin4};
const char *LEDPin[7] = {LEDPinA, LEDPinB, LEDPinC, LEDPinD, LEDPinE, LEDPinF, LEDPinG};
const char *LedPin = irLed;
	
uint16_t counter = 5;    //Default counter is set to 5
uint16_t Digit = 0;      //Display Digit
uint16_t shot = 0;       //Shot count
uint64_t lastCount = 0;  //Time stamp for timing

void digitalWrite(const char *port, bool state){
	//Just like the Arduino,Yeah~!
	int pin =  port[1] - '0';
	if(state == 1)
	{
		switch (port[0]){
			case 'B':
			PORTB |= (1<<pin);
			break;
			case 'C':
			PORTC |= (1<<pin);
			break;
			case 'D':
			PORTD |= (1<<pin);
			break;
			}
		}else{
			switch (port[0]){
				case 'B':
				PORTB &= ~(1<<pin);
				break;
				case 'C':
				PORTC &= ~(1<<pin);
				break;
				case 'D':
				PORTD &= ~(1<<pin);
				break;
				}
		}
}

void shutterNow(void){
	//Ir signal output for Sony camera
	//Source(Author): http://sebastian.setz.name/arduino/my-libraries/multiCameraIrControl (Sebastian Setz)
	bool _seq[] = {1,0,1,1,0,1,0,0,1,0,1,1,1,0,0,0,1,1,1,1}; 
	for (uint8_t j = 0; j < 3; j++) {
		//_freq = 40;
		//pause = 8;
		//high(2320,_freq,_pin);
		for (uint8_t i = 0; i < 145; i++){
			digitalWrite(LedPin,1);
			_delay_us(8);
			digitalWrite(LedPin,0);
			_delay_us(8);
		}
		_delay_us(650);
		for (uint8_t i = 0; i < sizeof(_seq); i++){
			if (_seq[i]==0){
				//high(575,_freq,_pin);
				for (uint8_t i = 0; i < 35; i++){
					digitalWrite(LedPin,1);
					_delay_us(8);
					digitalWrite(LedPin,0);
					_delay_us(8);
				}
				_delay_us(650);
			}
			else{
				//high(1175,_freq,_pin);
				for (uint8_t i = 0; i < 73; i++){
					digitalWrite(LedPin,1);
					_delay_us(8);
					digitalWrite(LedPin,0);
					_delay_us(8);
				}
				_delay_us(650);
				_delay_us(650);
			}
		}
		_delay_us(10000);
	}
}
	
void DisplaySingleDigit(uint8_t digit){
	{
		//Display Single Digit on 7 segment display
		//Conditions for displaying segment a
		if(digit!=1 && digit != 4)
		digitalWrite(LEDPin[0],1);
			
		//Conditions for displaying segment b
		if(digit != 5 && digit != 6)
		digitalWrite(LEDPin[1],1);
			
		//Conditions for displaying segment c
		if(digit !=2)
		digitalWrite(LEDPin[2],1);
			
		//Conditions for displaying segment d
		if(digit != 1 && digit !=4 && digit !=7)
		digitalWrite(LEDPin[3],1);
			
		//Conditions for displaying segment e
		if(digit == 2 || digit ==6 || digit == 8 || digit==0)
		digitalWrite(LEDPin[4],1);
			
		//Conditions for displaying segment f
		if(digit != 1 && digit !=2 && digit!=3 && digit !=7)
		digitalWrite(LEDPin[5],1);
		//Conditions for displaying segment g
		if (digit!=0 && digit!=1 && digit !=7)
		digitalWrite(LEDPin[6],1);
	}
}
	
void turnOff(void){
	//Clear 7 segment display
	for(uint8_t i = 0; i < 7; i++){
		digitalWrite(LEDPin[i],0);
	}
}

void displayDigit(uint16_t digit){
	//Display a 4 digit number
	uint16_t fourDigit = digit;
	uint8_t InDigit[4] = {0,0,0,0};
		
	for(int8_t i = 3; i > -1 ; i--) {
		//Store a 4 digit number in a array
		if(fourDigit <= 0){
		InDigit[i] = 0;
		}else {
			InDigit[i] = fourDigit % 10;
			fourDigit /= 10;}
	}
	//7 segment display take turns to display their digit respectively
	for (int b=0; b<4; b++){
		turnOff();
		for(int a=0; a<4; a++){
			digitalWrite(digPin[a],1);
		}
		digitalWrite(digPin[b],0);
		DisplaySingleDigit(InDigit[b]);
		_delay_ms(5);		
	}
}
/*
*avr-millis-function 
*https://github.com/monoclecat/avr-millis-function
*/
volatile unsigned long timer1_millis;

ISR(TIMER1_COMPA_vect)
{
	timer1_millis++;
}

void init_millis(unsigned long f_cpu)
{
	unsigned long ctc_match_overflow;
	
	ctc_match_overflow = ((f_cpu / 1000) / 8); //when timer1 is this value, 1ms has passed
	
	// (Set timer to clear when matching ctc_match_overflow) | (Set clock divisor to 8)
	TCCR1B |= (1 << WGM12) | (1 << CS11);
	
	// high byte first, then low byte
	OCR1AH = (ctc_match_overflow >> 8);
	OCR1AL = ctc_match_overflow;
	
	// Enable the compare match interrupt
	TIMSK1 |= (1 << OCIE1A);
	
	//REMEMBER TO ENABLE GLOBAL INTERRUPTS AFTER THIS WITH sei(); !!!
}

unsigned long millis ()
{
	unsigned long millis_return;
	
	// Ensure this cannot be disrupted
	ATOMIC_BLOCK(ATOMIC_FORCEON) {
		millis_return = timer1_millis;
	}
	return millis_return;
}
/*
*
*/

int main(void)
{
    //Set up GPIO
	//Set them all as output and low
    DDRB = 0xFF;
    PORTB = 0x00;
    DDRC = 0xFF;
    PORTC = 0x00;
    DDRD = 0xFF;
    PORTD = 0x00;
	
	//Set up UP/Down buttons
	DDRC &= ~(1 << DDC2);   //PC2
	PORTC |= (1 << PORTC2); // turn On the Pull-up
	DDRC &= ~(1 << DDC3);   //PC3
	PORTC |= (1 << PORTC3); // turn On the Pull-up
	
	//Set up State Switch
	//Set PC1 (PCINT9 pin) as an input with pull-up enabled
	DDRC &= ~(1 << DDC1);          // Clear the PC1 pin	
	PORTC |= (0 << PORTC1);        // turn On the Pull-down
	PCICR |= (1 << PCIE1);         // set PCIE1 to enable PCMSK1 scan
	PCMSK1 |= (1 << PCINT9);       // set PCINT9 to trigger an interrupt on state change
	
	//run setting mode once if boot with modeSwitch set to 1
	uint16_t delay = 500;
	while (PINC & (1<<PINC1)){		
		displayDigit(counter);
		turnOff();
		if ((PINC & (1<<PINC2))){
			if(counter < 9999){
				counter++;
			}
			if(delay>=5){
				//increase rate when long press
				delay = delay*0.8;
			}
			_delay_ms(delay);
		}
		else if ((PINC & (1<<PINC3))){
			if(counter > 0){
				counter--;
			}
			if(delay>=5){
				//increase rate when long press
				delay = delay*0.8;
			}
			_delay_ms(delay);
			}else{
			delay = 500;
		}
	}
	//Turn on millis counter
	init_millis(F_CPU);
	// turn on interrupts
	sei();
	
    while (1){ //main loop
		/*
		*Note to User:
		*The code is decide for the shutter set in bulb mode.
		*Therefore, counter should be set to shutter - 1 second if shutter time on camera is preset.
		*/
		lastCount = millis();
	    while (millis() < (lastCount + 3000)){
		    displayDigit(shot); //Display shot count, buffer time is set to 3s
	    }
	    shutterNow();//Bulb mode on camera: Open shutter/ Shutter time is preset: Take a photo

	    Digit = counter;
	    while( Digit > 0){
			lastCount = millis();
			while (millis() < (lastCount + 1000)){
				displayDigit(Digit);
			}
			Digit--; //Digit -1 for every second
		}
	    shutterNow();//Bulb mode on camera: Close shutter/ Shutter time is preset: Do nothing

    }
	return 0;
}


ISR (PCINT1_vect)
{
	//Enter setting mode with interrupt
	uint16_t delay = 500;
	while (PINC & (1<<PINC1)){
		displayDigit(counter);
		turnOff();
		if ((PINC & (1<<PINC2))){
			if(counter < 9999){
				counter++;
			}
			if(delay>=5){
				//increase rate when long press
				delay = delay*0.8;
			}
			_delay_ms(delay);
		}
		else if ((PINC & (1<<PINC3))){
			if(counter > 0){
				counter--;
			}
			if(delay>=5){
				//increase rate when long press
				delay = delay*0.8;
			}
			_delay_ms(delay);
			}else{
			delay = 500;
		}
	} 
	//Exit setting mode
	Digit = counter;
	shot = 0;
	
}
