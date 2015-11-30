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

void init_adc(void);
void init_Timer0(void);
void init_Timer1(void);
void Set_ISR(uint16_t time);

// global variables
volatile uint8_t adc_read;
volatile uint16_t software_time = 0;
volatile uint16_t software_comp = 0;


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
			
			REC_process(switches, command, tone);
			
			MIDI_send(command, tone);
			
			
			
			//UART_out(0b10010001); // Command
			//UART_out(60+Conversion(current & change)); // Note 7bit
			//UART_out(80 ); //volume(switches & 0x03)); // Velocity 7 bit
		}
		else if(previus & change) // Note off
		{

			command = 0b10010000 | (switches & 0x0C) >> 2;
			tone = MIDI_Conversion(previus & change);
						
			REC_process(switches, command, tone);
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
	TCCR1A = 0b00000000;			// normal mode
	TCCR1B = 0b00000101;			// prescaler 1024
	OCR1A = 65535;					// output compare, 65525*1024/(8*10^6) = 8.4s at max
}

ISR(TIMER1_COMPA_vect)
{
	software_time++;
	if(software_time == software_comp)
	{
		REC_ISR(software_time);
	}
}

void Set_ISR(uint16_t time)
{
	software_comp = time;
}

