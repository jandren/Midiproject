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

#define F_CPU 8000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "REC.h"
//#include "avrMIDI.h"
//#include "main.h"

void UART_out(uint8_t ch);
void Init_UART0(uint16_t BAUDRATE);
void Init_ports(void);


// For avrMIDI.c file
uint8_t MIDI_Conversion(uint8_t pressed);
void MIDI_send(uint8_t command, uint8_t tone);

void init_Timer0(void);
void init_Timer1(void);


// global variables
volatile uint16_t rx_ch = 0xFF;
volatile uint8_t switches = 0x00; // Byte containing [rec, play1, play2, play3, ch_bit0, ch_bit1, pitch_bit0, pitch_bit1];
extern volatile uint8_t volume;

int main(void)
{
	Init_ports();
	Init_UART0(9600);
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
		//switches = ~PINC; // Update switches
		PORTB = ~current; // rx_ch;
		
		change = current^previus;
		
		if(current & change) // Note on
		{
			command = 0b10010000 | (switches & 0x0C) >> 2;
			tone = MIDI_Conversion(current & change);
			
			if(switches & 0x80){
				REC_add(command, tone);
			}
			
			MIDI_send(command, tone);
			
			
			
			//UART_out(0b10010001); // Command
			//UART_out(60+Conversion(current & change)); // Note 7bit
			//UART_out(80 ); //volume(switches & 0x03)); // Velocity 7 bit
		}
		else if(previus & change) // Note off
		{
			command = 0b10010000 | (switches & 0x0C) >> 2;
			tone = MIDI_Conversion(previus & change);
						
			if(switches & 0x80){
				REC_add(command, tone);
			}
						
			MIDI_send(command, tone);
			
			
			
			//UART_out(0b10000001); // Command
			//UART_out(60+MIDI_Conversion(previus & change)); // Note 7bit
			//UART_out(0b01001000); // Velocity 7 bit
		}
		previus = current;
		//_delay_ms(300);
	}
	// End of while(1)
}

void MIDI_send(uint8_t command, uint8_t tone)
// Send the Midi command, not much really but abstracts the code a little.
{
	UART_out(command); // Command 7-bit
	UART_out(tone); // Note 7bit
	UART_out(volume >> 1); // Velocity 7 bit from 8 bit volume
	
	// See Midi command table for exact bits to input.
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

void init_adc(void)					// function for initializing ADC
{
	ADCSRA |= (1 << ADEN) | (1 << ADPS1);	// enable ADC, prescaler 4
	ADMUX |= (1 << REFS0) | (1 << ADLAR);	// internal 5V, 8 bits
}

ISR(ADC_vect)						// read ADC using interrupt
{
	volume = ADCH;									// save 8 bits
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
	TCNT0 = 0;						// reset the count TCNT0
}

void init_Timer1(void)				// for recording
{
	TCCR1A = 0b00000000;
	TCCR1B = 0b00000000;
	OCR1A = 0;
}
