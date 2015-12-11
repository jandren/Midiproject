/*
* main.c
* Description:
* Midi keyboard, control any MIDI compatible synthesizer.
* Sends MIDI commands over serial following the MIDI standard.

* Hardware:
*INPUTS:
* Push Buttons: L&S board 1 for upper 8 notes PORTA.0 ~ PORTA.7
* Push Buttons: L&S board 2 for lower 8 notes PORTD.0 ~ PORTD.7
* Switches: L&S board 1 for changing pitches PORTC.0 ~ PORTC.1
* Switches: L&S board 1 for changing instruments PORTC.2 ~ PORTC.3
* Switches: L&S board 1 for flat/full keyboard PORTC.4
* Switches: L&S board 1 for playing back records PORTC.6
* Switches: L&S board 1 for recording the music PORTC.7
* Sliders from Potentiometer board PORTF.0 ~ PORTF.3
*
*OUTPUTS
* Serial to USB interface PORTE.0, PORTE.1
* LEDs: STK600 board PORTB.0 ~ PORTB.7
*
* Created: 9/13/2015
* Author: Jakob Andren & SJ
* Ver: 0.1
*/

#include "main.h"

int main(void)
{
	Init_ports();
	Init_UART0(9600);
	init_Timer1();
	init_Timer0();
	init_adc();
	sei();
	
	// Pressed buttons
	uint16_t previus = 0x0000;
	uint16_t current = 0x0000;
	uint16_t change  = 0x0000;
	
	// MIDI related variables
	uint8_t command = 0;
	uint8_t tone = 0;
	
	while
	(1)
	{
		
		current = (PINA) | (PIND << 8);
		switches = PINC; // Update switches
		REC_state(switches);
		REC_POLL(0);
		//PORTB = current; // rx_ch;
		
		change = current^previus;
		
		if(current & change) // Note on
		{
			command = 0b10010000 | (switches & 0x0C) >> 2;
			tone = MIDI_Conversion(current & change);
			
			REC_add(command, tone);
			
			MIDI_send(command, tone, volume);
			
			
		}
		else if(previus & change) // Note off
		{

			command = 0b10000000 | ((switches & 0x0C) >> 2);
			
			tone = MIDI_Conversion(previus & change);
						
			REC_add(command, tone);
			MIDI_send(command, tone, volume);
			
		}
		previus = current;
		//_delay_ms(300);
	}
	// End of while(1)
}
