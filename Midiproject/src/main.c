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
//#include "avrMIDI.h"
//#include "main.h"



void UART_out(uint8_t ch);
void Init_UART0(uint16_t BAUDRATE);
void Init_ports(void);
uint8_t Conversion(uint8_t pressed);

// global variables
volatile uint16_t rx_ch = 0xFF;
volatile uint8_t switches = 0x00; // Byte containing [rec, play1, play2, play3, ch_bit0, ch_bit1, pitch_bit0, pitch_bit1];

int main(void)
{
	Init_ports();
	Init_UART0(9600);
	sei();
	
	// Pressed buttons
	uint8_t previus = 0x00;
	uint8_t current = 0x00;
	uint8_t change  = 0x00;
	
	while
	(1)
	{
		
		current = ~PIND;
		//switches = ~PINC; // Update switches
		PORTB = ~current; // rx_ch;
		
		change = current^previus;
		
		if(current & change) // Note on
		{
			// I want add stuff here
			UART_out(0b10010001); // Command
			UART_out(60+Conversion(current & change)); // Note 7bit
			UART_out(80 ); //volume(switches & 0x03)); // Velocity 7 bit
		}
		else if(previus & change) // Note off
		{
			UART_out(0b10000001); // Command
			UART_out(60+Conversion(previus & change)); // Note 7bit
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

uint8_t Conversion(uint8_t pressed)
{
	uint8_t converted = 48;
	uint8_t offset = 48;
	uint8_t pitch = switches & 0x03;		// mask with two far right switches

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


/*	switch(pressed)
	{
		case 0x01:
		converted = 48 + 12 * switches;
		break;				// C4 = 48 or C5 = 60 or C6 = 72
		case 0x02:
		converted = 50 + 12 * switches;
		break;				// D4 = 50 or D5 = 62 or D6 = 74
		case 0x04:
		converted = 52 + 12 * switches;
		break;				// E4 = 52 or E5 = 64 or E6 = 76
		case 0x08:
		converted = 53 + 12 * switches;
		break;				// F4 = 53 or F5 = 65 or F6 = 77
		case 0x10:
		converted = 55 + 12 * switches;
		break;				// G4 = 55 or G5 = 67 or G6 = 79
		case 0x20:
		converted = 57 + 12 * switches;
		break;				// A4 = 57 or A5 = 69 or A6 = 81
		case 0x40:
		converted = 59 + 12 * switches;
		break;				// B4 = 59 or B5 = 71 or B6 = 83
		case 0x80:
		converted = 60 + 12 * switches;
		break;				// C5 = 60 or C6 = 72 or C7 = 84
		default:
		break;
	}*/
	
	return converted;
}

void init_adc(void)					// function for initializing ADC
{
	ADCSRA |= (1 << ADEN) | (1 << ADPS1);	// enable ADC, prescaler 4
	ADMUX |= (1 << REFS0);					// internal 5V, Channel 0
}

uint16_t read_ADC()			// function for reading ADC
{
	uint8_t channel = (switches & 0x0C) >> 2; // Mask and shift channelbits from switches. 
	uint16_t adc_value;
	ADCSRA |= (1 << ADSC);			// start conversion
	ADMUX = (ADMUX & 0xE0) | channel;		// copy reference bit, merge with channel
	while((ADCSRA & (1<< ADIF)) == 0);		// wait until ADIF flag is set
	adc_value = ADCL;				// save lower 8 bits
	ADCSRA |= (1 << ADIF);			// reset ADIF flag
	return(adc_value);				// return with ADC value
}

/*
void init_INT(void)
{
	EICRA |= (1 << ISC11) | (1 << ISC10) | (1 << ISC01) | (1 << ISC00);			// rising edge for both INT0 and INT1
	EIMSK |= (1 << INT0) | (1 << INT1);			// INT0 and INT1
}

ISR (INT0_vect)
{
	if((channel < 3) && (channel >= 0))	// range of 0 and 3
	{
		channel++;				// incrementing
	}
}

ISR (INT1_vect)
{
	if((channel <= 3) && (channel > 0))	// range of 0 and 3
	{
		channel--;				// decrementing
	}
}
*/