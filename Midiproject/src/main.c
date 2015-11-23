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
* Author: Jakob Andren
* Ver: 0.1
*/

#define F_CPU 8000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "avrMIDI.h"
#include "main.h"


int main(void)
{
	Init_ports();
	Init_UART0(9600);
	sei();
	uint8_t previus = 0x00;
	uint8_t current = 0x00;
	uint8_t change  = 0x00;
	uint8_t channel = 0x01; // Send on channel 2
	
	while
	(1)
	{
		
		current = ~PIND;
		PORTB = ~current; // rx_ch;
		
		change = current^previus;
		
		if(current & change) // Note on
		{
			UART_out(0b10010001); // Command
			UART_out(60+Conversion(current & change, 0x00)); // Note 7bit
			UART_out(80 ); //volume(switches & 0x03)); // Velocity 7 bit
		}
		else if(previus & change) // Note off
		{
			UART_out(0b10000001); // Command
			UART_out(60+Conversion(previus & change, 0x00)); // Note 7bit
			UART_out(0b01001000); // Velocity 7 bit
		}
		previus = current;
		//_delay_ms(300);
	}
	// End of while(1)
}

ISR(USART0_RX_vect){
	rx_ch = UDR0;             //read UART register into value
}

void UART_out(uint8_t ch){
	while((UCSR0A & (1<<UDRE0)) == 0); // Wait until empty
	
	UDR0 = ch; // Put character to be sent in the output register
}

void Init_UART0(uint16_t BAUDRATE){
	//ubrr = F_CPU/16uL/BAUDRATE;
	
	// Set baud rate to 9600 @ 8MHz
	UBRR0L = 51;
	UBRR0H = 0;
	
	// No parity bit, 8 bit char, one stop bit
	UCSR0A = 0x00;
	UCSR0B = 0b10011000;
	UCSR0C = 0b00000110;
}





void Init_ports(void){
	// Hard coded setup
	
	// Set up port B LEDS
	DDRB = 0xFF; // No inputs, all ports as outputs
	PORTB = 0xFF; // All LEDS are off
	
	// Set up port D Buttons
	DDRD = 0x00;
	PORTD = 0xFF; // Set pull up on the inputs
}

uint8_t Conversion(uint8_t pressed, uint8_t switches)
{
	uint8_t converted;
	uint8_t mask;
	switch(pressed)
	{
		case 0x01:
		converted = 0;
		break;				// C
		case 0x02:
		converted = 2;
		break;				// D
		case 0x06:
		converted = 3;
		break;				// D# or Eb
		case 0x04:
		converted = 4;
		break;				// E
		case 0x08:
		converted = 5;
		break;				// F
		case 0x18:
		converted = 6;
		break;				// F# or Gb
		case 0x10:
		converted = 7;
		break;				// G
		case 0x30:
		converted = 8;
		break;				// G# or Ab
		case 0x20:
		converted = 9;
		break;				// A
		case 0x60:
		converted = 10;
		break;				// A# or Bb
		case 0x40:
		converted = 11;
		break;				// B
		case 0x80:
		converted = 12;
		break;				// C
		default:
		break;
	}
	
	mask = ((switches & 0x0C) >> 2);
	converted = converted + 48 + 12 * mask; 
	// mask = 0, start 48~60
		
	return converted;
}