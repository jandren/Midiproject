/*
 * avrMIDI.c
 *
 * Includes UART and MIDI instructions
 *
 * Created: 2015-11-23 11:31:25
 *  Author: ASUS
 */

#include "main.c"
#include "avrMIDI.h"
/*
void send_MIDI(uint8_t command, uint8_t tone, uint8_t volume)
// Send the Midi command, not much really but abstracts the code a little.
{
	uint8_t m = 0b01111111; // Mask 8 bit to zero if it isn't
	UART_out(command & m); // Command 7-bit
	UART_out(tone & m); // Note 7bit
	UART_out(volume & m); // Velocity 7 bit
	
	// See Midi command table for exact bits to input.
}*/