/*
 * avrMIDI.c
 *
 * Includes UART and MIDI instructions
 *
 * Created: 2015-11-23 11:31:25
 *  Author: ASUS
 */

#include "avrMIDI.h"

// global variables
volatile uint16_t rx_ch = 0xFF;
volatile uint8_t switches = 0x00; // Byte containing [rec, play1, play2, play3, ch_bit0, ch_bit1, pitch_bit0, pitch_bit1];

void Init_ports(void)
{
	// Hard coded setup
	
	// Set up port B LEDS
	DDRB = 0xFF; // No inputs, all ports as outputs
	PORTB = 0xFF; // All LEDS are off
	
	// Set up port D Buttons
	DDRD = 0x00;
	PORTD = 0xFF; // Set pull up on the inputs
	
	// Set up port C Switches
	DDRC = 0x00;
	PORTC = 0xFF; // Set pull up on the inputs
	
}

void Init_UART0(uint16_t BAUDRATE)
{
	//ubrr = F_CPU/16uL/BAUDRATE;
	
	// Set baud rate to 9600 @ 8MHz
	UBRR0L = 51;
	UBRR0H = 0;
	
	// No parity bit, 8 bit char, one stop bit
	UCSR0A = 0x00;
	UCSR0B = 0b10011000;
	UCSR0C = 0b00000110;
}

ISR(USART0_RX_vect)
{
	rx_ch = UDR0;             //read UART register into value
}

void UART_out(uint8_t ch)
{
	while((UCSR0A & (1<<UDRE0)) == 0); // Wait until empty
	
	UDR0 = ch; // Put character to be sent in the output register
}

void MIDI_send(uint8_t command, uint8_t tone, uint8_t volume)
// Send the Midi command, not much really but abstracts the code a little.
{
	UART_out(command); // Command 7-bit
	UART_out(tone); // Note 7bit
	UART_out(volume >> 1); // Velocity 7 bit from 8 bit volume
	
	// See Midi command table for exact bits to input.
}

uint8_t MIDI_Conversion(uint8_t pressed)
{
	uint8_t converted = 0;
	uint8_t offset = 48;
	uint8_t pitch = switches & 0x03;		// mask with two far right switches

/*
	if( pressed &  0x01){
		 converted = offset + 0 + 12 * pitch;
		 			// C4 = 48 or C5 = 60 or C6 = 72
		 // sendMIDI
	}
	if( pressed &  0x02){
		 converted = offset + 2 + 12 * pitch;
		 			// D4 = 50 or D5 = 62 or D6 = 74
	}
	if( pressed &  0x04) converted = offset + 4 + 12 * pitch;
		 			// E4 = 52 or E5 = 64 or E6 = 76
	if( pressed &  0x08) converted = offset + 5 + 12 * pitch;
		 			// F4 = 53 or F5 = 65 or F6 = 77
	if( pressed &  0x10) converted = offset + 7 + 12 * pitch;
		 			// G4 = 55 or G5 = 67 or G6 = 79
	if( pressed &  0x20) converted = offset + 9 + 12 * pitch;
		 			// A4 = 57 or A5 = 69 or A6 = 81
	if( pressed &  0x40) converted = offset + 11 + 12 * pitch;
		 			// B4 = 59 or B5 = 71 or B6 = 83
	if( pressed &  0x80) converted = offset + 12 + 12 * pitch;
		 			// C5 = 60 or C6 = 72 or C7 = 84
*/

	switch(pressed)
	{
		case 0x01:
		converted = 48 + 12 * pitch;
		break;				// C4 = 48 or C5 = 60 or C6 = 72
		case 0x02:
		converted = 50 + 12 * pitch;
		break;				// D4 = 50 or D5 = 62 or D6 = 74
		case 0x04:
		converted = 52 + 12 * pitch;
		break;				// E4 = 52 or E5 = 64 or E6 = 76
		case 0x08:
		converted = 53 + 12 * pitch;
		break;				// F4 = 53 or F5 = 65 or F6 = 77
		case 0x10:
		converted = 55 + 12 * pitch;
		break;				// G4 = 55 or G5 = 67 or G6 = 79
		case 0x20:
		converted = 57 + 12 * pitch;
		break;				// A4 = 57 or A5 = 69 or A6 = 81
		case 0x40:
		converted = 59 + 12 * pitch;
		break;				// B4 = 59 or B5 = 71 or B6 = 83
		case 0x80:
		converted = 60 + 12 * pitch;
		break;				// C5 = 60 or C6 = 72 or C7 = 84
		default:
		break;
	}
	
	return converted;
}