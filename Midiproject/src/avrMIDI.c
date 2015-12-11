/*
 * avrMIDI.c
 *
 * Includes UART and MIDI instructions
 *
 * Functions:
 * Initialization of ports				(Init_ports)
 * Initialization of UART				(Init_UART0)
 * Read from a pin for UART				(UART_out)
 * Send a command through UART			(MIDI_send)
 * Conversion of music note from input	(MIDI_Conversion)
 * Initialization of ADC				(init_adc)
 * initialization of Timer0				(init_Timer0)
 *
 * Created: 2015-11-23 11:31:25
 *  Author: Jakob Andren & SJ
 */

#include "avrMIDI.h"

// global variables
volatile uint16_t rx_ch = 0xFF;
volatile uint8_t switches = 0x00; // Byte containing [rec, play1, play2, play3, ch_bit0, ch_bit1, pitch_bit0, pitch_bit1];

volatile uint8_t adc_read;
volatile uint8_t volume;

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

uint8_t MIDI_Conversion(uint16_t pressed)
{
	uint16_t converted = 0;				// music note
	uint8_t offset = 48;				// start from C4 = 48
	uint8_t pitch = switches & 0x03;	// mask with two far right switches
	
	if(switches & 0b00010000){  // Only flats
	
		switch(pressed)
		{
			case 0x0001:
			converted = 48 + 12 * pitch;
			break;				// C4 = 48 or C5 = 60 or C6 = 72
			case 0x0002:
			converted = 50 + 12 * pitch;
			break;				// D4 = 50 or D5 = 62 or D6 = 74
			case 0x0004:
			converted = 52 + 12 * pitch;
			break;				// E4 = 52 or E5 = 64 or E6 = 76
			case 0x0008:
			converted = 53 + 12 * pitch;
			break;				// F4 = 53 or F5 = 65 or F6 = 77
			case 0x0010:
			converted = 55 + 12 * pitch;
			break;				// G4 = 55 or G5 = 67 or G6 = 79
			case 0x0020:
			converted = 57 + 12 * pitch;
			break;				// A4 = 57 or A5 = 69 or A6 = 81
			case 0x0040:
			converted = 59 + 12 * pitch;
			break;				// B4 = 59 or B5 = 71 or B6 = 83
			case 0x0080:
			converted = 60 + 12 * pitch;
			break;				// C5 = 60 or C6 = 72 or C7 = 84
			case 0x0100:
			converted = 62 + 12 * pitch;
			break;				// D5 = 62 or D6 = 74 or D7 = 86
			case 0x0200:
			converted = 64 + 12 * pitch;
			break;				// E5 = 64 or E6 = 76 or E7 = 88
			case 0x0400:
			converted = 65 + 12 * pitch;
			break;				// F5 = 65 or F6 = 77 or F7 = 89
			case 0x0800:
			converted = 67 + 12 * pitch;
			break;				// G5 = 67 or G6 = 79 or G7 = 91
			case 0x1000:
			converted = 69 + 12 * pitch;
			break;				// A5 = 69 or A6 = 81 or A7 = 93
			case 0x2000:
			converted = 71 + 12 * pitch;
			break;				// B5 = 71 or B6 = 83 or B7 = 95
			case 0x4000:
			converted = 72 + 12 * pitch;
			break;				// C6 = 72 or C7 = 84 or C8 = 96
			case 0x8000:
			converted = 74 + 12 * pitch;
			break;				// D6 = 74 or D7 = 86 or D8 = 98
			default:
			break;
		}
	}
	else{  // All keys
		switch(pressed)
		{
			case 0x0001:
			converted = offset + 0 + 12 * pitch;
			break;				// C4 = 48 or C5 = 60 or C6 = 72
			case 0x0002:
			converted = offset + 1 + 12 * pitch;
			break;				// C4# = 49 or C5# = 61 or C6# = 73
			case 0x0004:
			converted = offset + 2 + 12 * pitch;
			break;				// D4 = 50 or D5 = 62 or D6 = 74
			case 0x0008:
			converted = offset + 3 + 12 * pitch;
			break;				// D4# = 51 or D5# = 63 or D6# = 75
			case 0x0010:
			converted = offset + 4 + 12 * pitch;
			break;				// E4 = 52 or E5 = 64 or E6 = 76
			case 0x0020:
			converted = offset + 5 + 12 * pitch;
			break;				// F4 = 53 or F5 = 65 or F6 = 77
			case 0x0040:
			converted = offset + 6 + 12 * pitch;
			break;				// F4# = 54 or F5# = 66 or F6# = 78
			case 0x0080:
			converted = offset + 7 + 12 * pitch;
			break;				// G4 = 55 or G5 = 67 or G6 = 79
			case 0x0100:
			converted = offset + 8 + 12 * pitch;
			break;				// G4# = 56 or G5# = 68 or G6# = 80
			case 0x0200:
			converted = offset + 9 + 12 * pitch;
			break;				// A4 = 57 or A5 = 69 or A6 = 81
			case 0x0400:
			converted = offset + 10 + 12 * pitch;
			break;				// A4# = 58 or A5# = 70 or A6# = 82
			case 0x0800:
			converted = offset + 11 + 12 * pitch;
			break;				// B4 = 59 or B5 = 71 or B6 = 83
			case 0x1000:
			converted = offset + 12 + 12 * pitch;
			break;				// C5 = 60 or C6 = 72 or C7 = 84
			case 0x2000:
			converted = offset + 13 + 12 * pitch;
			break;				// C5# = 61 or C6# = 73 or C7# = 85
			case 0x4000:
			converted = offset + 14 + 12 * pitch;
			break;				// D5 = 62 or D6 = 74 or D7 = 86
			case 0x8000:
			converted = offset + 15 + 12 * pitch;
			break;				// D5# = 63 or D6# = 75 or D7# = 87
			default:
			break;
			}
	}
	return converted;
	
}

void init_adc(void)					// function for initializing ADC
{
	ADCSRA |= (1 << ADEN) | (1 << ADPS1) | (1 << ADIE) | (1<<ADSC);	// enable ADC, prescaler 4
	ADMUX |= (1 << REFS0) | (1 << ADLAR);	// internal 5V, 8 bits
}

ISR(ADC_vect)						// read ADC using interrupt
{
	volume = ADCH;										// save 8 bits
	ADMUX = (ADMUX & 0xE0) | ((switches & 0x0C) >> 2);	// Mask and shift channel bits from switches
}

void init_Timer0(void)
{
	TCCR0A = 0b00000000;			// normal mode
	TCCR0B = 0b00000101;			// prescaler 1024
	TCNT0 = 0;						// overflow, 255*1024/(8*10^6) = 32.6ms delay at max
	TIMSK0 |= (1 << TOIE0);			// enable overflow interrupt enable
}

ISR(TIMER0_OVF_vect)
{
	ADCSRA |= (1<<ADSC);			// start the ADC conversion
	//TCNT0 = 0;					// reset the count TCNT0
}