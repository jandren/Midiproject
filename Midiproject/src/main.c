/*
* serialMidi.c
* Description:
* Pressing button SW0-2 lights up the corresponding LEDS for 0.5s
* Repeat

* Hardware:
* Outputs: PORTB 0~7 connected to STK600 LEDs 0~7 (active low)
* Inputs: PORTD 0~7 connected to STK600 pushbuttons 0~7 (active low)
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
	uint8_t previus = 0x00;
	uint8_t current = 0x00;
	uint8_t change  = 0x00;
	
	// MIDI related variables
	uint8_t command = 0;
	uint8_t tone = 0;
	
	while
	(1)
	{
		current = ~PIND;
		switches = ~PINC; // Update switches
		REC_state(switches);
		//PORTB = ~switches; // rx_ch;
		
		change = current^previus;
		
		if(current & change) // Note on
		{
			command = 0b10010000 | (switches & 0x0C) >> 2;
			tone = MIDI_Conversion(current & change);
			
			REC_add(command, tone);
			
			MIDI_send(command, tone, volume);
			
			
			
			//UART_out(0b10010001); // Command
			//UART_out(MIDI_Conversion(current & change)); // Note 7bit
			//UART_out(80 ); //volume(switches & 0x03)); // Velocity 7 bit
		}
		else if(previus & change) // Note off
		{

			command = 0b10000000 | ((switches & 0x0C) >> 2);
			
			tone = MIDI_Conversion(previus & change);
						
			REC_add(command, tone);
			MIDI_send(command, tone, volume);
			
			
			
			//UART_out(0b10000001); // Command
			//UART_out(MIDI_Conversion(previus & change)); // Note 7bit
			//UART_out(0b01001000); // Velocity 7 bit


		}
		previus = current;
		//_delay_ms(300);
	}
	// End of while(1)
}