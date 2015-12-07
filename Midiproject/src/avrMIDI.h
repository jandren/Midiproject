/*
 * avrMIDI.h
 *
 * Includes UART and MIDI instructions
 *
 * Created: 2015-11-23 11:31:45
 *  Author: Jakob Andren, SJ
 */ 


#ifndef AVRMIDI_H_
#define AVRMIDI_H_

#define F_CPU 8000000UL

#include <util/delay.h>
#include <avr/interrupt.h>

// Global Variables
extern volatile uint16_t rx_ch;
extern volatile uint8_t switches;
extern volatile uint8_t adc_read;
extern volatile uint8_t volume;

// Functions
void Init_ports(void);
void UART_out(uint8_t ch);
void Init_UART0(uint16_t BAUDRATE);
void MIDI_send(uint8_t command, uint8_t tone, uint8_t volume);
uint8_t MIDI_Conversion(uint16_t pressed);

void init_adc(void);
void init_Timer0(void);

//void send_MIDI(uint8_t command, uint8_t tone, uint8_t volume);

#endif /* AVRMIDI_H_ */